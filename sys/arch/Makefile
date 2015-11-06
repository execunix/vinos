#	$NetBSD: Makefile,v 1.44 2014/08/10 05:57:31 matt Exp $

# For now, we install the machine and arch includes, and symlink 'machine'
# to the location of the machine includes (usually).
#
# Eventually, we should install everything.

.include <bsd.own.mk>

ARCHSUBDIR= ${MACHINE_CPU}

.if ${MACHINE_CPU} == "aarch64"
SUBDIR= evbarm64
.elif ${MACHINE_CPU} == "arm"
SUBDIR= evbarm
.else
SUBDIR=	${MACHINE}
.endif

.if ${MACHINE} != ${ARCHSUBDIR}
.if exists(${ARCHSUBDIR})
SUBDIR+= ${ARCHSUBDIR}
.endif
.endif
.if ${MACHINE_CPU} == "aarch64"
SUBDIR+= arm
.endif
.if defined(XEN_BUILD)
SUBDIR+= xen
.endif

#SUBDIR=arm \
#	evbarm \
#	i386 \
#	x86_64 xen

.if ${MACHINE_CPU} == aarch64 || ${MACHINE_CPU} == "arm"
INCSYMLINKS= ${MACHINE_CPU} /usr/include/machine
.else
INCSYMLINKS= ${MACHINE} /usr/include/machine
.endif

INCSYMLINKS+= machine/float.h /usr/include/float.h

.include <bsd.kinc.mk>
