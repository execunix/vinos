/*	$NetBSD: bootblock.h,v 1.56 2014/02/24 07:23:44 skrll Exp $	*/

/*-
 * Copyright (c) 2002-2004 The NetBSD Foundation, Inc.
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
/*-
 * Copyright (C) 1993	Allen K. Briggs, Chris P. Caputo,
 *			Michael L. Finch, Bradley A. Grantham, and
 *			Lawrence A. Kesteloot
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Alice Group.
 * 4. The names of the Alice Group or any of its members may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ALICE GROUP ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE ALICE GROUP BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Copyright (c) 1994, 1999 Christopher G. Demetriou
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Christopher G. Demetriou
 *      for the NetBSD Project.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright (c) 1994 Rolf Grossmann
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Rolf Grossmann.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SYS_BOOTBLOCK_H
#define	_SYS_BOOTBLOCK_H

#if !defined(__ASSEMBLER__)
#include <sys/cdefs.h>
#if defined(_KERNEL) || defined(_STANDALONE)
#include <sys/stdint.h>
#else
#include <stdint.h>
#endif
#endif	/* !defined(__ASSEMBLER__) */

/* ------------------------------------------
 * MBR (Master Boot Record) --
 *	definitions for systems that use MBRs
 */

/*
 * Layout of boot records:
 *
 *	Byte range	Use	Description
 *	----------	---	-----------
 *
 *	0 - 2		FMP	JMP xxx, NOP
 *	3 - 10		FP	OEM Name
 *
 *	11 - 61		FMP	FAT12/16 BPB
 *				Whilst not strictly necessary for MBR,
 *				GRUB reserves this area
 *
 *	11 - 89		P	FAT32 BPB
 *				(are we ever going to boot off this?)
 *
 *
 *	62 - 217	FMP	Boot code
 *
 *	90 - 217	P	FAT32 boot code
 *
 *	218 - 223	M	Win95b/98/me "drive time"
 *		http://www.geocities.com/thestarman3/asm/mbr/95BMEMBR.htm#MYST
 *				only changed if all 6 bytes are 0
 *
 *	224 - 436	FMP	boot code (continued)
 *
 *	437 - 439	M	WinNT/2K/XP MBR "boot language"
 *		http://www.geocities.com/thestarman3/asm/mbr/Win2kmbr.htm
 *				not needed by us
 *
 *	400 - 439	MP	NetBSD: mbr_bootsel
 *
 *	424 - 439	M	NetBSD: bootptn_guid (in GPT PMBR only)
 *
 *	440 - 443	M	WinNT/2K/XP Drive Serial Number (NT DSN)
 *		http://www.geocities.com/thestarman3/asm/mbr/Win2kmbr.htm
 *
 *	444 - 445	FMP	bootcode or unused
 *				NetBSD: mbr_bootsel_magic
 *
 *	446 - 509	M	partition table
 *
 *	510 - 511	FMP	magic number (0xAA55)
 *
 *	Use:
 *	----
 *	F	Floppy boot sector
 *	M	Master Boot Record
 *	P	Partition Boot record
 *
 */

/*
 * MBR (Master Boot Record)
 */
#define	MBR_BBSECTOR		0	/* MBR relative sector # */
#define	MBR_BPB_OFFSET		11	/* offsetof(mbr_sector, mbr_bpb) */
#define	MBR_BOOTCODE_OFFSET	90	/* offsetof(mbr_sector, mbr_bootcode) */
#define	MBR_BS_OFFSET		400	/* offsetof(mbr_sector, mbr_bootsel) */
#define	MBR_BS_OLD_OFFSET	404	/* where mbr_bootsel used to be */
#define	MBR_GPT_GUID_OFFSET	424	/* location of partition GUID to boot */
#define	MBR_GPT_GUID_DEFAULT		/* default uninitialized GUID */ \
	{0xeee69d04,0x02f4,0x11e0,0x8f,0x5d,{0x00,0xe0,0x81,0x52,0x9a,0x6b}}
#define	MBR_DSN_OFFSET		440	/* offsetof(mbr_sector, mbr_dsn) */
#define	MBR_BS_MAGIC_OFFSET	444	/* offsetof(mbr_sector, mbr_bootsel_magic) */
#define	MBR_PART_OFFSET		446	/* offsetof(mbr_sector, mbr_part[0]) */
#define	MBR_MAGIC_OFFSET	510	/* offsetof(mbr_sector, mbr_magic) */
#define	MBR_MAGIC		0xaa55	/* MBR magic number */
#define	MBR_BS_MAGIC		0xb5e1	/* mbr_bootsel magic number */
#define	MBR_PART_COUNT		4	/* Number of partitions in MBR */
#define	MBR_BS_PARTNAMESIZE	8	/* Size of name mbr_bootsel nametab */
					/* (excluding trailing NUL) */

		/* values for mbr_partition.mbrp_flag */
#define	MBR_PFLAG_ACTIVE	0x80	/* The active partition */

		/* values for mbr_partition.mbrp_type */
#define	MBR_PTYPE_UNUSED	0x00	/* Unused */
#define	MBR_PTYPE_FAT12		0x01	/* 12-bit FAT */
#define	MBR_PTYPE_XENIX_ROOT	0x02	/* XENIX / */
#define	MBR_PTYPE_XENIX_USR	0x03	/* XENIX /usr */
#define	MBR_PTYPE_FAT16S	0x04	/* 16-bit FAT, less than 32M */
#define	MBR_PTYPE_EXT		0x05	/* extended partition */
#define	MBR_PTYPE_FAT16B	0x06	/* 16-bit FAT, more than 32M */
#define	MBR_PTYPE_NTFS		0x07	/* OS/2 HPFS, NTFS, QNX2, Adv. UNIX */
#define	MBR_PTYPE_DELL		0x08	/* AIX or os, or etc. */
#define MBR_PTYPE_AIX_BOOT	0x09	/* AIX boot partition or Coherent */
#define MBR_PTYPE_OS2_BOOT	0x0a	/* O/2 boot manager or Coherent swap */
#define	MBR_PTYPE_FAT32		0x0b	/* 32-bit FAT */
#define	MBR_PTYPE_FAT32L	0x0c	/* 32-bit FAT, LBA-mapped */
#define	MBR_PTYPE_7XXX		0x0d	/* 7XXX, LBA-mapped */
#define	MBR_PTYPE_FAT16L	0x0e	/* 16-bit FAT, LBA-mapped */
#define	MBR_PTYPE_EXT_LBA	0x0f	/* extended partition, LBA-mapped */
#define	MBR_PTYPE_OPUS		0x10	/* OPUS */
#define MBR_PTYPE_OS2_DOS12	0x11 	/* OS/2 DOS 12-bit FAT */
#define MBR_PTYPE_COMPAQ_DIAG	0x12 	/* Compaq diagnostics */
#define MBR_PTYPE_OS2_DOS16S	0x14 	/* OS/2 DOS 16-bit FAT <32M */
#define MBR_PTYPE_OS2_DOS16B	0x16 	/* OS/2 DOS 16-bit FAT >=32M */
#define MBR_PTYPE_OS2_IFS	0x17 	/* OS/2 hidden IFS */
#define MBR_PTYPE_AST_SWAP	0x18 	/* AST Windows swapfile */
#define MBR_PTYPE_WILLOWTECH	0x19 	/* Willowtech Photon coS */
#define MBR_PTYPE_HID_FAT32	0x1b 	/* hidden win95 fat 32 */
#define MBR_PTYPE_HID_FAT32_LBA	0x1c 	/* hidden win95 fat 32 lba */
#define MBR_PTYPE_HID_FAT16_LBA	0x1d	/* hidden win95 fat 16 lba */
#define MBR_PTYPE_WILLOWSOFT	0x20 	/* Willowsoft OFS1 */
#define MBR_PTYPE_RESERVED_x21	0x21 	/* reserved */
#define MBR_PTYPE_RESERVED_x23	0x23 	/* reserved */
#define MBR_PTYPE_RESERVED_x24	0x24	/* NEC DOS */
#define MBR_PTYPE_RESERVED_x26	0x26 	/* reserved */
#define MBR_PTYPE_RESERVED_x31	0x31 	/* reserved */
#define MBR_PTYPE_NOS		0x32	/* Alien Internet Services NOS */
#define MBR_PTYPE_RESERVED_x33	0x33 	/* reserved */
#define MBR_PTYPE_RESERVED_x34	0x34 	/* reserved */
#define MBR_PTYPE_OS2_JFS	0x35 	/* JFS on OS2 */
#define MBR_PTYPE_RESERVED_x36	0x36 	/* reserved */
#define MBR_PTYPE_THEOS		0x38 	/* Theos */
#define MBR_PTYPE_PLAN9		0x39 	/* Plan 9, or Theos spanned */
#define MBR_PTYPE_THEOS_4GB	0x3a 	/* Theos ver 4 4gb partition */
#define MBR_PTYPE_THEOS_EXT	0x3b 	/* Theos ve 4 extended partition */
#define MBR_PTYPE_PMRECOVERY	0x3c 	/* PartitionMagic recovery */
#define MBR_PTYPE_HID_NETWARE	0x3d 	/* Hidden Netware */
#define MBR_PTYPE_VENIX		0x40 	/* VENIX 286 or LynxOS */
#define	MBR_PTYPE_PREP		0x41	/* PReP */
#define	MBR_PTYPE_DRDOS_LSWAP	0x42	/* linux swap sharing DRDOS disk */
#define	MBR_PTYPE_DRDOS_LINUX	0x43	/* linux sharing DRDOS disk */
#define	MBR_PTYPE_GOBACK	0x44	/* GoBack change utility */
#define	MBR_PTYPE_BOOT_US	0x45	/* Boot US Boot manager */
#define	MBR_PTYPE_EUMEL_x46	0x46	/* EUMEL/Elan or Ergos 3 */
#define	MBR_PTYPE_EUMEL_x47	0x47	/* EUMEL/Elan or Ergos 3 */
#define	MBR_PTYPE_EUMEL_x48	0x48	/* EUMEL/Elan or Ergos 3 */
#define	MBR_PTYPE_ALFS_THIN	0x4a	/* ALFX/THIN filesystem for DOS */
#define	MBR_PTYPE_OBERON	0x4c	/* Oberon partition */
#define MBR_PTYPE_QNX4X		0x4d 	/* QNX4.x */
#define MBR_PTYPE_QNX4X_2	0x4e 	/* QNX4.x 2nd part */
#define MBR_PTYPE_QNX4X_3	0x4f 	/* QNX4.x 3rd part */
#define MBR_PTYPE_DM		0x50 	/* DM (disk manager) */
#define MBR_PTYPE_DM6_AUX1	0x51 	/* DM6 Aux1 (or Novell) */
#define MBR_PTYPE_CPM		0x52 	/* CP/M or Microport SysV/AT */
#define MBR_PTYPE_DM6_AUX3	0x53 	/* DM6 Aux3 */
#define	MBR_PTYPE_DM6_DDO	0x54	/* DM6 DDO */
#define MBR_PTYPE_EZDRIVE	0x55	/* EZ-Drive (disk manager) */
#define MBR_PTYPE_GOLDEN_BOW	0x56	/* Golden Bow (disk manager) */
#define MBR_PTYPE_DRIVE_PRO	0x57	/* Drive PRO */
#define MBR_PTYPE_PRIAM_EDISK	0x5c	/* Priam Edisk (disk manager) */
#define MBR_PTYPE_SPEEDSTOR	0x61	/* SpeedStor */
#define MBR_PTYPE_HURD		0x63	/* GNU HURD or Mach or Sys V/386 */
#define MBR_PTYPE_NOVELL_2XX	0x64	/* Novell Netware 2.xx or Speedstore */
#define MBR_PTYPE_NOVELL_3XX	0x65	/* Novell Netware 3.xx */
#define MBR_PTYPE_NOVELL_386	0x66	/* Novell 386 Netware */
#define MBR_PTYPE_NOVELL_x67	0x67	/* Novell */
#define MBR_PTYPE_NOVELL_x68	0x68	/* Novell */
#define MBR_PTYPE_NOVELL_x69	0x69	/* Novell */
#define MBR_PTYPE_DISKSECURE	0x70	/* DiskSecure Multi-Boot */
#define MBR_PTYPE_RESERVED_x71	0x71	/* reserved */
#define MBR_PTYPE_RESERVED_x73	0x73	/* reserved */
#define MBR_PTYPE_RESERVED_x74	0x74	/* reserved */
#define MBR_PTYPE_PCIX		0x75	/* PC/IX */
#define MBR_PTYPE_RESERVED_x76	0x76	/* reserved */
#define MBR_PTYPE_M2FS_M2CS	0x77	/* M2FS/M2CS partition */
#define MBR_PTYPE_XOSL_FS	0x78	/* XOSL boot loader filesystem */
#define MBR_PTYPE_MINIX_14A	0x80	/* MINIX until 1.4a */
#define MBR_PTYPE_MINIX_14B	0x81	/* MINIX since 1.4b */
#define	MBR_PTYPE_LNXSWAP	0x82	/* Linux swap or Solaris */
#define	MBR_PTYPE_LNXEXT2	0x83	/* Linux native */
#define MBR_PTYPE_OS2_C		0x84	/* OS/2 hidden C: drive */
#define	MBR_PTYPE_EXT_LNX	0x85	/* Linux extended partition */
#define	MBR_PTYPE_NTFATVOL 	0x86	/* NT FAT volume set */
#define	MBR_PTYPE_NTFSVOL	0x87	/* NTFS volume set or HPFS mirrored */
#define	MBR_PTYPE_LNX_KERNEL	0x8a	/* Linux Kernel AiR-BOOT partition */
#define	MBR_PTYPE_FT_FAT32	0x8b	/* Legacy Fault tolerant FAT32 */
#define	MBR_PTYPE_FT_FAT32_EXT	0x8c	/* Legacy Fault tolerant FAT32 ext */
#define	MBR_PTYPE_HID_FR_FD_12	0x8d	/* Hidden free FDISK FAT12 */
#define	MBR_PTYPE_LNX_LVM	0x8e	/* Linux Logical Volume Manager */
#define	MBR_PTYPE_HID_FR_FD_16	0x90	/* Hidden free FDISK FAT16 */
#define	MBR_PTYPE_HID_FR_FD_EXT	0x91	/* Hidden free FDISK DOS EXT */
#define	MBR_PTYPE_HID_FR_FD_16B	0x92	/* Hidden free FDISK FAT16 Big */
#define MBR_PTYPE_AMOEBA_FS 	0x93	/* Amoeba filesystem */
#define MBR_PTYPE_AMOEBA_BAD 	0x94	/* Amoeba bad block table */
#define MBR_PTYPE_MIT_EXOPC 	0x95	/* MIT EXOPC native partitions */
#define	MBR_PTYPE_HID_FR_FD_32	0x97	/* Hidden free FDISK FAT32 */
#define	MBR_PTYPE_DATALIGHT	0x98	/* Datalight ROM-DOS Super-Boot */
#define MBR_PTYPE_MYLEX 	0x99	/* Mylex EISA SCSI */
#define	MBR_PTYPE_HID_FR_FD_16L	0x9a	/* Hidden free FDISK FAT16 LBA */
#define	MBR_PTYPE_HID_FR_FD_EXL	0x9b	/* Hidden free FDISK EXT LBA */
#define MBR_PTYPE_BSDI	 	0x9f	/* BSDI? */
#define MBR_PTYPE_IBM_HIB	0xa0	/* IBM Thinkpad hibernation */
#define MBR_PTYPE_HP_VOL_xA1	0xa1	/* HP Volume expansion (SpeedStor) */
#define MBR_PTYPE_HP_VOL_xA3	0xa3	/* HP Volume expansion (SpeedStor) */
#define MBR_PTYPE_HP_VOL_xA4	0xa4	/* HP Volume expansion (SpeedStor) */
#define	MBR_PTYPE_386BSD	0xa5	/* 386BSD partition type */
#define	MBR_PTYPE_OPENBSD	0xa6	/* OpenBSD partition type */
#define	MBR_PTYPE_NEXTSTEP_486 	0xa7	/* NeXTSTEP 486 */
#define	MBR_PTYPE_APPLE_UFS 	0xa8	/* Apple UFS */
#define	MBR_PTYPE_NETBSD	0xa9	/* NetBSD partition type */
#define MBR_PTYPE_OLIVETTI	0xaa	/* Olivetty Fat12 1.44MB Service part */
#define MBR_PTYPE_APPLE_BOOT	0xab	/* Apple Boot */
#define MBR_PTYPE_SHAG_OS	0xae	/* SHAG OS filesystem */
#define MBR_PTYPE_APPLE_HFS	0xaf	/* Apple HFS */
#define MBR_PTYPE_BOOTSTAR_DUM	0xb0	/* BootStar Dummy */
#define MBR_PTYPE_RESERVED_xB1	0xb1	/* reserved */
#define MBR_PTYPE_RESERVED_xB3	0xb3	/* reserved */
#define MBR_PTYPE_RESERVED_xB4	0xb4	/* reserved */
#define MBR_PTYPE_RESERVED_xB6	0xb6	/* reserved */
#define MBR_PTYPE_BSDI_386	0xb7	/* BSDI BSD/386 filesystem */
#define MBR_PTYPE_BSDI_SWAP	0xb8	/* BSDI BSD/386 swap */
#define	MBR_PTYPE_BOOT_WIZARD	0xbb	/* Boot Wizard Hidden */
#define	MBR_PTYPE_SOLARIS_8	0xbe	/* Solaris 8 partition type */
#define	MBR_PTYPE_SOLARIS	0xbf	/* Solaris partition type */
#define MBR_PTYPE_CTOS		0xc0 	/* CTOS */
#define MBR_PTYPE_DRDOS_FAT12	0xc1 	/* DRDOS/sec (FAT-12) */
#define MBR_PTYPE_HID_LNX	0xc2 	/* Hidden Linux */
#define MBR_PTYPE_HID_LNX_SWAP	0xc3 	/* Hidden Linux swap */
#define MBR_PTYPE_DRDOS_FAT16S	0xc4 	/* DRDOS/sec (FAT-16, < 32M) */
#define MBR_PTYPE_DRDOS_EXT	0xc5 	/* DRDOS/sec (EXT) */
#define MBR_PTYPE_DRDOS_FAT16B	0xc6 	/* DRDOS/sec (FAT-16, >= 32M) */
#define MBR_PTYPE_SYRINX	0xc7 	/* Syrinx (Cyrnix?) or HPFS disabled */
#define MBR_PTYPE_DRDOS_8_xC8	0xc8 	/* Reserved for DR-DOS 8.0+ */
#define MBR_PTYPE_DRDOS_8_xC9	0xc9 	/* Reserved for DR-DOS 8.0+ */
#define MBR_PTYPE_DRDOS_8_xCA	0xca 	/* Reserved for DR-DOS 8.0+ */
#define MBR_PTYPE_DRDOS_74_CHS	0xcb 	/* DR-DOS 7.04+ Secured FAT32 CHS */
#define MBR_PTYPE_DRDOS_74_LBA	0xcc 	/* DR-DOS 7.04+ Secured FAT32 LBA */
#define MBR_PTYPE_CTOS_MEMDUMP	0xcd	/* CTOS Memdump */
#define MBR_PTYPE_DRDOS_74_16X	0xce 	/* DR-DOS 7.04+ FAT16X LBA */
#define MBR_PTYPE_DRDOS_74_EXT	0xcf 	/* DR-DOS 7.04+ EXT LBA */
#define MBR_PTYPE_REAL32	0xd0 	/* REAL/32 secure big partition */
#define MBR_PTYPE_MDOS_FAT12	0xd1 	/* Old Multiuser DOS FAT12 */
#define MBR_PTYPE_MDOS_FAT16S	0xd4 	/* Old Multiuser DOS FAT16 Small */
#define MBR_PTYPE_MDOS_EXT	0xd5 	/* Old Multiuser DOS Extended */
#define MBR_PTYPE_MDOS_FAT16B	0xd6 	/* Old Multiuser DOS FAT16 Big */
#define MBR_PTYPE_CPM_86	0xd8 	/* CP/M 86 */
#define MBR_PTYPE_CONCURRENT	0xdb 	/* CP/M or Concurrent CP/M */
#define MBR_PTYPE_HID_CTOS_MEM	0xdd 	/* Hidden CTOS Memdump */
#define MBR_PTYPE_DELL_UTIL	0xde 	/* Dell PowerEdge Server utilities */
#define MBR_PTYPE_DGUX_VIRTUAL	0xdf 	/* DG/UX virtual disk manager */
#define MBR_PTYPE_STMICROELEC	0xe0 	/* STMicroelectronics ST AVFS */
#define MBR_PTYPE_DOS_ACCESS	0xe1 	/* DOS access or SpeedStor 12-bit */
#define MBR_PTYPE_STORDIM	0xe3 	/* DOS R/O or Storage Dimensions */
#define MBR_PTYPE_SPEEDSTOR_16S	0xe4 	/* SpeedStor 16-bit FAT < 1024 cyl. */
#define MBR_PTYPE_RESERVED_xE5	0xe5	/* reserved */
#define MBR_PTYPE_RESERVED_xE6	0xe6	/* reserved */
#define MBR_PTYPE_BEOS		0xeb 	/* BeOS */
#define	MBR_PTYPE_PMBR		0xee	/* GPT Protective MBR */
#define	MBR_PTYPE_EFI		0xef	/* EFI system partition */
#define MBR_PTYPE_LNX_PA_RISC	0xf0 	/* Linux PA-RISC boot loader */
#define MBR_PTYPE_SPEEDSTOR_X	0xf1 	/* SpeedStor or Storage Dimensions */
#define MBR_PTYPE_DOS33_SEC	0xf2 	/* DOS 3.3+ Secondary */
#define MBR_PTYPE_RESERVED_xF3	0xf3	/* reserved */
#define MBR_PTYPE_SPEEDSTOR_L	0xf4	/* SpeedStor large partition */
#define MBR_PTYPE_PROLOGUE	0xf5	/* Prologue multi-volumen partition */
#define MBR_PTYPE_RESERVED_xF6	0xf6 	/* reserved */
#define MBR_PTYPE_PCACHE	0xf9 	/* pCache: ext2/ext3 persistent cache */
#define MBR_PTYPE_BOCHS		0xfa 	/* Bochs x86 emulator */
#define MBR_PTYPE_VMWARE	0xfb 	/* VMware File System */
#define MBR_PTYPE_VMWARE_SWAP	0xfc 	/* VMware Swap */
#define MBR_PTYPE_LNX_RAID	0xfd 	/* Linux RAID partition persistent sb */
#define MBR_PTYPE_LANSTEP	0xfe	/* LANstep or IBM PS/2 IML */
#define MBR_PTYPE_XENIX_BAD	0xff 	/* Xenix Bad Block Table */

#ifdef MBRPTYPENAMES
static const struct mbr_ptype {
	int id;
	const char *name;
} mbr_ptypes[] = {
	{ MBR_PTYPE_UNUSED, "<UNUSED>" },
	{ MBR_PTYPE_FAT12, "Primary DOS with 12 bit FAT" },
	{ MBR_PTYPE_XENIX_ROOT, "XENIX / filesystem" },
	{ MBR_PTYPE_XENIX_USR, "XENIX /usr filesystem" },
	{ MBR_PTYPE_FAT16S, "Primary DOS with 16 bit FAT <32M" },
	{ MBR_PTYPE_EXT, "Extended partition" },
	{ MBR_PTYPE_FAT16B, "Primary 'big' DOS, 16-bit FAT (> 32MB)" },
	{ MBR_PTYPE_NTFS, "NTFS, OS/2 HPFS, QNX2 or Advanced UNIX" },
	{ MBR_PTYPE_DELL, "AIX filesystem or OS/2 (thru v1.3) or DELL "
			  "multiple drives or Commodore DOS or SplitDrive" },
	{ MBR_PTYPE_AIX_BOOT, "AIX boot partition or Coherent" },
	{ MBR_PTYPE_OS2_BOOT, "OS/2 Boot Manager or Coherent swap or OPUS" },
	{ MBR_PTYPE_FAT32, "Primary DOS with 32 bit FAT" },
	{ MBR_PTYPE_FAT32L, "Primary DOS with 32 bit FAT - LBA" },
	{ MBR_PTYPE_7XXX, "Type 7??? - LBA" },
	{ MBR_PTYPE_FAT16L, "DOS (16-bit FAT) - LBA" },
	{ MBR_PTYPE_EXT_LBA, "Ext. partition - LBA" },
	{ MBR_PTYPE_OPUS, "OPUS" },
	{ MBR_PTYPE_OS2_DOS12, "OS/2 BM: hidden DOS 12-bit FAT" },
	{ MBR_PTYPE_COMPAQ_DIAG, "Compaq diagnostics" },
	{ MBR_PTYPE_OS2_DOS16S, "OS/2 BM: hidden DOS 16-bit FAT <32M "
				"or Novell DOS 7.0 bug" },
	{ MBR_PTYPE_OS2_DOS16B, "OS/2 BM: hidden DOS 16-bit FAT >=32M" },
	{ MBR_PTYPE_OS2_IFS, "OS/2 BM: hidden IFS" },
	{ MBR_PTYPE_AST_SWAP, "AST Windows swapfile" },
	{ MBR_PTYPE_WILLOWTECH, "Willowtech Photon coS" },
	{ MBR_PTYPE_HID_FAT32, "hidden Windows/95 FAT32" },
	{ MBR_PTYPE_HID_FAT32_LBA, "hidden Windows/95 FAT32 LBA" },
	{ MBR_PTYPE_HID_FAT16_LBA, "hidden Windows/95 FAT16 LBA" },
	{ MBR_PTYPE_WILLOWSOFT, "Willowsoft OFS1" },
	{ MBR_PTYPE_RESERVED_x21, "reserved" },
	{ MBR_PTYPE_RESERVED_x23, "reserved" },
	{ MBR_PTYPE_RESERVED_x24, "NEC DOS"},
	{ MBR_PTYPE_RESERVED_x26, "reserved" },
	{ MBR_PTYPE_RESERVED_x31, "reserved" },
	{ MBR_PTYPE_NOS, "Alien Internet Services NOS" },
	{ MBR_PTYPE_RESERVED_x33, "reserved" },
	{ MBR_PTYPE_RESERVED_x34, "reserved" },
	{ MBR_PTYPE_OS2_JFS, "JFS on OS2" },
	{ MBR_PTYPE_RESERVED_x36, "reserved" },
	{ MBR_PTYPE_THEOS, "Theos" },
	{ MBR_PTYPE_PLAN9, "Plan 9" },
	{ MBR_PTYPE_PLAN9, "Plan 9, or Theos spanned" },
	{ MBR_PTYPE_THEOS_4GB,	"Theos ver 4 4gb partition" },
	{ MBR_PTYPE_THEOS_EXT,	"Theos ve 4 extended partition" },
	{ MBR_PTYPE_PMRECOVERY, "PartitionMagic recovery" },
	{ MBR_PTYPE_HID_NETWARE, "Hidden Netware" },
	{ MBR_PTYPE_VENIX, "VENIX 286 or LynxOS" },
	{ MBR_PTYPE_PREP, "Linux/MINIX (sharing disk with DRDOS) "
			  "or Personal RISC boot" },
	{ MBR_PTYPE_DRDOS_LSWAP, "SFS or Linux swap "
				 "(sharing disk with DRDOS)" },
	{ MBR_PTYPE_DRDOS_LINUX, "Linux native (sharing disk with DRDOS)" },
	{ MBR_PTYPE_GOBACK, "GoBack change utility" },
	{ MBR_PTYPE_BOOT_US, "Boot US Boot manager" },
	{ MBR_PTYPE_EUMEL_x46, "EUMEL/Elan or Ergos 3" },
	{ MBR_PTYPE_EUMEL_x47, "EUMEL/Elan or Ergos 3" },
	{ MBR_PTYPE_EUMEL_x48, "EUMEL/Elan or Ergos 3" },
	{ MBR_PTYPE_ALFS_THIN, "ALFX/THIN filesystem for DOS" },
	{ MBR_PTYPE_OBERON, "Oberon partition" },
	{ MBR_PTYPE_QNX4X, "QNX4.x" },
	{ MBR_PTYPE_QNX4X_2, "QNX4.x 2nd part" },
	{ MBR_PTYPE_QNX4X_3, "QNX4.x 3rd part" },
	{ MBR_PTYPE_DM, "DM (disk manager)" },
	{ MBR_PTYPE_DM6_AUX1, "DM6 Aux1 (or Novell)" },
	{ MBR_PTYPE_CPM, "CP/M or Microport SysV/AT" },
	{ MBR_PTYPE_DM6_AUX3, "DM6 Aux3" },
	{ MBR_PTYPE_DM6_DDO, "DM6 DDO" },
	{ MBR_PTYPE_EZDRIVE, "EZ-Drive (disk manager)" },
	{ MBR_PTYPE_GOLDEN_BOW, "Golden Bow (disk manager)" },
	{ MBR_PTYPE_DRIVE_PRO, "Drive PRO" },
	{ MBR_PTYPE_PRIAM_EDISK, "Priam Edisk (disk manager)" },
	{ MBR_PTYPE_SPEEDSTOR, "SpeedStor" },
	{ MBR_PTYPE_HURD, "GNU HURD or Mach or Sys V/386 "
			  "(such as ISC UNIX) or MtXinu" },
	{ MBR_PTYPE_NOVELL_2XX, "Novell Netware 2.xx or Speedstore" },
	{ MBR_PTYPE_NOVELL_3XX, "Novell Netware 3.xx" },
	{ MBR_PTYPE_NOVELL_386, "Novell 386 Netware" },
	{ MBR_PTYPE_NOVELL_x67, "Novell" },
	{ MBR_PTYPE_NOVELL_x68, "Novell" },
	{ MBR_PTYPE_NOVELL_x69, "Novell" },
	{ MBR_PTYPE_DISKSECURE, "DiskSecure Multi-Boot" },
	{ MBR_PTYPE_RESERVED_x71, "reserved" },
	{ MBR_PTYPE_RESERVED_x73, "reserved" },
	{ MBR_PTYPE_RESERVED_x74, "reserved" },
	{ MBR_PTYPE_PCIX, "PC/IX" },
	{ MBR_PTYPE_RESERVED_x76, "reserved" },
	{ MBR_PTYPE_M2FS_M2CS,	"M2FS/M2CS partition" },
	{ MBR_PTYPE_XOSL_FS, "XOSL boot loader filesystem" },
	{ MBR_PTYPE_MINIX_14A, "MINIX until 1.4a" },
	{ MBR_PTYPE_MINIX_14B, "MINIX since 1.4b, early Linux, Mitac dmgr" },
	{ MBR_PTYPE_LNXSWAP, "Linux swap or Prime or Solaris" },
	{ MBR_PTYPE_LNXEXT2, "Linux native" },
	{ MBR_PTYPE_OS2_C, "OS/2 hidden C: drive" },
	{ MBR_PTYPE_EXT_LNX, "Linux extended" },
	{ MBR_PTYPE_NTFATVOL, "NT FAT volume set" },
	{ MBR_PTYPE_NTFSVOL, "NTFS volume set or HPFS mirrored" },
	{ MBR_PTYPE_LNX_KERNEL,	"Linux Kernel AiR-BOOT partition" },
	{ MBR_PTYPE_FT_FAT32, "Legacy Fault tolerant FAT32" },
	{ MBR_PTYPE_FT_FAT32_EXT, "Legacy Fault tolerant FAT32 ext" },
	{ MBR_PTYPE_HID_FR_FD_12, "Hidden free FDISK FAT12" },
	{ MBR_PTYPE_LNX_LVM, "Linux Logical Volume Manager" },
	{ MBR_PTYPE_HID_FR_FD_16, "Hidden free FDISK FAT16" },
	{ MBR_PTYPE_HID_FR_FD_EXT, "Hidden free FDISK DOS EXT" },
	{ MBR_PTYPE_HID_FR_FD_16L, "Hidden free FDISK FAT16 Large" },
	{ MBR_PTYPE_AMOEBA_FS, "Amoeba filesystem" },
	{ MBR_PTYPE_AMOEBA_BAD, "Amoeba bad block table" },
	{ MBR_PTYPE_MIT_EXOPC, "MIT EXOPC native partitions" },
	{ MBR_PTYPE_HID_FR_FD_32, "Hidden free FDISK FAT32" },
	{ MBR_PTYPE_DATALIGHT, "Datalight ROM-DOS Super-Boot" },
	{ MBR_PTYPE_MYLEX, "Mylex EISA SCSI" },
	{ MBR_PTYPE_HID_FR_FD_16L, "Hidden free FDISK FAT16 LBA" },
	{ MBR_PTYPE_HID_FR_FD_EXL, "Hidden free FDISK EXT LBA" },
	{ MBR_PTYPE_BSDI, "BSDI?" },
	{ MBR_PTYPE_IBM_HIB, "IBM Thinkpad hibernation" },
	{ MBR_PTYPE_HP_VOL_xA1, "HP Volume expansion (SpeedStor)" },
	{ MBR_PTYPE_HP_VOL_xA3, "HP Volume expansion (SpeedStor)" },
	{ MBR_PTYPE_HP_VOL_xA4, "HP Volume expansion (SpeedStor)" },
	{ MBR_PTYPE_386BSD, "FreeBSD or 386BSD or old NetBSD" },
	{ MBR_PTYPE_OPENBSD, "OpenBSD" },
	{ MBR_PTYPE_NEXTSTEP_486, "NeXTSTEP 486" },
	{ MBR_PTYPE_APPLE_UFS, "Apple UFS" },
	{ MBR_PTYPE_NETBSD, "NetBSD" },
	{ MBR_PTYPE_OLIVETTI, "Olivetty Fat12 1.44MB Service part" },
	{ MBR_PTYPE_SHAG_OS, "SHAG OS filesystem" },
	{ MBR_PTYPE_BOOTSTAR_DUM, "BootStar Dummy" },
	{ MBR_PTYPE_BOOT_WIZARD, "Boot Wizard Hidden" },
	{ MBR_PTYPE_APPLE_BOOT, "Apple Boot" },
	{ MBR_PTYPE_APPLE_HFS, "Apple HFS" },
	{ MBR_PTYPE_RESERVED_xB6, "reserved" },
	{ MBR_PTYPE_RESERVED_xB6, "reserved" },
	{ MBR_PTYPE_RESERVED_xB6, "reserved" },
	{ MBR_PTYPE_RESERVED_xB6, "reserved" },
	{ MBR_PTYPE_BSDI_386, "BSDI BSD/386 filesystem" },
	{ MBR_PTYPE_BSDI_SWAP, "BSDI BSD/386 swap" },
	{ MBR_PTYPE_SOLARIS_8, "Solaris 8 boot partition" },
	{ MBR_PTYPE_SOLARIS, "Solaris boot partition" },
	{ MBR_PTYPE_CTOS, "CTOS" },
	{ MBR_PTYPE_DRDOS_FAT12, "DRDOS/sec (FAT-12)" },
	{ MBR_PTYPE_HID_LNX, "Hidden Linux" },
	{ MBR_PTYPE_HID_LNX_SWAP, "Hidden Linux Swap" },
	{ MBR_PTYPE_DRDOS_FAT16S, "DRDOS/sec (FAT-16, < 32M)" },
	{ MBR_PTYPE_DRDOS_EXT, "DRDOS/sec (EXT)" },
	{ MBR_PTYPE_DRDOS_FAT16B, "DRDOS/sec (FAT-16, >= 32M)" },
	{ MBR_PTYPE_SYRINX, "Syrinx (Cyrnix?) or HPFS disabled" },
	{ MBR_PTYPE_DRDOS_8_xC8, "Reserved for DR-DOS 8.0+" },
	{ MBR_PTYPE_DRDOS_8_xC9, "Reserved for DR-DOS 8.0+" },
	{ MBR_PTYPE_DRDOS_8_xCA, "Reserved for DR-DOS 8.0+" },
	{ MBR_PTYPE_DRDOS_74_CHS, "DR-DOS 7.04+ Secured FAT32 CHS" },
	{ MBR_PTYPE_DRDOS_74_LBA, "DR-DOS 7.04+ Secured FAT32 LBA" },
	{ MBR_PTYPE_CTOS_MEMDUMP, "CTOS Memdump" },
	{ MBR_PTYPE_DRDOS_74_16X, "DR-DOS 7.04+ FAT16X LBA" },
	{ MBR_PTYPE_DRDOS_74_EXT, "DR-DOS 7.04+ EXT LBA" },
	{ MBR_PTYPE_REAL32, "REAL/32 secure big partition" },
	{ MBR_PTYPE_MDOS_FAT12, "Old Multiuser DOS FAT12" },
	{ MBR_PTYPE_MDOS_FAT16S, "Old Multiuser DOS FAT16 Small" },
	{ MBR_PTYPE_MDOS_EXT, "Old Multiuser DOS Extended" },
	{ MBR_PTYPE_MDOS_FAT16B, "Old Multiuser DOS FAT16 Big" },
	{ MBR_PTYPE_CPM_86, "CP/M 86" },
	{ MBR_PTYPE_CONCURRENT, "CP/M or Concurrent CP/M or Concurrent DOS "
				"or CTOS" },
	{ MBR_PTYPE_HID_CTOS_MEM, "Hidden CTOS Memdump" },
	{ MBR_PTYPE_DELL_UTIL, "Dell PowerEdge Server utilities" },
	{ MBR_PTYPE_DGUX_VIRTUAL, "DG/UX virtual disk manager" },
	{ MBR_PTYPE_STMICROELEC, "STMicroelectronics ST AVFS" },
	{ MBR_PTYPE_DOS_ACCESS, "DOS access or SpeedStor 12-bit FAT "
				"extended partition" },
	{ MBR_PTYPE_STORDIM, "DOS R/O or SpeedStor or Storage Dimensions" },
	{ MBR_PTYPE_SPEEDSTOR_16S, "SpeedStor 16-bit FAT extended partition "
				   "< 1024 cyl." },
	{ MBR_PTYPE_RESERVED_xE5, "reserved" },
	{ MBR_PTYPE_RESERVED_xE6, "reserved" },
	{ MBR_PTYPE_BEOS, "BeOS" },
	{ MBR_PTYPE_PMBR, "GPT Protective MBR" },
	{ MBR_PTYPE_EFI, "EFI system partition" },
	{ MBR_PTYPE_LNX_PA_RISC, "Linux PA-RISC boot loader" },
	{ MBR_PTYPE_SPEEDSTOR_X, "SpeedStor or Storage Dimensions" },
	{ MBR_PTYPE_DOS33_SEC, "DOS 3.3+ Secondary" },
	{ MBR_PTYPE_RESERVED_xF3, "reserved" },
	{ MBR_PTYPE_SPEEDSTOR_L, "SpeedStor large partition or "
				 "Storage Dimensions" },
	{ MBR_PTYPE_PROLOGUE, "Prologue multi-volumen partition" },
	{ MBR_PTYPE_RESERVED_xF6, "reserved" },
	{ MBR_PTYPE_PCACHE, "pCache: ext2/ext3 persistent cache" },
	{ MBR_PTYPE_BOCHS, "Bochs x86 emulator" },
	{ MBR_PTYPE_VMWARE, "VMware File System" },
	{ MBR_PTYPE_VMWARE_SWAP, "VMware Swap" },
	{ MBR_PTYPE_LNX_RAID, "Linux RAID partition persistent sb" },
	{ MBR_PTYPE_LANSTEP, "SpeedStor >1024 cyl. or LANstep "
			     "or IBM PS/2 IML" },
	{ MBR_PTYPE_XENIX_BAD, "Xenix Bad Block Table" },
};
#endif

#define	MBR_PSECT(s)		((s) & 0x3f)
#define	MBR_PCYL(c, s)		((c) + (((s) & 0xc0) << 2))

#define	MBR_IS_EXTENDED(x)	((x) == MBR_PTYPE_EXT || \
				 (x) == MBR_PTYPE_EXT_LBA || \
				 (x) == MBR_PTYPE_EXT_LNX)

		/* values for mbr_bootsel.mbrbs_flags */
#define	MBR_BS_ACTIVE	0x01	/* Bootselector active (or code present) */
#define	MBR_BS_EXTINT13	0x02	/* Set by fdisk if LBA needed (deprecated) */
#define	MBR_BS_READ_LBA	0x04	/* Force LBA reads (deprecated) */
#define	MBR_BS_EXTLBA	0x08	/* Extended ptn capable (LBA reads) */
#define	MBR_BS_ASCII	0x10	/* Bootselect code needs ascii key code */
/* This is always set, the bootsel is located using the magic number...  */
#define	MBR_BS_NEWMBR	0x80	/* New bootsel at offset 440 */

#if !defined(__ASSEMBLER__)					/* { */

/*
 * (x86) BIOS Parameter Block for FAT12
 */
struct mbr_bpbFAT12 {
	uint16_t	bpbBytesPerSec;	/* bytes per sector */
	uint8_t		bpbSecPerClust;	/* sectors per cluster */
	uint16_t	bpbResSectors;	/* number of reserved sectors */
	uint8_t		bpbFATs;	/* number of FATs */
	uint16_t	bpbRootDirEnts;	/* number of root directory entries */
	uint16_t	bpbSectors;	/* total number of sectors */
	uint8_t		bpbMedia;	/* media descriptor */
	uint16_t	bpbFATsecs;	/* number of sectors per FAT */
	uint16_t	bpbSecPerTrack;	/* sectors per track */
	uint16_t	bpbHeads;	/* number of heads */
	uint16_t	bpbHiddenSecs;	/* # of hidden sectors */
} __packed;

/*
 * (x86) BIOS Parameter Block for FAT16
 */
struct mbr_bpbFAT16 {
	uint16_t	bpbBytesPerSec;	/* bytes per sector */
	uint8_t		bpbSecPerClust;	/* sectors per cluster */
	uint16_t	bpbResSectors;	/* number of reserved sectors */
	uint8_t		bpbFATs;	/* number of FATs */
	uint16_t	bpbRootDirEnts;	/* number of root directory entries */
	uint16_t	bpbSectors;	/* total number of sectors */
	uint8_t		bpbMedia;	/* media descriptor */
	uint16_t	bpbFATsecs;	/* number of sectors per FAT */
	uint16_t	bpbSecPerTrack;	/* sectors per track */
	uint16_t	bpbHeads;	/* number of heads */
	uint32_t	bpbHiddenSecs;	/* # of hidden sectors */
	uint32_t	bpbHugeSectors;	/* # of sectors if bpbSectors == 0 */
	uint8_t		bsDrvNum;	/* Int 0x13 drive number (e.g. 0x80) */
	uint8_t		bsReserved1;	/* Reserved; set to 0 */
	uint8_t		bsBootSig;	/* 0x29 if next 3 fields are present */
	uint8_t		bsVolID[4];	/* Volume serial number */
	uint8_t		bsVolLab[11];	/* Volume label */
	uint8_t		bsFileSysType[8];
					/* "FAT12   ", "FAT16   ", "FAT     " */
} __packed;

/*
 * (x86) BIOS Parameter Block for FAT32
 */
struct mbr_bpbFAT32 {
	uint16_t	bpbBytesPerSec;	/* bytes per sector */
	uint8_t		bpbSecPerClust;	/* sectors per cluster */
	uint16_t	bpbResSectors;	/* number of reserved sectors */
	uint8_t		bpbFATs;	/* number of FATs */
	uint16_t	bpbRootDirEnts;	/* number of root directory entries */
	uint16_t	bpbSectors;	/* total number of sectors */
	uint8_t		bpbMedia;	/* media descriptor */
	uint16_t	bpbFATsecs;	/* number of sectors per FAT */
	uint16_t	bpbSecPerTrack;	/* sectors per track */
	uint16_t	bpbHeads;	/* number of heads */
	uint32_t	bpbHiddenSecs;	/* # of hidden sectors */
	uint32_t	bpbHugeSectors;	/* # of sectors if bpbSectors == 0 */
	uint32_t	bpbBigFATsecs;	/* like bpbFATsecs for FAT32 */
	uint16_t	bpbExtFlags;	/* extended flags: */
#define	MBR_FAT32_FATNUM	0x0F	/*   mask for numbering active FAT */
#define	MBR_FAT32_FATMIRROR	0x80	/*   FAT is mirrored (as previously) */
	uint16_t	bpbFSVers;	/* filesystem version */
#define	MBR_FAT32_FSVERS	0	/*   currently only 0 is understood */
	uint32_t	bpbRootClust;	/* start cluster for root directory */
	uint16_t	bpbFSInfo;	/* filesystem info structure sector */
	uint16_t	bpbBackup;	/* backup boot sector */
	uint8_t		bsReserved[12];	/* Reserved for future expansion */
	uint8_t		bsDrvNum;	/* Int 0x13 drive number (e.g. 0x80) */
	uint8_t		bsReserved1;	/* Reserved; set to 0 */
	uint8_t		bsBootSig;	/* 0x29 if next 3 fields are present */
	uint8_t		bsVolID[4];	/* Volume serial number */
	uint8_t		bsVolLab[11];	/* Volume label */
	uint8_t		bsFileSysType[8]; /* "FAT32   " */
} __packed;

/*
 * (x86) MBR boot selector
 */
struct mbr_bootsel {
	uint8_t		mbrbs_defkey;
	uint8_t		mbrbs_flags;
	uint16_t	mbrbs_timeo;
	char		mbrbs_nametab[MBR_PART_COUNT][MBR_BS_PARTNAMESIZE + 1];
} __packed;

/*
 * MBR partition
 */
struct mbr_partition {
	uint8_t		mbrp_flag;	/* MBR partition flags */
	uint8_t		mbrp_shd;	/* Starting head */
	uint8_t		mbrp_ssect;	/* Starting sector */
	uint8_t		mbrp_scyl;	/* Starting cylinder */
	uint8_t		mbrp_type;	/* Partition type (see below) */
	uint8_t		mbrp_ehd;	/* End head */
	uint8_t		mbrp_esect;	/* End sector */
	uint8_t		mbrp_ecyl;	/* End cylinder */
	uint32_t	mbrp_start;	/* Absolute starting sector number */
	uint32_t	mbrp_size;	/* Partition size in sectors */
} __packed;

int xlat_mbr_fstype(int);	/* in sys/lib/libkern/xlat_mbr_fstype.c */

/*
 * MBR boot sector.
 * This is used by both the MBR (Master Boot Record) in sector 0 of the disk
 * and the PBR (Partition Boot Record) in sector 0 of an MBR partition.
 */
struct mbr_sector {
					/* Jump instruction to boot code.  */
					/* Usually 0xE9nnnn or 0xEBnn90 */
	uint8_t			mbr_jmpboot[3];
					/* OEM name and version */
	uint8_t			mbr_oemname[8];
	union {				/* BIOS Parameter Block */
		struct mbr_bpbFAT12	bpb12;
		struct mbr_bpbFAT16	bpb16;
		struct mbr_bpbFAT32	bpb32;
	} mbr_bpb;
					/* Boot code */
	uint8_t			mbr_bootcode[310];
					/* Config for /usr/mdec/mbr_bootsel */
	struct mbr_bootsel	mbr_bootsel;
					/* NT Drive Serial Number */
	uint32_t		mbr_dsn;
					/* mbr_bootsel magic */
	uint16_t		mbr_bootsel_magic;
					/* MBR partition table */
	struct mbr_partition	mbr_parts[MBR_PART_COUNT];
					/* MBR magic (0xaa55) */
	uint16_t		mbr_magic;
} __packed;

#endif	/* !defined(__ASSEMBLER__) */				/* } */


/* ------------------------------------------
 * shared --
 *	definitions shared by many platforms
 */

#if !defined(__ASSEMBLER__)					/* { */

	/* Maximum # of blocks in bbi_block_table, each bbi_block_size long */
#define	SHARED_BBINFO_MAXBLOCKS	118	/* so sizeof(shared_bbinfo) == 512 */

struct shared_bbinfo {
	uint8_t bbi_magic[32];
	int32_t bbi_block_size;
	int32_t bbi_block_count;
	int32_t bbi_block_table[SHARED_BBINFO_MAXBLOCKS];
};

/* ------------------------------------------
 * x86
 *
 */

/*
 * Parameters for NetBSD /boot written to start of pbr code by installboot
 */

struct x86_boot_params {
	uint32_t	bp_length;	/* length of patchable data */
	uint32_t	bp_flags;
	uint32_t	bp_timeout;	/* boot timeout in seconds */
	uint32_t	bp_consdev;
	uint32_t	bp_conspeed;
	uint8_t		bp_password[16];	/* md5 hash of password */
	char		bp_keymap[64];	/* keyboard translation map */
	uint32_t	bp_consaddr;	/* ioaddr for console */
};

#endif	/* !defined(__ASSEMBLER__) */				/* } */

#define	X86_BOOT_MAGIC(n)	('x' << 24 | 0x86b << 12 | 'm' << 4 | (n))
#define	X86_BOOT_MAGIC_1	X86_BOOT_MAGIC(1)	/* pbr.S */
#define	X86_BOOT_MAGIC_2	X86_BOOT_MAGIC(2)	/* bootxx.S */
#define	X86_BOOT_MAGIC_PXE	X86_BOOT_MAGIC(3)	/* start_pxe.S */
#define	X86_BOOT_MAGIC_FAT	X86_BOOT_MAGIC(4)	/* fatboot.S */
#define	X86_MBR_GPT_MAGIC	0xedb88320		/* gpt.S */

		/* values for bp_flags */
#define	X86_BP_FLAGS_RESET_VIDEO	1
#define	X86_BP_FLAGS_PASSWORD		2
#define	X86_BP_FLAGS_NOMODULES		4
#define	X86_BP_FLAGS_NOBOOTCONF		8
#define	X86_BP_FLAGS_LBA64VALID		0x10

		/* values for bp_consdev */
#define	X86_BP_CONSDEV_PC	0
#define	X86_BP_CONSDEV_COM0	1
#define	X86_BP_CONSDEV_COM1	2
#define	X86_BP_CONSDEV_COM2	3
#define	X86_BP_CONSDEV_COM3	4
#define	X86_BP_CONSDEV_COM0KBD	5
#define	X86_BP_CONSDEV_COM1KBD	6
#define	X86_BP_CONSDEV_COM2KBD	7
#define	X86_BP_CONSDEV_COM3KBD	8

#endif	/* !_SYS_BOOTBLOCK_H */
