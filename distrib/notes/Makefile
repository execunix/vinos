#	$NetBSD: Makefile,v 1.44 2014/02/24 07:23:39 skrll Exp $
#

.include <bsd.own.mk>

TARGETS+=	release

.ifndef ALLNOTES
.if exists(${RELEASEMACHINE}/Makefile)
SUBDIR=	${RELEASEMACHINE}
.endif
.else
SUBDIR+= amd64
SUBDIR+= evbarm
SUBDIR+= i386
SUBDIR+= xen
.endif

.include <bsd.subdir.mk>
