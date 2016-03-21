#	$NetBSD: Makefile,v 1.5 2014/03/01 19:45:29 tsutsui Exp $

.include "../Makefile.common"

SUBDIR= xnest vfb
.if ${XSERVER_XFREE86} != "no"
SUBDIR+= xfree86
.endif

.include <bsd.subdir.mk>
