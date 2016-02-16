#	$NetBSD: Makefile,v 1.10 2014/04/04 18:17:36 njoly Exp $

.include "../Makefile.inc"

.PATH:	${S}/compat/common

KMOD=	compat

#
# We don't want to hook these in for the native emulation, but do want
# the symbols available for other emulations.  So include the source
# files but don't hook them in via compat_modcmd().
#
CPPFLAGS+=	-DCOMPAT_70 -DCOMPAT_80
CPPFLAGS+=	-DSYSVSEM -DSYSVSHM -DSYSVMSG

SRCS=	compat_mod.c

.PATH:	${S}/arch/${MACHINE}/${MACHINE}
.PATH:	${S}/arch/${MACHINE_ARCH}/${MACHINE_ARCH}
.PATH:	${S}/arch/${MACHINE_CPU}/${MACHINE_CPU}

.include <bsd.kmodule.mk>
