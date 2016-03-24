/*	$NetBSD: disklabel.h,v 1.116 2013/11/05 00:36:02 msaitoh Exp $	*/

/*
 * Copyright (c) 1987, 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
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
 *	@(#)disklabel.h	8.2 (Berkeley) 7/10/94
 */

#ifndef _SYS_DISKLABEL_H_
#define	_SYS_DISKLABEL_H_

#ifndef _LOCORE
#include <sys/types.h>
#endif

/*
 * Each disk has a label which includes information about the hardware
 * disk geometry, filesystem partitions, and drive specific information.
 * The location of the label, as well as the number of partitions the
 * label can describe and the number of the "whole disk" (raw)
 * partition are machine dependent.
 */
#if HAVE_NBTOOL_CONFIG_H
#undef MAXPARTITIONS
#define MAXPARTITIONS		MAXMAXPARTITIONS
#else
#include <machine/diskinfo.h>
#endif /* HAVE_NBTOOL_CONFIG_H */

/*
 * The absolute maximum number of disk partitions allowed.
 * This is the maximum value of MAXPARTITIONS for which 'struct disklabel'
 * is <= DEV_BSIZE bytes long.  If MAXPARTITIONS is greater than this, beware.
 */
#define	MAXMAXPARTITIONS	22
#if MAXPARTITIONS > MAXMAXPARTITIONS
#warning beware: MAXPARTITIONS bigger than MAXMAXPARTITIONS
#endif

/*
 * Translate between device numbers and major/disk unit/disk partition.
 */
#if !HAVE_NBTOOL_CONFIG_H
#define	DISKUNIT(dev)	(minor(dev) / MAXPARTITIONS)
#define	DISKPART(dev)	(minor(dev) % MAXPARTITIONS)
#define	DISKMINOR(unit, part) \
    (((unit) * MAXPARTITIONS) + (part))
#endif /* !HAVE_NBTOOL_CONFIG_H */
#define	MAKEDISKDEV(maj, unit, part) \
    (makedev((maj), DISKMINOR((unit), (part))))

#define	DISKMAGIC	((uint32_t)0x82564557)	/* The disk magic number */

#ifndef _LOCORE
struct disklabel {
	uint32_t d_magic;		/* the magic number */
	uint16_t d_type;		/* drive type */
	uint16_t d_subtype;		/* controller/d_type specific */
	char	  d_typename[16];	/* type name, e.g. "eagle" */

	/*
	 * d_packname contains the pack identifier and is returned when
	 * the disklabel is read off the disk or in-core copy.
	 * d_boot0 and d_boot1 are the (optional) names of the
	 * primary (block 0) and secondary (block 1-15) bootstraps
	 * as found in /usr/mdec.  These are returned when using
	 * getdiskbyname(3) to retrieve the values from /etc/disktab.
	 */
	union {
		char	un_d_packname[16];	/* pack identifier */
		struct {
			char *un_d_boot0;	/* primary bootstrap name */
			char *un_d_boot1;	/* secondary bootstrap name */
		} un_b;
		uint64_t un_d_pad;		/* force 8 byte alignment */
	} d_un;
#define	d_packname	d_un.un_d_packname
#define	d_boot0		d_un.un_b.un_d_boot0
#define	d_boot1		d_un.un_b.un_d_boot1

			/* disk geometry: */
	uint32_t d_secsize;		/* # of bytes per sector */
	uint32_t d_nsectors;		/* # of data sectors per track */
	uint32_t d_ntracks;		/* # of tracks per cylinder */
	uint32_t d_ncylinders;		/* # of data cylinders per unit */
	uint32_t d_secpercyl;		/* # of data sectors per cylinder */
	uint32_t d_secperunit;		/* # of data sectors per unit */

	/*
	 * Spares (bad sector replacements) below are not counted in
	 * d_nsectors or d_secpercyl.  Spare sectors are assumed to
	 * be physical sectors which occupy space at the end of each
	 * track and/or cylinder.
	 */
	uint16_t d_sparespertrack;	/* # of spare sectors per track */
	uint16_t d_sparespercyl;	/* # of spare sectors per cylinder */
	/*
	 * Alternative cylinders include maintenance, replacement,
	 * configuration description areas, etc.
	 */
	uint32_t d_acylinders;		/* # of alt. cylinders per unit */

			/* hardware characteristics: */
	/*
	 * d_interleave, d_trackskew and d_cylskew describe perturbations
	 * in the media format used to compensate for a slow controller.
	 * Interleave is physical sector interleave, set up by the
	 * formatter or controller when formatting.  When interleaving is
	 * in use, logically adjacent sectors are not physically
	 * contiguous, but instead are separated by some number of
	 * sectors.  It is specified as the ratio of physical sectors
	 * traversed per logical sector.  Thus an interleave of 1:1
	 * implies contiguous layout, while 2:1 implies that logical
	 * sector 0 is separated by one sector from logical sector 1.
	 * d_trackskew is the offset of sector 0 on track N relative to
	 * sector 0 on track N-1 on the same cylinder.  Finally, d_cylskew
	 * is the offset of sector 0 on cylinder N relative to sector 0
	 * on cylinder N-1.
	 */
	uint16_t d_rpm;		/* rotational speed */
	uint16_t d_interleave;		/* hardware sector interleave */
	uint16_t d_trackskew;		/* sector 0 skew, per track */
	uint16_t d_cylskew;		/* sector 0 skew, per cylinder */
	uint32_t d_headswitch;		/* head switch time, usec */
	uint32_t d_trkseek;		/* track-to-track seek, usec */
	uint32_t d_flags;		/* generic flags */
#define	NDDATA 5
	uint32_t d_drivedata[NDDATA];	/* drive-type specific information */
#define	NSPARE 5
	uint32_t d_spare[NSPARE];	/* reserved for future use */
	uint32_t d_magic2;		/* the magic number (again) */
	uint16_t d_checksum;		/* xor of data incl. partitions */

			/* filesystem and partition information: */
	uint16_t d_npartitions;	/* number of partitions in following */
	uint32_t d_bbsize;		/* size of boot area at sn0, bytes */
	uint32_t d_sbsize;		/* max size of fs superblock, bytes */
	struct	partition {		/* the partition table */
		uint32_t p_size;	/* number of sectors in partition */
		uint32_t p_offset;	/* starting sector */
		union {
			uint32_t fsize; /* FFS, ADOS:
					    filesystem basic fragment size */
			uint32_t cdsession; /* ISO9660: session offset */
		} __partition_u2;
#define	p_fsize		__partition_u2.fsize
#define	p_cdsession	__partition_u2.cdsession
		uint8_t p_fstype;	/* filesystem type, see below */
		uint8_t p_frag;	/* filesystem fragments per block */
		union {
			uint16_t cpg;	/* UFS: FS cylinders per group */
			uint16_t sgs;	/* LFS: FS segment shift */
		} __partition_u1;
#define	p_cpg	__partition_u1.cpg
#define	p_sgs	__partition_u1.sgs
	} d_partitions[MAXPARTITIONS];	/* actually may be more */
};

#else /* _LOCORE */
	/*
	 * offsets for asm boot files.
	 */
	.set	d_secsize,40
	.set	d_nsectors,44
	.set	d_ntracks,48
	.set	d_ncylinders,52
	.set	d_secpercyl,56
	.set	d_secperunit,60
	.set	d_end_,148+(MAXPARTITIONS*16)
#endif /* _LOCORE */

/*
 * We normally use C99 initialisers (just in case the lists below are out
 * of sequence, or have gaps), but lint doesn't grok them.
 * Maybe some host compilers don't either, but many have for quite some time.
 */

#ifndef lint
#define ARRAY_INIT(element,value) [element]=value
#else
#define ARRAY_INIT(element,value) value
#endif

/* Use pre-processor magic to get all the parameters one one line... */

/* d_type values: */
#define DKTYPE_DEFN(x) \
x(UNKNOWN,	0,	"unknown") \
x(SMD,		1,	"SMD")		/* SMD, XSMD; VAX hp/up */ \
x(MSCP,		2,	"MSCP")		/* MSCP */ \
x(DEC,		3,	"old DEC")	/* other DEC (rk, rl) */ \
x(SCSI,		4,	"SCSI")		/* SCSI */ \
x(ESDI,		5,	"ESDI")		/* ESDI interface */ \
x(ST506,	6,	"ST506")	/* ST506 etc. */ \
x(HPIB,		7,	"HP-IB")	/* CS/80 on HP-IB */ \
x(HPFL,		8,	"HP-FL")	/* HP Fiber-link */ \
x(TYPE_9,	9,	"type 9") \
x(FLOPPY,	10,	"floppy")	/* floppy */ \
x(ATAPI,	11,	"ATAPI")	/* ATAPI */ \
x(LD,		12,	"ld")		/* logical disk */ \
x(JFS2,		13,	"jfs")		/* IBM JFS2 */ \
x(VINUM,	14,	"vinum")	/* vinum volume */ \
x(FLASH,	15,	"flash")	/* flash memory devices */ \
x(DM,		16,	"dm")		/* device-mapper pseudo-disk devices */\
    
#ifndef _LOCORE
#define DKTYPE_NUMS(tag, number, name) __CONCAT(DTYPE_,tag=number),
#ifndef DKTYPE_ENUMNAME
#define DKTYPE_ENUMNAME
#endif
enum DKTYPE_ENUMNAME { DKTYPE_DEFN(DKTYPE_NUMS) DKMAXTYPES };
#undef	DKTYPE_NUMS
#endif

#ifdef DKTYPENAMES
#define	DKTYPE_NAMES(tag, number, name) ARRAY_INIT(number,name),
static const char *const dktypenames[] = { DKTYPE_DEFN(DKTYPE_NAMES) NULL };
#undef	DKTYPE_NAMES
#endif

/*
 * Partition type names, numbers, label-names, fsck prog, and mount prog
 */
#define	FSTYPE_DEFN(x) \
x(UNUSED,   0, "unused",     NULL,    NULL)   /* unused */ \
x(SWAP,     1, "swap",       NULL,    NULL)   /* swap */ \
x(BSDFFS,   2, "4.2BSD",    "ffs",   "ffs")   /* 4.2BSD fast file system */ \
x(MSDOS,    3, "MSDOS",     "msdos", "msdos") /* MSDOS file system */ \
x(OTHER,    4, "unknown",    NULL,    NULL)   /* in use, unknown/unsupported */\
x(ISO9660,  5, "ISO9660",    NULL,   "cd9660")/* ISO 9660, normally CD-ROM */ \
x(BOOT,     6, "boot",       NULL,    NULL)   /* bootstrap code in partition */\
x(EX2FS,    7, "Linux Ext2","ext2fs","ext2fs")/* Linux Extended 2 FS */ \
x(NTFS,     8, "NTFS",       NULL,   "ntfs")  /* Windows/NT file system */ \
x(UDF,      9, "UDF",        NULL,   "udf")   /* UDF */ \


#ifndef _LOCORE
#define	FS_TYPENUMS(tag, number, name, fsck, mount) __CONCAT(FS_,tag=number),
#ifndef FSTYPE_ENUMNAME
#define FSTYPE_ENUMNAME
#endif
enum FSTYPE_ENUMNAME { FSTYPE_DEFN(FS_TYPENUMS) FSMAXTYPES };
#undef	FS_TYPENUMS
#endif

#ifdef	FSTYPENAMES
#define	FS_TYPENAMES(tag, number, name, fsck, mount) ARRAY_INIT(number,name),
static const char *const fstypenames[] = { FSTYPE_DEFN(FS_TYPENAMES) NULL };
#undef	FS_TYPENAMES
#endif

#ifdef FSCKNAMES
/* These are the names MOUNT_XXX from <sys/mount.h> */
#define	FS_FSCKNAMES(tag, number, name, fsck, mount) ARRAY_INIT(number,fsck),
static const char *const fscknames[] = { FSTYPE_DEFN(FS_FSCKNAMES) NULL };
#undef	FS_FSCKNAMES
#define	FSMAXNAMES	FSMAXTYPES
#endif

#ifdef MOUNTNAMES
/* These are the names MOUNT_XXX from <sys/mount.h> */
#define	FS_MOUNTNAMES(tag, number, name, fsck, mount) ARRAY_INIT(number,mount),
static const char *const mountnames[] = { FSTYPE_DEFN(FS_MOUNTNAMES) NULL };
#undef	FS_MOUNTNAMES
#define	FSMAXMOUNTNAMES	FSMAXTYPES
#endif

/*
 * flags shared by various drives:
 */
#define		D_REMOVABLE	0x01		/* removable media */
#define		D_ECC		0x02		/* supports ECC */
#define		D_BADSECT	0x04		/* supports bad sector forw. */
#define		D_RAMDISK	0x08		/* disk emulator */
#define		D_CHAIN		0x10		/* can do back-back transfers */
#define		D_SCSI_MMC	0x20		/* SCSI MMC sessioned media */

/*
 * Drive data for SMD.
 */
#define	d_smdflags	d_drivedata[0]
#define		D_SSE		0x1		/* supports skip sectoring */
#define	d_mindist	d_drivedata[1]
#define	d_maxdist	d_drivedata[2]
#define	d_sdist		d_drivedata[3]

/*
 * Drive data for ST506.
 */
#define	d_precompcyl	d_drivedata[0]
#define	d_gap3		d_drivedata[1]		/* used only when formatting */

/*
 * Drive data for SCSI.
 */
#define	d_blind		d_drivedata[0]

#ifndef _LOCORE
/*
 * Structure used to perform a format or other raw operation,
 * returning data and/or register values.  Register identification
 * and format are device- and driver-dependent. Currently unused.
 */
struct format_op {
	char	*df_buf;
	int	 df_count;		/* value-result */
	daddr_t	 df_startblk;
	int	 df_reg[8];		/* result */
};

#ifdef _KERNEL
/*
 * Structure used internally to retrieve information about a partition
 * on a disk.
 */
struct partinfo {
	struct disklabel *disklab;
	struct partition *part;
};

struct disk;

int disk_read_sectors(void (*)(struct buf *), const struct disklabel *,
    struct buf *, unsigned int, int);
void	 diskerr(const struct buf *, const char *, const char *, int,
	    int, const struct disklabel *);
u_int	 dkcksum(struct disklabel *);
u_int	 dkcksum_sized(struct disklabel *, size_t);
int	 setdisklabel(struct disklabel *, struct disklabel *, u_long,
	    struct cpu_disklabel *);
const char *readdisklabel(dev_t, void (*)(struct buf *),
	    struct disklabel *, struct cpu_disklabel *);
int	 writedisklabel(dev_t, void (*)(struct buf *), struct disklabel *,
	    struct cpu_disklabel *);
const char *convertdisklabel(struct disklabel *, void (*)(struct buf *),
    struct buf *, uint32_t);
int	 bounds_check_with_label(struct disk *, struct buf *, int);
int	 bounds_check_with_mediasize(struct buf *, int, uint64_t);
#endif
#endif /* _LOCORE */

#if !defined(_KERNEL) && !defined(_LOCORE)

#include <sys/cdefs.h>

#endif

/*
 * Definitions for the EFI GUID Partition Table disk partitioning scheme.
 *
 * NOTE: As EFI is an Intel specification, all fields are stored in
 * little-endian byte-order.
 */

/*
 * GUID Partition Table Header
 */
struct gpt_hdr {
	int8_t		hdr_sig[8];	/* identifies GUID Partition Table */
	uint32_t	hdr_revision;	/* GPT specification revsion */
	uint32_t	hdr_size;	/* size of GPT Header */
	uint32_t	hdr_crc_self;	/* CRC32 of GPT Header */
	uint32_t	hdr__rsvd0;	/* must be zero */
	uint64_t	hdr_lba_self;	/* LBA that contains this Header */
	uint64_t	hdr_lba_alt;	/* LBA of backup GPT Header */
	uint64_t	hdr_lba_start;	/* first LBA usable for partitions */
	uint64_t	hdr_lba_end;	/* last LBA usable for partitions */
	uint8_t		hdr_guid[16];	/* GUID to identify the disk */
	uint64_t	hdr_lba_table;	/* first LBA of GPE array */
	uint32_t	hdr_entries;	/* number of entries in GPE array */
	uint32_t	hdr_entsz;	/* size of each GPE */
	uint32_t	hdr_crc_table;	/* CRC32 of GPE array */
	/*
	 * The remainder of the block that contains the GPT Header
	 * is reserved by EFI for future GPT Header expansion, and
	 * must be zero.
	 */
};

#define	GPT_HDR_SIG		"EFI PART"
#define	GPT_HDR_REVISION	0x00010000	/* 1.0 */

#define	GPT_HDR_BLKNO		1

#define	GPT_HDR_SIZE		0x5c

/*
 * GUID Partition Entry
 */
struct gpt_ent {
	uint8_t		ent_type[16];	/* partition type GUID */
	uint8_t		ent_guid[16];	/* unique partition GUID */
	uint64_t	ent_lba_start;	/* start of partition */
	uint64_t	ent_lba_end;	/* end of partition */
	uint64_t	ent_attr;	/* partition attributes */
	uint16_t	ent_name[36];	/* partition name in UNICODE-16 */
};

#define	GPT_ENT_ATTR_REQUIRED_PARTITION		(1ULL << 0)
					/* required for platform to function */
#define	GPT_ENT_ATTR_NO_BLOCK_IO_PROTOCOL	(1ULL << 1)
					/* UEFI won't recognize file system */
#define	GPT_ENT_ATTR_LEGACY_BIOS_BOOTABLE	(1ULL << 2)
					/* legacy BIOS boot partition */
/* The following three entries are from FreeBSD. */
#define GPT_ENT_ATTR_BOOTME			(1ULL << 59)
					/* indicates a bootable partition */
#define GPT_ENT_ATTR_BOOTONCE			(1ULL << 58)
				/* attempt to boot this partition only once */
#define GPT_ENT_ATTR_BOOTFAILED			(1ULL << 57)
		/* partition that was marked bootonce but failed to boot */

/*
 * Partition types defined by the EFI specification:
 *
 *	GPT_ENT_TYPE_UNUSED		Unused Entry
 *	GPT_ENT_TYPE_EFI		EFI System Partition
 *	GPT_ENT_TYPE_MBR		Partition containing legacy MBR
 */
#define	GPT_ENT_TYPE_UNUSED		\
	{0x00000000,0x0000,0x0000,0x00,0x00,{0x00,0x00,0x00,0x00,0x00,0x00}}
#define	GPT_ENT_TYPE_EFI		\
	{0xc12a7328,0xf81f,0x11d2,0xba,0x4b,{0x00,0xa0,0xc9,0x3e,0xc9,0x3b}}
#define	GPT_ENT_TYPE_MBR		\
	{0x024dee41,0x33e7,0x11d3,0x9d,0x69,{0x00,0x08,0xc7,0x81,0xf3,0x9f}}

/*
 * Partition types defined by other operating systems.
 */
#define	GPT_ENT_TYPE_NETBSD_SWAP	\
	{0x49f48d32,0xb10e,0x11dc,0xb9,0x9b,{0x00,0x19,0xd1,0x87,0x96,0x48}}
#define	GPT_ENT_TYPE_NETBSD_FFS		\
	{0x49f48d5a,0xb10e,0x11dc,0xb9,0x9b,{0x00,0x19,0xd1,0x87,0x96,0x48}}
#define	GPT_ENT_TYPE_NETBSD_LFS		\
	{0x49f48d82,0xb10e,0x11dc,0xb9,0x9b,{0x00,0x19,0xd1,0x87,0x96,0x48}}

#define	GPT_ENT_TYPE_FREEBSD		\
	{0x516e7cb4,0x6ecf,0x11d6,0x8f,0xf8,{0x00,0x02,0x2d,0x09,0x71,0x2b}}
#define	GPT_ENT_TYPE_FREEBSD_SWAP	\
	{0x516e7cb5,0x6ecf,0x11d6,0x8f,0xf8,{0x00,0x02,0x2d,0x09,0x71,0x2b}}
#define	GPT_ENT_TYPE_FREEBSD_UFS	\
	{0x516e7cb6,0x6ecf,0x11d6,0x8f,0xf8,{0x00,0x02,0x2d,0x09,0x71,0x2b}}
#define	GPT_ENT_TYPE_FREEBSD_VINUM	\
	{0x516e7cb8,0x6ecf,0x11d6,0x8f,0xf8,{0x00,0x02,0x2d,0x09,0x71,0x2b}}
#define GPT_ENT_TYPE_FREEBSD_ZFS	\
	{0x516e7cba,0x6ecf,0x11d6,0x8f,0xf8,{0x00,0x02,0x2d,0x09,0x71,0x2b}}
/*
 * The following are unused but documented here to avoid reuse.
 *
 *      GPT_ENT_TYPE_FREEBSD_UFS2	\
 *	{0x516e7cb7,0x6ecf,0x11d6,0x8f,0xf8,{0x00,0x02,0x2d,0x09,0x71,0x2b}}
 */

#define	GPT_ENT_TYPE_MS_RESERVED	\
	{0xe3c9e316,0x0b5c,0x4db8,0x81,0x7d,{0xf9,0x2d,0xf0,0x02,0x15,0xae}}
#define	GPT_ENT_TYPE_MS_BASIC_DATA	\
	{0xebd0a0a2,0xb9e5,0x4433,0x87,0xc0,{0x68,0xb6,0xb7,0x26,0x99,0xc7}}
#define	GPT_ENT_TYPE_MS_LDM_METADATA	\
	{0x5808c8aa,0x7e8f,0x42e0,0x85,0xd2,{0xe1,0xe9,0x04,0x34,0xcf,0xb3}}
#define	GPT_ENT_TYPE_MS_LDM_DATA	\
	{0xaf9b60a0,0x1431,0x4f62,0xbc,0x68,{0x33,0x11,0x71,0x4a,0x69,0xad}}

/*
 * Linux originally used GPT_ENT_TYPE_MS_BASIC_DATA in place of
 * GPT_ENT_TYPE_LINUX_DATA.
 */
#define	GPT_ENT_TYPE_LINUX_DATA		\
	{0x0fc63daf,0x8483,0x4772,0x8e,0x79,{0x3d,0x69,0xd8,0x47,0x7d,0xe4}}
#define	GPT_ENT_TYPE_LINUX_SWAP		\
	{0x0657fd6d,0xa4ab,0x43c4,0x84,0xe5,{0x09,0x33,0xc8,0x4b,0x4f,0x4f}}
#define	GPT_ENT_TYPE_LINUX_LVM		\
	{0xe6d6d379,0xf507,0x44c2,0xa2,0x3c,{0x23,0x8f,0x2a,0x3d,0xf9,0x28}}

#define	GPT_ENT_TYPE_APPLE_HFS		\
	{0x48465300,0x0000,0x11aa,0xaa,0x11,{0x00,0x30,0x65,0x43,0xec,0xac}}
#define	GPT_ENT_TYPE_APPLE_UFS		\
	{0x55465300,0x0000,0x11aa,0xaa,0x11,{0x00,0x30,0x65,0x43,0xec,0xac}}

/*
 * Used by GRUB 2.
 */
#define	GPT_ENT_TYPE_BIOS		\
	{0x21686148,0x6449,0x6e6f,0x74,0x4e,{0x65,0x65,0x64,0x45,0x46,0x49}}

#endif /* !_SYS_DISKLABEL_H_ */
