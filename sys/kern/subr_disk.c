/*	$NetBSD: subr_disk.c,v 1.103.4.2 2015/06/01 19:19:44 snj Exp $	*/

/*-
 * Copyright (c) 1996, 1997, 1999, 2000, 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
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
 * Copyright (c) 1982, 1986, 1988, 1993
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
 *
 *	@(#)ufs_disksubr.c	8.5 (Berkeley) 1/21/94
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: subr_disk.c,v 1.103.4.2 2015/06/01 19:19:44 snj Exp $");

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/kmem.h>
#include <sys/buf.h>
#include <sys/syslog.h>
#include <sys/diskinfo.h>
#include <sys/disk.h>
#include <sys/sysctl.h>
#include <lib/libkern/libkern.h>

/*
 * Compute checksum for disk label.
 */
u_int
dkcksum(struct disklabel *lp)
{

	return dkcksum_sized(lp, lp->d_npartitions);
}

u_int
dkcksum_sized(struct disklabel *lp, size_t npartitions)
{
	uint16_t *start, *end;
	uint16_t sum = 0;

	start = (uint16_t *)lp;
	end = (uint16_t *)&lp->d_partitions[npartitions];
	while (start < end)
		sum ^= *start++;
	return sum;
}

/*
 * Disk error is the preface to plaintive error messages
 * about failing disk transfers.  It prints messages of the form

hp0g: hard error reading fsbn 12345 of 12344-12347 (hp0 bn %d cn %d tn %d sn %d)

 * if the offset of the error in the transfer and a disk label
 * are both available.  blkdone should be -1 if the position of the error
 * is unknown; the disklabel pointer may be null from drivers that have not
 * been converted to use them.  The message is printed with printf
 * if pri is LOG_PRINTF, otherwise it uses log at the specified priority.
 * The message should be completed (with at least a newline) with printf
 * or addlog, respectively.  There is no trailing space.
 */
#ifndef PRIdaddr
#define PRIdaddr PRId64
#endif
void
diskerr(const struct buf *bp, const char *dname, const char *what, int pri,
    int blkdone, const struct disklabel *lp)
{
	int unit = DISKUNIT(bp->b_dev), part = DISKPART(bp->b_dev);
	void (*pr)(const char *, ...) __printflike(1, 2);
	char partname = 'a' + part;
	daddr_t sn;

	if (/*CONSTCOND*/0)
		/* Compiler will error this is the format is wrong... */
		printf("%" PRIdaddr, bp->b_blkno);

	if (pri != LOG_PRINTF) {
		static const char fmt[] = "";
		log(pri, fmt);
		pr = addlog;
	} else
		pr = printf;
	(*pr)("%s%d%c: %s %sing fsbn ", dname, unit, partname, what,
	    bp->b_flags & B_READ ? "read" : "writ");
	sn = bp->b_blkno;
	if (bp->b_bcount <= DEV_BSIZE)
		(*pr)("%" PRIdaddr, sn);
	else {
		if (blkdone >= 0) {
			sn += blkdone;
			(*pr)("%" PRIdaddr " of ", sn);
		}
		(*pr)("%" PRIdaddr "-%" PRIdaddr "", bp->b_blkno,
		    bp->b_blkno + (bp->b_bcount - 1) / DEV_BSIZE);
	}
	if (lp && (blkdone >= 0 || bp->b_bcount <= lp->d_secsize)) {
		sn += lp->d_partitions[part].p_offset;
		(*pr)(" (%s%d bn %" PRIdaddr "; cn %" PRIdaddr "",
		    dname, unit, sn, sn / lp->d_secpercyl);
		sn %= lp->d_secpercyl;
		(*pr)(" tn %" PRIdaddr " sn %" PRIdaddr ")",
		    sn / lp->d_nsectors, sn % lp->d_nsectors);
	}
}

/*
 * Searches the iostatlist for the disk corresponding to the
 * name provided.
 */
struct disk *
disk_find(const char *name)
{
	struct io_stats *stat;

	stat = iostat_find(name);

	if ((stat != NULL) && (stat->io_type == IOSTAT_DISK))
		return stat->io_parent;

	return (NULL);
}

void
disk_init(struct disk *diskp, const char *name, const struct dkdriver *driver)
{

	mutex_init(&diskp->dk_openlock, MUTEX_DEFAULT, IPL_NONE);
	disk_blocksize(diskp, DEV_BSIZE);
	diskp->dk_name = name;
	diskp->dk_driver = driver;
}

/*
 * Attach a disk.
 */
void
disk_attach(struct disk *diskp)
{

	/*
	 * Allocate and initialize the disklabel structures.
	 */
	diskp->dk_label = kmem_zalloc(sizeof(struct disklabel), KM_SLEEP);
	if (diskp->dk_label == NULL)
		panic("disk_attach: can't allocate storage for disklabel");

	/*
	 * Set up the stats collection.
	 */
	diskp->dk_stats = iostat_alloc(IOSTAT_DISK, diskp, diskp->dk_name);
}

int
disk_begindetach(struct disk *dk, int (*lastclose)(device_t),
    device_t self, int flags)
{
	int rc;

	rc = 0;
	mutex_enter(&dk->dk_openlock);
	if (dk->dk_openmask == 0)
		;	/* nothing to do */
	else if ((flags & DETACH_FORCE) == 0)
		rc = EBUSY;
	else if (lastclose != NULL)
		rc = (*lastclose)(self);
	mutex_exit(&dk->dk_openlock);

	return rc;
}

/*
 * Detach a disk.
 */
void
disk_detach(struct disk *diskp)
{

	/*
	 * Remove from the drivelist.
	 */
	iostat_free(diskp->dk_stats);

	/*
	 * Free the space used by the disklabel structures.
	 */
	kmem_free(diskp->dk_label, sizeof(*diskp->dk_label));
}

void
disk_destroy(struct disk *diskp)
{

	mutex_destroy(&diskp->dk_openlock);
}

/*
 * Mark the disk as busy for metrics collection.
 */
void
disk_busy(struct disk *diskp)
{

	iostat_busy(diskp->dk_stats);
}

/*
 * Finished disk operations, gather metrics.
 */
void
disk_unbusy(struct disk *diskp, long bcount, int read)
{

	iostat_unbusy(diskp->dk_stats, bcount, read);
}

/*
 * Return true if disk has an I/O operation in flight.
 */
bool
disk_isbusy(struct disk *diskp)
{

	return iostat_isbusy(diskp->dk_stats);
}

/*
 * Set the physical blocksize of a disk, in bytes.
 * Only necessary if blocksize != DEV_BSIZE.
 */
void
disk_blocksize(struct disk *diskp, int blocksize)
{

	diskp->dk_blkshift = DK_BSIZE2BLKSHIFT(blocksize);
	diskp->dk_byteshift = DK_BSIZE2BYTESHIFT(blocksize);
}

/*
 * Bounds checking against the media size, used for the raw partition.
 * secsize, mediasize and b_blkno must all be the same units.
 * Possibly this has to be DEV_BSIZE (512).
 */
int
bounds_check_with_mediasize(struct buf *bp, int secsize, uint64_t mediasize)
{
	int64_t sz;

	if (bp->b_blkno < 0) {
		/* Reject negative offsets immediately. */
		bp->b_error = EINVAL;
		return 0;
	}

	sz = howmany((int64_t)bp->b_bcount, secsize);

	/*
	 * bp->b_bcount is a 32-bit value, and we rejected a negative
	 * bp->b_blkno already, so "bp->b_blkno + sz" cannot overflow.
	 */

	if (bp->b_blkno + sz > mediasize) {
		sz = mediasize - bp->b_blkno;
		if (sz == 0) {
			/* If exactly at end of disk, return EOF. */
			bp->b_resid = bp->b_bcount;
			return 0;
		}
		if (sz < 0) {
			/* If past end of disk, return EINVAL. */
			bp->b_error = EINVAL;
			return 0;
		}
		/* Otherwise, truncate request. */
		bp->b_bcount = sz * secsize;
	}

	return 1;
}

/*
 * Determine the size of the transfer, and make sure it is
 * within the boundaries of the partition. Adjust transfer
 * if needed, and signal errors or early completion.
 */
int
bounds_check_with_label(struct disk *dk, struct buf *bp)
{
	struct disklabel *lp = dk->dk_label;
	struct partition *p = lp->d_partitions + DISKPART(bp->b_dev);
	uint64_t p_size;
	int64_t sz;

	if (bp->b_blkno < 0) {
		/* Reject negative offsets immediately. */
		bp->b_error = EINVAL;
		return -1;
	}

	/* Protect against division by zero. XXX: Should never happen?!?! */
	if (lp->d_secpercyl == 0) {
		bp->b_error = EINVAL;
		return -1;
	}

	p_size = (uint64_t)p->p_size << dk->dk_blkshift;

	sz = howmany((int64_t)bp->b_bcount, DEV_BSIZE);

	/*
	 * bp->b_bcount is a 32-bit value, and we rejected a negative
	 * bp->b_blkno already, so "bp->b_blkno + sz" cannot overflow.
	 */

	if (bp->b_blkno + sz > p_size) {
		sz = p_size - bp->b_blkno;
		if (sz == 0) {
			/* If exactly at end of disk, return EOF. */
			bp->b_resid = bp->b_bcount;
			return 0;
		}
		if (sz < 0) {
			/* If past end of disk, return EINVAL. */
			bp->b_error = EINVAL;
			return -1;
		}
		/* Otherwise, truncate request. */
		bp->b_bcount = sz << DEV_BSHIFT;
	}

	/* calculate cylinder for disksort to order transfers with */
	bp->b_cylinder = (bp->b_blkno + p->p_offset) /
	    (lp->d_secsize / DEV_BSIZE) / lp->d_secpercyl;
	return 1;
}

int
disk_read_sectors(void (*strat)(struct buf *), const struct disklabel *lp,
    struct buf *bp, unsigned int sector, int count)
{
	bp->b_blkno = sector;
	bp->b_bcount = count * lp->d_secsize;
	bp->b_flags = (bp->b_flags & ~B_WRITE) | B_READ;
	bp->b_oflags &= ~BO_DONE;
	bp->b_cylinder = sector / lp->d_secpercyl;
	(*strat)(bp);
	return biowait(bp);
}

/*
 * disk_ioctl --
 *	Generic disk ioctl handling.
 */
int
disk_ioctl(struct disk *diskp, u_long cmd, void *data, int flag,
	   struct lwp *l)
{
	int error = 0;

	switch (cmd) {
	case DIOCGDISKGEOM:
	    {
		struct disk_geom *dg = (struct disk_geom *)data;

		//error = ENOTSUP;
		memcpy(dg, &diskp->dk_geom, sizeof diskp->dk_geom);
		break;
	    }

	case DIOCGSECTORSIZE:
		*(u_int *)data = diskp->dk_geom.dg_secsize;
		break;

	case DIOCGMEDIASIZE:
		*(off_t *)data = (off_t)diskp->dk_geom.dg_secsize *
		    diskp->dk_geom.dg_secperunit;
		break;

	default:
		error = EPASSTHROUGH;
	}

	return (error);
}

void
disk_set_info(device_t dev, struct disk *dk, const char *type)
{
	struct disk_geom *dg = &dk->dk_geom;

	if (dg->dg_secperunit == 0 && dg->dg_ncylinders == 0) {
#ifdef DIAGNOSTIC
		printf("%s: secperunit and ncylinders are zero\n", dk->dk_name);
#endif
		return;
	}

	if (dg->dg_secperunit == 0) {
		if (dg->dg_nsectors == 0 || dg->dg_ntracks == 0) {
#ifdef DIAGNOSTIC
			printf("%s: secperunit and (sectors or tracks) "
			    "are zero\n", dk->dk_name);
#endif
			return;
		}
		dg->dg_secperunit = (int64_t) dg->dg_nsectors *
		    dg->dg_ntracks * dg->dg_ncylinders;
	}

	if (dg->dg_ncylinders == 0) {
		if (dg->dg_ntracks && dg->dg_nsectors)
			dg->dg_ncylinders = dg->dg_secperunit /
			    (dg->dg_ntracks * dg->dg_nsectors);
	}

	if (dg->dg_secsize == 0) {
#ifdef DIAGNOSTIC
		printf("%s: fixing 0 sector size\n", dk->dk_name);
#endif
		dg->dg_secsize = DEV_BSIZE;
	}
}
