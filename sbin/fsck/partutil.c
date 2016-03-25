/*	$NetBSD: partutil.c,v 1.12.6.1 2015/05/25 09:10:48 msaitoh Exp $	*/

/*-
 * Copyright (c) 2006 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
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

#include <sys/cdefs.h>
__RCSID("$NetBSD: partutil.c,v 1.12.6.1 2015/05/25 09:10:48 msaitoh Exp $");

#include <sys/types.h>
#include <sys/param.h>
#include <sys/diskinfo.h>
#include <sys/disk.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <util.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <prop/proplib.h>

#include "partutil.h"

/*
 * Set what we need to know about disk geometry.
 */
static void
dict2geom(struct disk_geom *geo, prop_dictionary_t dict)
{
	(void)memset(geo, 0, sizeof(struct disk_geom));
	prop_dictionary_get_int64(dict, "sectors-per-unit",
	    &geo->dg_secperunit);
	prop_dictionary_get_uint32(dict, "sector-size", &geo->dg_secsize);
	prop_dictionary_get_uint32(dict, "sectors-per-track",
	    &geo->dg_nsectors);
	prop_dictionary_get_uint32(dict, "tracks-per-cylinder",
	    &geo->dg_ntracks);
	prop_dictionary_get_uint32(dict, "cylinders-per-unit",
	    &geo->dg_ncylinders);
}


int
getdiskinfo(const char *s, int fd, struct disk_geom *geo)
{
	struct disklabel lab;
	struct disklabel *lp = &lab;
	prop_dictionary_t disk_dict, geom_dict;
	struct stat sb;
	int ptn;

	/* Get disk description dictionary */
	if (prop_dictionary_recv_ioctl(fd, DIOCGDISKINFO, &disk_dict)) {
		warn("DIOCGDISKINFO on %s failed", s);
		return -1;
	}
	geom_dict = prop_dictionary_get(disk_dict, "geometry");
	dict2geom(geo, geom_dict);

	if (stat(s, &sb) == -1)
		return 0;

	ptn = strchr(s, '\0')[-1] - 'a';
	if ((unsigned)ptn >= lp->d_npartitions ||
	    (devminor_t)ptn != DISKPART(sb.st_rdev))
		return 0;

	return 0;
}

int
getdisksize(const char *name, u_int *secsize, off_t *mediasize)
{
	char buf[MAXPATHLEN];
	struct disk_geom geo;
	int fd, error;

	if ((fd = opendisk(name, O_RDONLY, buf, sizeof(buf), 0)) == -1)
		return -1;

	error = getdiskinfo(name, fd, &geo);
	close(fd);
	if (error)
		return error;

	*secsize = geo.dg_secsize;
	*mediasize = geo.dg_secsize * geo.dg_secperunit;
	return 0;
}
