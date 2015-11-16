#	$NetBSD: Makefile,v 1.4 2011/01/18 00:22:56 jym Exp $

.include <bsd.own.mk>
.include "${NETBSDSRCDIR}/distrib/common/Makefile.distrib"

#	create ${RAMDISK_*} variables
#
RAMDISKS=	RAMDISK		ramdisk

.for V F in ${RAMDISKS}
${V}DIR!=	cd ${.CURDIR}/../ramdisks/${F} && ${PRINTOBJDIR}
${V}=		${${V}DIR}/${F}.fs
.endfor

MDSETTARGETS=		INSTALL		${RAMDISK}	-
MDSETTARGETS+=		GENERIC		${RAMDISK}	-

MDSET_RELEASEDIR=	binary/kernel

# need symbols to load modules. don't actually want image inserted.
MDSET_NOSTRIP.netbsd-GENERIC=
MDSET_NOIMAGE.netbsd-GENERIC=

.include "${DISTRIBDIR}/common/Makefile.mdset"

.include <bsd.prog.mk>
