/*	$NetBSD: dkctl.c,v 1.20.20.1 2014/11/11 10:36:40 martin Exp $	*/

/*
 * Copyright 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * dkctl(8) -- a program to manipulate disks.
 */
#include <sys/cdefs.h>

#ifndef lint
__RCSID("$NetBSD: dkctl.c,v 1.20.20.1 2014/11/11 10:36:40 martin Exp $");
#endif


#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/dkio.h>
#include <sys/disk.h>
#include <sys/queue.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     
#include <unistd.h>
#include <util.h>

#define	YES	1
#define	NO	0

#ifndef PRIdaddr
#define PRIdaddr PRId64
#endif

struct command {
	const char *cmd_name;
	const char *arg_names;
	void (*cmd_func)(int, char *[]);
	int open_flags;
};

static struct command *lookup(const char *);
__dead static void	usage(void);
static void	run(int, char *[]);
static void	showall(void);

static int	fd;				/* file descriptor for device */
static const	char *dvname;			/* device name */
static char	dvname_store[MAXPATHLEN];	/* for opendisk(3) */
static const	char *cmdname;			/* command user issued */

static void	disk_getcache(int, char *[]);
static void	disk_setcache(int, char *[]);
static void	disk_synccache(int, char *[]);
static void	disk_badsectors(int, char *[]);

static void	disk_strategy(int, char *[]);

static struct command commands[] = {
	{ "getcache",
	  "",
	  disk_getcache,
	  O_RDONLY },

	{ "setcache",
	  "none | r | w | rw [save]",
	  disk_setcache,
	  O_RDWR },

	{ "synccache",
	  "[force]",
	  disk_synccache,
	  O_RDWR },

	{ "badsector",
	  "flush | list | retry",
	   disk_badsectors,
	   O_RDWR },

	{ "strategy",
	  "[name]",
	  disk_strategy,
	  O_RDWR },

	{ NULL,
	  NULL,
	  NULL,
	  0 },
};

int
main(int argc, char *argv[])
{

	/* Must have at least: device command */
	if (argc < 2)
		usage();

	dvname = argv[1];
	if (argc == 2)
		showall();
	else {
		/* Skip program name, get and skip device name and command. */
		cmdname = argv[2];
		argv += 3;
		argc -= 3;
		run(argc, argv);
	}

	exit(0);
}

static void
run(int argc, char *argv[])
{
	struct command *command;

	command = lookup(cmdname);

	/* Open the device. */
	fd = opendisk(dvname, command->open_flags, dvname_store,
	    sizeof(dvname_store), 0);
	if (fd == -1)
		err(1, "%s", dvname);
	dvname = dvname_store;

	(*command->cmd_func)(argc, argv);

	/* Close the device. */
	(void)close(fd);
}

static struct command *
lookup(const char *name)
{
	int i;

	/* Look up the command. */
	for (i = 0; commands[i].cmd_name != NULL; i++)
		if (strcmp(name, commands[i].cmd_name) == 0)
			break;
	if (commands[i].cmd_name == NULL)
		errx(1, "unknown command: %s", name);

	return &commands[i];
}

static void
usage(void)
{
	int i;

	fprintf(stderr,
	    "usage: %s device\n"
	    "       %s device command [arg [...]]\n",
	    getprogname(), getprogname());

	fprintf(stderr, "   Available commands:\n");
	for (i = 0; commands[i].cmd_name != NULL; i++)
		fprintf(stderr, "\t%s %s\n", commands[i].cmd_name,
		    commands[i].arg_names);

	exit(1);
}

static void
showall(void)
{
	printf("strategy:\n");
	cmdname = "strategy";
	run(0, NULL);

	putchar('\n');

	printf("cache:\n");
	cmdname = "getcache";
	run(0, NULL);
}

static void
disk_strategy(int argc, char *argv[])
{
	struct disk_strategy odks;
	struct disk_strategy dks;

	memset(&dks, 0, sizeof(dks));
	if (ioctl(fd, DIOCGSTRATEGY, &odks) == -1) {
		err(EXIT_FAILURE, "%s: DIOCGSTRATEGY", dvname);
	}

	memset(&dks, 0, sizeof(dks));
	switch (argc) {
	case 0:
		/* show the buffer queue strategy used */
		printf("%s: %s\n", dvname, odks.dks_name);
		return;
	case 1:
		/* set the buffer queue strategy */
		strlcpy(dks.dks_name, argv[0], sizeof(dks.dks_name));
		if (ioctl(fd, DIOCSSTRATEGY, &dks) == -1) {
			err(EXIT_FAILURE, "%s: DIOCSSTRATEGY", dvname);
		}
		printf("%s: %s -> %s\n", dvname, odks.dks_name, argv[0]);
		break;
	default:
		usage();
		/* NOTREACHED */
	}
}

static void
disk_getcache(int argc, char *argv[])
{
	int bits;

	if (ioctl(fd, DIOCGCACHE, &bits) == -1)
		err(1, "%s: getcache", dvname);

	if ((bits & (DKCACHE_READ|DKCACHE_WRITE)) == 0)
		printf("%s: No caches enabled\n", dvname);
	else {
		if (bits & DKCACHE_READ)
			printf("%s: read cache enabled\n", dvname);
		if (bits & DKCACHE_WRITE)
			printf("%s: write-back cache enabled\n", dvname);
	}

	printf("%s: read cache enable is %schangeable\n", dvname,
	    (bits & DKCACHE_RCHANGE) ? "" : "not ");
	printf("%s: write cache enable is %schangeable\n", dvname,
	    (bits & DKCACHE_WCHANGE) ? "" : "not ");

	printf("%s: cache parameters are %ssavable\n", dvname,
	    (bits & DKCACHE_SAVE) ? "" : "not ");
}

static void
disk_setcache(int argc, char *argv[])
{
	int bits;

	if (argc > 2 || argc == 0)
		usage();

	if (strcmp(argv[0], "none") == 0)
		bits = 0;
	else if (strcmp(argv[0], "r") == 0)
		bits = DKCACHE_READ;
	else if (strcmp(argv[0], "w") == 0)
		bits = DKCACHE_WRITE;
	else if (strcmp(argv[0], "rw") == 0)
		bits = DKCACHE_READ|DKCACHE_WRITE;
	else
		usage();

	if (argc == 2) {
		if (strcmp(argv[1], "save") == 0)
			bits |= DKCACHE_SAVE;
		else
			usage();
	}

	if (ioctl(fd, DIOCSCACHE, &bits) == -1)
		err(1, "%s: setcache", dvname);
}

static void
disk_synccache(int argc, char *argv[])
{
	int force;

	switch (argc) {
	case 0:
		force = 0;
		break;

	case 1:
		if (strcmp(argv[0], "force") == 0)
			force = 1;
		else
			usage();
		break;

	default:
		usage();
	}

	if (ioctl(fd, DIOCCACHESYNC, &force) == -1)
		err(1, "%s: sync cache", dvname);
}

static void
disk_badsectors(int argc, char *argv[])
{
	struct disk_badsectors *dbs, *dbs2, buffer[200];
	SLIST_HEAD(, disk_badsectors) dbstop;
	struct disk_badsecinfo dbsi;
	daddr_t blk, totbad, bad;
	u_int32_t count;
	struct stat sb;
	u_char *block;
	time_t tm;

	if (argc != 1)
		usage();

	if (strcmp(argv[0], "list") == 0) {
		/*
		 * Copy the list of kernel bad sectors out in chunks that fit
		 * into buffer[].  Updating dbsi_skip means we don't sit here
		 * forever only getting the first chunk that fit in buffer[].
		 */
		dbsi.dbsi_buffer = (caddr_t)buffer;
		dbsi.dbsi_bufsize = sizeof(buffer);
		dbsi.dbsi_skip = 0;
		dbsi.dbsi_copied = 0;
		dbsi.dbsi_left = 0;

		do {
			if (ioctl(fd, DIOCBSLIST, (caddr_t)&dbsi) == -1)
				err(1, "%s: badsectors list", dvname);

			dbs = (struct disk_badsectors *)dbsi.dbsi_buffer;
			for (count = dbsi.dbsi_copied; count > 0; count--) {
				tm = dbs->dbs_failedat.tv_sec;
				printf("%s: blocks %" PRIdaddr " - %" PRIdaddr " failed at %s",
					dvname, dbs->dbs_min, dbs->dbs_max,
					ctime(&tm));
				dbs++;
			}
			dbsi.dbsi_skip += dbsi.dbsi_copied;
		} while (dbsi.dbsi_left != 0);

	} else if (strcmp(argv[0], "flush") == 0) {
		if (ioctl(fd, DIOCBSFLUSH) == -1)
			err(1, "%s: badsectors flush", dvname);

	} else if (strcmp(argv[0], "retry") == 0) {
		/*
		 * Enforce use of raw device here because the block device
		 * causes access to blocks to be clustered in a larger group,
		 * making it impossible to determine which individual sectors
		 * are the cause of a problem.
		 */ 
		if (fstat(fd, &sb) == -1)
			err(1, "fstat");

		if (!S_ISCHR(sb.st_mode)) {
			fprintf(stderr, "'badsector retry' must be used %s\n",
				"with character device");
			exit(1);
		}

		SLIST_INIT(&dbstop);

		/*
		 * Build up a copy of the in-kernel list in a number of stages.
		 * That the list we build up here is in the reverse order to
		 * the kernel's is of no concern.
		 */
		dbsi.dbsi_buffer = (caddr_t)buffer;
		dbsi.dbsi_bufsize = sizeof(buffer);
		dbsi.dbsi_skip = 0;
		dbsi.dbsi_copied = 0;
		dbsi.dbsi_left = 0;

		do {
			if (ioctl(fd, DIOCBSLIST, (caddr_t)&dbsi) == -1)
				err(1, "%s: badsectors list", dvname);

			dbs = (struct disk_badsectors *)dbsi.dbsi_buffer;
			for (count = dbsi.dbsi_copied; count > 0; count--) {
				dbs2 = malloc(sizeof *dbs2);
				if (dbs2 == NULL)
					err(1, NULL);
				*dbs2 = *dbs;
				SLIST_INSERT_HEAD(&dbstop, dbs2, dbs_next);
				dbs++;
			}
			dbsi.dbsi_skip += dbsi.dbsi_copied;
		} while (dbsi.dbsi_left != 0);

		/*
		 * Just calculate and print out something that will hopefully
		 * provide some useful information about what's going to take
		 * place next (if anything.)
		 */
		bad = 0;
		totbad = 0;
		if ((block = calloc(1, DEV_BSIZE)) == NULL)
			err(1, NULL);
		SLIST_FOREACH(dbs, &dbstop, dbs_next) {
			bad++;
			totbad += dbs->dbs_max - dbs->dbs_min + 1;
		}

		printf("%s: bad sector clusters %"PRIdaddr
		    " total sectors %"PRIdaddr"\n", dvname, bad, totbad);

		/*
		 * Clear out the kernel's list of bad sectors, ready for us
		 * to test all those it thought were bad.
		 */
		if (ioctl(fd, DIOCBSFLUSH) == -1)
			err(1, "%s: badsectors flush", dvname);

		printf("%s: bad sectors flushed\n", dvname);

		/*
		 * For each entry we obtained from the kernel, retry each
		 * individual sector recorded as bad by seeking to it and
		 * attempting to read it in.  Print out a line item for each
		 * bad block we verify.
		 *
		 * PRIdaddr is used here because the type of dbs_max is daddr_t
		 * and that may be either a 32bit or 64bit number(!)
		 */
		SLIST_FOREACH(dbs, &dbstop, dbs_next) {
			printf("%s: Retrying %"PRIdaddr" - %"
			    PRIdaddr"\n", dvname, dbs->dbs_min, dbs->dbs_max);

			for (blk = dbs->dbs_min; blk <= dbs->dbs_max; blk++) {
				if (lseek(fd, (off_t)blk * DEV_BSIZE,
				    SEEK_SET) == -1) {
					warn("%s: lseek block %" PRIdaddr "",
					    dvname, blk);
					continue;
				}
				printf("%s: block %"PRIdaddr" - ", dvname, blk);
				if (read(fd, block, DEV_BSIZE) != DEV_BSIZE)
					printf("failed\n");
				else
					printf("ok\n");
				fflush(stdout);
			}
		}
	}
}
