/*	$NetBSD: subr_disk_mbr.c,v 1.46 2013/06/26 18:47:26 matt Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1988 Regents of the University of California.
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
 *	@(#)ufs_disksubr.c	7.16 (Berkeley) 5/4/91
 */

/*
 * Code to find a NetBSD label on a disk that contains an i386 style MBR.
 * The first NetBSD label found in the 2nd sector of a NetBSD partition
 * is used.
 * If we don't find a label searching the MBR, we look at the start of the
 * disk, if that fails then a label is faked up from the MBR.
 *
 * If there isn't a disklabel or anything in the MBR then the disc is searched
 * for ecma-167/iso9660/udf style partition indicators.
 * Useful for media or files that contain single filesystems (etc).
 *
 * This code will read host endian netbsd labels from little endian MBR.
 *
 * Based on the i386 disksubr.c
 *
 * Since the mbr only has 32bit fields for sector addresses, we do the same.
 *
 * XXX There are potential problems writing labels to disks where there
 * is only space for 8 netbsd partitions but this code has been compiled
 * with MAXPARTITIONS=16.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: subr_disk_mbr.c,v 1.46 2013/06/26 18:47:26 matt Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/bootblock.h>
#include <sys/diskinfo.h>
#include <sys/disk.h>
#include <sys/syslog.h>
#include <sys/vnode.h>
#include <sys/fcntl.h>
#include <sys/conf.h>
#include <sys/cdio.h>
#include <fs/udf/ecma167-udf.h>

#include <sys/kauth.h>

typedef struct mbr_partition mbr_partition_t;

#define SCAN_CONTINUE	0
#define SCAN_FOUND	1
#define SCAN_ERROR	2

typedef struct mbr_args {
	struct disklabel *lp;
	void		(*strat)(struct buf *);
	struct buf	*bp;
	const char	*msg;
	int		error;
	int		found_mbr;	/* set if disk has a valid mbr */
	uint32_t	secperunit;
} mbr_args_t;

static int
read_sector(mbr_args_t *a, uint sector, int count)
{
	int error;

	error = disk_read_sectors(a->strat, a->lp, a->bp, sector, count);
	if (error != 0)
		a->error = error;
	return error;
}

static int
look_mbr_part(mbr_args_t *a, mbr_partition_t *dp, int slot, uint ext_base)
{
	int n = a->lp->d_npartitions++;

	a->lp->d_partitions[n].p_size = dp->mbrp_size;
	a->lp->d_partitions[n].p_offset = dp->mbrp_start;
	a->lp->d_partitions[n].p_fstype = dp->mbrp_type;

	if (a->lp->d_npartitions < MAXPARTITIONS)
		return SCAN_CONTINUE;
	return SCAN_FOUND;
}

/*
 * Scan MBR for partitions
 */
static int
scan_mbr(mbr_args_t *a)
{
	mbr_partition_t ptns[MBR_PART_COUNT];
	mbr_partition_t *dp;
	struct mbr_sector *mbr;
	uint ext_base, this_ext, next_ext;
	int rval;
	int i;
	int j;

	ext_base = 0;
	this_ext = 0;
	for (;;) {
		if (read_sector(a, this_ext, 1)) {
			a->msg = "dos partition I/O error";
			return SCAN_ERROR;
		}

		/* Note: Magic number is little-endian. */
		mbr = (void *)a->bp->b_data;
		if (mbr->mbr_magic != htole16(MBR_MAGIC))
			return SCAN_CONTINUE;

		/*
		 * If this is a protective MBR, bail now.
		 */
		if (mbr->mbr_parts[0].mbrp_type == MBR_PTYPE_PMBR &&
		    mbr->mbr_parts[1].mbrp_type == MBR_PTYPE_UNUSED &&
		    mbr->mbr_parts[2].mbrp_type == MBR_PTYPE_UNUSED &&
		    mbr->mbr_parts[3].mbrp_type == MBR_PTYPE_UNUSED)
			return SCAN_CONTINUE;

		/* Copy data out of buffer */
		memcpy(ptns, &mbr->mbr_parts, sizeof ptns);

		/* Look for drivers and skip them */
		if (ext_base == 0 &&
		    ptns[0].mbrp_type == MBR_PTYPE_DM6_DDO &&
		    ptns[1].mbrp_type == MBR_PTYPE_UNUSED &&
		    ptns[2].mbrp_type == MBR_PTYPE_UNUSED &&
		    ptns[3].mbrp_type == MBR_PTYPE_UNUSED) {
			/* We've found a DM6 DDO partition type (used by
			 * the Ontrack Disk Manager drivers).
			 *
			 * Ensure that there are no other partitions in the
			 * MBR and jump to the real partition table (stored
			 * in the first sector of the second track). */
			this_ext = le32toh(a->lp->d_secpercyl / a->lp->d_ntracks);
			continue;
		}

		/* look for NetBSD partition */
		next_ext = 0;
		dp = ptns;
		j = 0;
		for (i = 0; i < MBR_PART_COUNT; i++, dp++) {
			if (dp->mbrp_type == MBR_PTYPE_UNUSED)
				continue;
			/* Check end of partition is inside disk limits */
			if ((uint64_t)ext_base + le32toh(dp->mbrp_start) +
			    le32toh(dp->mbrp_size) > a->lp->d_secperunit) {
				/* This mbr doesn't look good.... */
				a->msg = "mbr partition exceeds disk size";
				/* ...but don't report this as an error (yet) */
				return SCAN_CONTINUE;
			}
			a->found_mbr = 1;
			if (MBR_IS_EXTENDED(dp->mbrp_type)) {
				next_ext = le32toh(dp->mbrp_start);
				continue;
			}
			rval = look_mbr_part(a, dp, j, this_ext);
			if (rval != SCAN_CONTINUE)
				return rval;
			j++;
		}
		if (next_ext == 0)
			break;
		if (ext_base == 0) {
			ext_base = next_ext;
			next_ext = 0;
		}
		next_ext += ext_base;
		if (next_ext <= this_ext)
			break;
		this_ext = next_ext;
	}
	if (a->found_mbr)
		return SCAN_FOUND;
	return SCAN_CONTINUE;
}


static void
scan_iso_vrs_session(mbr_args_t *a, uint32_t first_sector,
	int *is_iso9660, int *is_udf)
{
	struct vrs_desc *vrsd;
	uint64_t vrs;
	int sector_size;
	int blks, inc;

	sector_size = a->lp->d_secsize;
	blks = sector_size / DEV_BSIZE;
	inc  = MAX(1, 2048 / sector_size);

	/* by definition */
	vrs = ((32*1024 + sector_size - 1) / sector_size)
	        + first_sector;

	/* read first vrs sector */
	if (read_sector(a, vrs * blks, 1))
		return;

	/* skip all CD001 records */
	vrsd = a->bp->b_data;
	/* printf("vrsd->identifier = `%s`\n", vrsd->identifier); */
	while (memcmp(vrsd->identifier, "CD001", 5) == 0) {
		/* for sure */
		*is_iso9660 = first_sector;

		vrs += inc;
		if (read_sector(a, vrs * blks, 1))
			return;
	}

	/* search for BEA01 */
	vrsd = a->bp->b_data;
	/* printf("vrsd->identifier = `%s`\n", vrsd->identifier); */
	if (memcmp(vrsd->identifier, "BEA01", 5))
		return;

	/* read successor */
	vrs += inc;
	if (read_sector(a, vrs * blks, 1))
		return;

	/* check for NSR[23] */
	vrsd = a->bp->b_data;
	/* printf("vrsd->identifier = `%s`\n", vrsd->identifier); */
	if (memcmp(vrsd->identifier, "NSR0", 4))
		return;

	*is_udf = first_sector;
}


/*
 * Scan for ISO Volume Recognition Sequences
 */
static int
scan_iso_vrs(mbr_args_t *a)
{
	struct mmc_discinfo  di;
	struct mmc_trackinfo ti;
	dev_t dev;
	uint64_t sector;
	int is_iso9660, is_udf;
	int tracknr, sessionnr;
	int new_session, error;

	is_iso9660 = is_udf = -1;

	/* parse all sessions of disc if we're on a SCSI MMC device */
	if (a->lp->d_flags & D_SCSI_MMC) {
		/* get disc info */
		dev = a->bp->b_dev;
		error = bdev_ioctl(dev, MMCGETDISCINFO, &di, FKIOCTL, curlwp);
		if (error)
			return SCAN_CONTINUE;

		/* go trough all (data) tracks */
		sessionnr = -1;
		for (tracknr = di.first_track;
		    tracknr <= di.first_track_last_session; tracknr++)
		{
			ti.tracknr = tracknr;
			error = bdev_ioctl(dev, MMCGETTRACKINFO, &ti,
					FKIOCTL, curlwp);
			if (error)
				return SCAN_CONTINUE;
			new_session = (ti.sessionnr != sessionnr);
			sessionnr = ti.sessionnr;
			if (new_session) {
				if (ti.flags & MMC_TRACKINFO_BLANK)
					continue;
				if (!(ti.flags & MMC_TRACKINFO_DATA))
					continue;
				sector = ti.track_start;
				scan_iso_vrs_session(a, sector,
					&is_iso9660, &is_udf);
			}
		}
	} else {
		/* try start of disc */
		sector = 0;
		scan_iso_vrs_session(a, sector, &is_iso9660, &is_udf);
	}

	if ((is_iso9660 < 0) && (is_udf < 0))
		return SCAN_CONTINUE;

	strncpy(a->lp->d_typename, "iso partition", 16);

	/* adjust session information for iso9660 partition */
	if (is_iso9660 >= 0) {
		/* set 'a' partition to iso9660 */
		a->lp->d_partitions[0].p_offset = 0;
		a->lp->d_partitions[0].p_size   = a->lp->d_secperunit;
		a->lp->d_partitions[0].p_cdsession = is_iso9660;
		a->lp->d_partitions[0].p_fstype = FS_ISO9660;
	}

	/* UDF doesn't care about the cd session specified here */

	return SCAN_FOUND;
}


/*
 * Attempt to read a disk label from a device
 * using the indicated strategy routine.
 * The label must be partly set up before this:
 * secpercyl, secsize and anything required for a block i/o read
 * operation in the driver's strategy/start routines
 * must be filled in before calling us.
 *
 * If dos partition table requested, attempt to load it and
 * find disklabel inside a DOS partition. Also, if bad block
 * table needed, attempt to extract it as well. Return buffer
 * for use in signalling errors if requested.
 *
 * Returns null on success and an error string on failure.
 */
const char *
readdisklabel(dev_t dev, void (*strat)(struct buf *), struct disklabel *lp)
{
	int rval;
	int i;
	mbr_args_t a;

	memset(&a, 0, sizeof a);
	a.lp = lp;
	a.strat = strat;

	/* minimal requirements for architypal disk label */
	if (lp->d_secsize == 0)
		lp->d_secsize = DEV_BSIZE;
	if (lp->d_secperunit == 0)
		lp->d_secperunit = 0x1fffffff;
	a.secperunit = lp->d_secperunit;

	/*
	 * Set partition 'a' to be the whole disk.
	 */
	lp->d_partitions[0].p_size = lp->d_secperunit;
	lp->d_partitions[0].p_fstype = FS_UNUSED/*FS_BSDFFS*/;
	lp->d_npartitions = 1;
	for (i = 1; i < MAXPARTITIONS; i++) {
		lp->d_partitions[i].p_size = 0;
		lp->d_partitions[i].p_offset = 0;
	}

	/*
	 * Get a buffer to read a disklabel in and initialize it
	 */
	a.bp = geteblk((int)lp->d_secsize);
	a.bp->b_dev = dev;

	/*
	 * Scan mbr searching for netbsd partition and saving
	 * bios partition information to use if the netbsd one
	 * is absent.
	 */
	rval = scan_mbr(&a);

	if (rval == SCAN_CONTINUE) {
		rval = scan_iso_vrs(&a);
	}

	brelse(a.bp, 0);
	if (rval == SCAN_ERROR || rval == SCAN_CONTINUE)
		return a.msg;
	return NULL;
}
