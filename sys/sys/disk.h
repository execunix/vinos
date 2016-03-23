/*	$NetBSD: disk.h,v 1.60 2014/04/03 15:24:20 christos Exp $	*/

/*-
 * Copyright (c) 1996, 1997, 2004 The NetBSD Foundation, Inc.
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
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This software was developed by the Computer Systems Engineering group
 * at Lawrence Berkeley Laboratory under DARPA contract BG 91-66 and
 * contributed to Berkeley.
 *
 * All advertising materials mentioning features or use of this software
 * must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Lawrence Berkeley Laboratories.
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
 * from: Header: disk.h,v 1.5 92/11/19 04:33:03 torek Exp  (LBL)
 *
 *	@(#)disk.h	8.2 (Berkeley) 1/9/95
 */

#ifndef _SYS_DISK_H_
#define _SYS_DISK_H_

/*
 * Disk device structures.
 */

#ifdef _KERNEL
#include <sys/device.h>
#endif
#include <sys/dkio.h>
#include <sys/time.h>
#include <sys/queue.h>
#include <sys/mutex.h>
#include <sys/iostat.h>

#include <prop/proplib.h>

struct buf;
struct disk;
struct disklabel;
struct cpu_disklabel;
struct lwp;
struct vnode;

/*
 * Disk information dictionary.
 *
 * This contains general infomation for disk devices.
 *
 *	<dict>
 *		<key>type</key>
 *		<string>...</string>
 *		<key>geometry</key>
 *		<dict>
 *			<!-- See below for disk geometry dictionary
 *			     contents. -->
 *		</dict>
 *
 *		<!-- optional information -->
 *		<key>rpm</key>
 *		<integer>...</integer>
 *		<key>sector-interleave</key>
 *		<integer>...</integer>
 *		<key>track-skew</key>
 *		<integer>...</integer>
 *		<key>cylinder-skew</key>
 *		<integer>...</integer>
 *		<key>head-switch-usecs</key>
 *		<integer>...</integer>
 *		<key>track-seek-usecs</key>
 *		<integer>...</integer>
 *		<key>removable</key>
 *		<false/>
 *		<key>ecc</key>
 *		<false/>
 *		<key>bad-sector-forwarding</key>
 *		<true/>
 *		<key>ramdisk</key>
 *		<false/>
 *		<key>back-to-back-transfers</key>
 *		<true/>
 *
 *		<!-- additional information for SMD drives -->
 *		<key>smd-skip-sectoring</key>
 *		<false/>
 *		<!-- XXX better names for these properties -->
 *		<key>smd-mindist</key>
 *		<integer>...</integer>
 *		<key>smd-maxdist</key>
 *		<integer>...</integer>
 *		<key>smd-sdist</key>
 *		<integer>...</integer>
 *
 *		<!-- additional information for ST506 drives -->
 *		<!-- XXX better names for these properties -->
 *		<key>st506-precompcyl</key>
 *		<integer>...</integer>
 *		<key>st506-gap3</key>
 *		<integer>...</integer>
 *
 *		<!-- additional information for ATA drives -->
 *		<!-- XXX -->
 *
 *		<!-- additional information for SCSI drives -->
 *		<!-- XXX -->
 *	</dict>
 */

/*
 * Disk geometry dictionary.
 *
 * NOTE: Not all geometry information is relevant for every kind of disk.
 *
 *	<dict>
 *		<key>sectors-per-unit</key>
 *		<integer>...</integer>
 *		<key>sector-size</key>
 *		<integer>...</integer>
 *		<key>sectors-per-track</key>
 *		<integer>...</integer>
 *		<key>tracks-per-cylinder</key>
 *		<integer>...</integer>
 *		<key>cylinders-per-unit</key>
 *		<integer>...</integer>
 *		<key>physical-cylinders-per-unit</key>
 *		<integer>...</integer>
 *		<key>spare-sectors-per-track</key>
 *		<integer>...</integer>
 *		<key>spare-sectors-per-cylinder</key>
 *		<integer>...</integer>
 *		<key>alternative-cylinders</key>
 *		<integer>...</integer>
 *	</dict>
 * NOTE: Not all geometry information is relevant for every kind of disk.
 */

struct disk_geom {
	int64_t		dg_secperunit;	/* # of data sectors per unit */
	uint32_t	dg_secsize;	/* # of bytes per sector */
	uint32_t	dg_nsectors;	/* # of data sectors per track */
	uint32_t	dg_ntracks;	/* # of tracks per cylinder */
	uint32_t	dg_ncylinders;	/* # of data cylinders per unit */
	uint32_t	dg_secpercyl;	/* # of data sectors per cylinder */
	uint32_t	dg_pcylinders;	/* # of physical cylinders per unit */

	/*
	 * Spares (bad sector replacements) below are not counted in
	 * dg_nsectors or dg_secpercyl.  Spare sectors are assumed to
	 * be physical sectors which occupy space at the end of each
	 * track and/or cylinder.
	 */
	uint32_t	dg_sparespertrack;
	uint32_t	dg_sparespercyl;
	/*
	 * Alternative cylinders include maintenance, replacement,
	 * configuration description areas, etc.
	 */
	uint32_t	dg_acylinders;
};

/*
 * Disk partition dictionary.
 *
 * A partition is represented as a dictionary containing generic partition
 * properties (such as starting block and block count), as well as information
 * that is specific to individual partition map formats.
 *
 *	<dict>
 *		<key>start-block</key>
 *		<integer>...</integer>
 *		<key>block-count</key>
 *		<integer>...</integer>
 *		<!-- DKW_PTYPE strings ("" or missing if unknown) -->
 *		<key>type</type>
 *		<string>...</string>
 *		<!-- optional -->
 *		<key>name</key>
 *		<string>...</string>
 *
 *		<!-- these are valid for GPT partition maps -->
 *		<key>gpt-type-guid</key>
 *		<string>...</string>
 *		<key>gpt-partition-guid</key>
 *		<string>...</string>
 *		<key>gpt-platform-required</key>
 *		<false/>
 *
 *		<!-- these are valid for 4.4BSD partition maps -->
 *		<key>bsd44-partition-type</key>
 *		<integer>...</integer>
 *		<key>bsd44-fs-fragment-size</key>
 *		<integer>...</integer>
 *		<key>bsd44-iso9660-session-offset</key>
 *		<integer>...</integer>
 *		<key>bsd44-ffs-cylinders-per-group</key>
 *		<integer>...</integer>
 *
 *		<!-- these are valid for NeXT partition maps -->
 *		<key>next-block-size</key>
 *		<integer>...</integer>
 *		<key>next-fs-fragment-size</key>
 *		<integer>...</integer>
 *		<key>next-fs-optimization</key>
 *		<string>...</string>	<!-- "space" or "time" -->
 *		<key>next-fs-cylinders-per-group</key>
 *		<integer>...</integer>
 *		<key>next-bytes-per-inode-density</key>
 *		<integer>...</integer>
 *		<key>next-minfree-percentage</key>
 *		<integer>...</integer>
 *		<key>next-run-newfs-during-init</key>
 *		<false/>
 *		<key>next-mount-point</key>
 *		<string>...</string>
 *		<key>next-automount</key>
 *		<true/>
 *		<key>next-partition-type</key>
 *		<string>...</string>
 *
 *		<!-- these are valid for MBR partition maps -->
 *		<key>mbr-start-head</key>
 *		<integer>...</integer>
 *		<key>mbr-start-sector</key>
 *		<integer>...</integer>
 *		<key>mbr-start-cylinder</key>
 *		<integer>...</integer>
 *		<key>mbr-partition-type</key>
 *		<integer>...</integer>
 *		<key>mbr-end-head</key>
 *		<integer>...</integer>
 *		<key>mbr-end-sector</key>
 *		<integer>...</integer>
 *		<key>mbr-end-cylinder</key>
 *		<integer>...</integer>
 *		<key>mbr-active-partition</key>
 *		<false/>
 *
 *		<!-- these are valid for Apple partition maps -->
 *		<key>apple-partition-type</key>
 *		<string>...</string>
 *		<!-- XXX What else do we need?  wrstuden? -->
 *
 *		<!-- these are valid for RISCiX partition maps -->
 *		<key>riscix-partition-type</key>
 *		<integer>...</integer>
 *
 *		<!-- these are valid for MIPS/SGI partition maps -->
 *		<key>mips-partition-type</key>
 *		<integer>...</integer>
 *
 *		<!-- SunOS 4 partition maps have no specific
 *		     additional information.  Note, however,
 *		     that SunOS 4 partitions must begin on
 *		     cylinder boundaries. -->
 *
 *		<!-- XXX Need Amiga partition map info -->
 *
 *		<!-- these are valid for VTOC partition maps -->
 *		<key>vtoc-tag</key>
 *		<integer>...</integer>
 *		<key>vtoc-unmount</key>
 *		<false/>
 *		<key>vtoc-read-only</key>
 *		<false/>
 *		<!-- XXX is this really part of the partition info? -->
 *		<key>vtoc-timestamp</key>
 *		<integer>...</integer>
 *
 *		<!-- mvme68k partition maps use 4.4BSD partition
 *		     info stuffed into two different areas of the
 *		     disk information label recognized by BUG. -->
 *
 *		<!-- XXX What else? -->
 *	</dict>
 */

struct disk {
	TAILQ_ENTRY(disk) dk_link;	/* link in global disklist */
	const char	*dk_name;	/* disk name */
	prop_dictionary_t dk_info;	/* reference to disk-info dictionary */
	struct disk_geom dk_geom;	/* cooked version of dk_info */
	int		dk_bopenmask;	/* block devices open */
	int		dk_copenmask;	/* character devices open */
	int		dk_openmask;	/* composite (bopen|copen) */
	int		dk_state;	/* label state   ### */
	int		dk_blkshift;	/* shift to convert DEV_BSIZE to blks */
	int		dk_byteshift;	/* shift to convert bytes to blks */

	/*
	 * Metrics data; note that some metrics may have no meaning
	 * on certain types of disks.
	 */
	struct io_stats	*dk_stats;

	const struct dkdriver *dk_driver;	/* pointer to driver */

	kmutex_t	dk_openlock;	/* lock on these and openmask */

	/*
	 * Disk label information.  Storage for the in-core disk label
	 * must be dynamically allocated, otherwise the size of this
	 * structure becomes machine-dependent.
	 */
	daddr_t		dk_labelsector;		/* sector containing label */
	struct disklabel *dk_label;	/* label */
	struct cpu_disklabel *dk_cpulabel;
};

struct dkdriver {
	void	(*d_strategy)(struct buf *);
	void	(*d_minphys)(struct buf *);
#ifdef notyet
	int	(*d_open)(dev_t, int, int, struct proc *);
	int	(*d_close)(dev_t, int, int, struct proc *);
	int	(*d_ioctl)(dev_t, u_long, void *, int, struct proc *);
	int	(*d_dump)(dev_t);
	void	(*d_start)(struct buf *, daddr_t);
	int	(*d_mklabel)(struct disk *);
#endif
};

/* states */
#define	DK_CLOSED	0		/* drive is closed */
#define	DK_WANTOPEN	1		/* drive being opened */
#define	DK_WANTOPENRAW	2		/* drive being opened */
#define	DK_RDLABEL	3		/* label being read */
#define	DK_OPEN		4		/* label read, drive open */
#define	DK_OPENRAW	5		/* open without label */

/*
 * Bad sector lists per fixed disk
 */
struct disk_badsectors {
	SLIST_ENTRY(disk_badsectors)	dbs_next;
	daddr_t		dbs_min;	/* min. sector number */
	daddr_t		dbs_max;	/* max. sector number */
	struct timeval	dbs_failedat;	/* first failure at */
};

struct disk_badsecinfo {
	uint32_t	dbsi_bufsize;	/* size of region pointed to */
	uint32_t	dbsi_skip;	/* how many to skip past */
	uint32_t	dbsi_copied;	/* how many got copied back */
	uint32_t	dbsi_left;	/* remaining to copy */
	void *		dbsi_buffer;	/* region to copy disk_badsectors to */
};

#define	DK_STRATEGYNAMELEN	32
struct disk_strategy {
	char dks_name[DK_STRATEGYNAMELEN]; /* name of strategy */
	char *dks_param;		/* notyet; should be NULL */
	size_t dks_paramlen;		/* notyet; should be 0 */
};

#define	DK_BSIZE2BLKSHIFT(b)	((ffs((b) / DEV_BSIZE)) - 1)
#define	DK_BSIZE2BYTESHIFT(b)	(ffs((b)) - 1)

#ifdef _KERNEL
extern	int disk_count;			/* number of disks in global disklist */

struct proc;

void	disk_attach(struct disk *);
int	disk_begindetach(struct disk *, int (*)(device_t), device_t, int);
void	disk_detach(struct disk *);
void	disk_init(struct disk *, const char *, const struct dkdriver *);
void	disk_destroy(struct disk *);
void	disk_busy(struct disk *);
void	disk_unbusy(struct disk *, long, int);
bool	disk_isbusy(struct disk *);
void	disk_blocksize(struct disk *, int);
struct disk *disk_find(const char *);
int	disk_ioctl(struct disk *, u_long, void *, int, struct lwp *);
void	disk_set_info(device_t, struct disk *, const char *);
#endif

#endif /* _SYS_DISK_H_ */
