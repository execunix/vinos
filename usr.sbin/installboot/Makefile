#	$NetBSD: Makefile,v 1.48.4.1 2015/07/16 21:36:59 snj Exp $
#

.include <bsd.own.mk>

PROG=	installboot
MAN=	installboot.8
SRCS=	installboot.c sum.c machines.c fstypes.c

ARCH_XLAT= amd64-i386.c

.if !defined(SMALLPROG) && !defined(ARCH_FILES)
ARCH_FILES=  i386.c
.else
ARCH_FILES?= ${ARCH_XLAT:M${MACHINE}-*:S/${MACHINE}-//}
.if empty(ARCH_FILES)
ARCH_FILES= ${MACHINE}.c
.endif
.endif

SRCS+=${ARCH_FILES}

.if empty(ARCH_FILES:C/(macppc|news|sparc|sun68k|x68k)/stg2/:Mstg2.c)
CPPFLAGS	+= -DNO_STAGE2
.else
SRCS+= bbinfo.c

# fstypes are only needed for 'stage2' and then only from bbinfo.
SRCS+= ffs.c
.if SMALLPROG
CPPFLAGS+=	-DNO_FFS_SWAP
.else
SRCS+= ffs_bswap.c
.endif
#SRCS+= ext2fs.c ext2fs_bswap.c
.endif

UFSSRC=		${NETBSDSRCDIR}/sys/ufs
CPPFLAGS+=	-I${.CURDIR} -I.
.PATH:		${.CURDIR}/arch ${UFSSRC}/ffs ${UFSSRC}/ext2fs

.if !defined(HOSTPROGNAME)
.for f in i386 macppc
COPTS.${f}.c+=  -Wno-pointer-sign
.endfor

LDADD+= -lutil
DPADD+= ${LIBUTIL}
.endif

.include <bsd.prog.mk>
