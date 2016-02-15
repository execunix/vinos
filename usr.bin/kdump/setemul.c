/*	$NetBSD: setemul.c,v 1.29 2011/04/26 16:57:42 joerg Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: setemul.c,v 1.29 2011/04/26 16:57:42 joerg Exp $");
#endif /* not lint */

#include <sys/param.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/queue.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vis.h>

#include "setemul.h"

#include <sys/syscall.h>

#define KTRACE
#include "../../sys/kern/syscalls.c"

#undef KTRACE

#define NELEM(a) (sizeof(a) / sizeof(a[0]))

/* static */
const struct emulation emulations[] = {
	{ "netbsd",	syscallnames,		SYS_MAXSYSCALL,
	  NULL,				0,
	  NULL,				0,	0 },

	{ NULL,		NULL,			0,
	  NULL,				0,
	  NULL,				0,	0 }
};

struct emulation_ctx {
	pid_t	pid;
	const struct emulation *emulation;
	LIST_ENTRY(emulation_ctx) ctx_link;
};

const struct emulation *cur_emul;
const struct emulation *prev_emul;

static const struct emulation *default_emul = &emulations[0];

struct emulation_ctx *current_ctx;
static LIST_HEAD(, emulation_ctx) emul_ctx =
	LIST_HEAD_INITIALIZER(emul_ctx);

static struct emulation_ctx *ectx_find(pid_t);
static void	ectx_update(pid_t, const struct emulation *);

void
setemul(const char *name, pid_t pid, int update_ectx)
{
	int i;
	const struct emulation *match = NULL;

	for (i = 0; emulations[i].name != NULL; i++) {
		if (strcmp(emulations[i].name, name) == 0) {
			match = &emulations[i];
			break;
		}
	}

	if (!match) {
		warnx("Emulation `%s' unknown", name);
		return;
	}

	if (update_ectx)
		ectx_update(pid, match);
	else
		default_emul = match;

	if (cur_emul != NULL) 
		prev_emul = cur_emul;
	else
		prev_emul = match;

	cur_emul = match;
}

/*
 * Emulation context list is very simple chained list, not even hashed.
 * We expect the number of separate traced contexts/processes to be
 * fairly low, so it's not worth it to optimize this.
 * MMMmmmm not when I use it, it is only bounded PID_MAX!
 * Requeue looked up item at start of list to cache result since the
 * trace file tendes to have a burst of calls for a single process.
 */

/*
 * Find an emulation context appropriate for the given pid.
 */
static struct emulation_ctx *
ectx_find(pid_t pid)
{
	struct emulation_ctx *ctx;

	/* Find an existing entry */
	LIST_FOREACH(ctx, &emul_ctx, ctx_link) {
		if (ctx->pid == pid)
			break;
	}

	if (ctx == NULL) {
		/* create entry with default emulation */
		ctx = malloc(sizeof *ctx);
		if (ctx == NULL)
			err(1, "malloc emul context");
		ctx->pid = pid;
		ctx->emulation = default_emul;

		/* chain into the list */
		LIST_INSERT_HEAD(&emul_ctx, ctx, ctx_link);
	} else {
		/* move entry to head to optimize lookup for syscall bursts */
		LIST_REMOVE(ctx, ctx_link);
		LIST_INSERT_HEAD(&emul_ctx, ctx, ctx_link);
	}

	return ctx;
}

/*
 * Update emulation context for given pid, or create new if no context
 * for this pid exists.
 */
static void
ectx_update(pid_t pid, const struct emulation *emul)
{
	struct emulation_ctx *ctx;

	ctx = ectx_find(pid);
	ctx->emulation = emul;
}

/*
 * Ensure current emulation context is correct for given pid.
 */
void
ectx_sanify(pid_t pid)
{
	struct emulation_ctx *ctx;

	ctx = ectx_find(pid);
	cur_emul = ctx->emulation;
}

/*
 * Delete emulation context for current pid.
 * (eg when tracing exit())
 * Defer delete just in case we've cached a pointer...
 */
void
ectx_delete(void)
{
	static struct emulation_ctx *ctx = NULL;

	if (ctx != NULL)
		free(ctx);

	/*
	 * The emulation for current syscall entry is always on HEAD, due
	 * to code in ectx_find().
	 */
	ctx = LIST_FIRST(&emul_ctx);

	if (ctx)
		LIST_REMOVE(ctx, ctx_link);
}
