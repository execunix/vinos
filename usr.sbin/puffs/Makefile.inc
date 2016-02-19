#	$NetBSD: Makefile.inc,v 1.14 2014/01/16 01:15:34 christos Exp $

WARNS?=	4

.include <bsd.own.mk>

USE_FORT?= yes	# network client

.if exists(${.CURDIR}/../../Makefile.inc)
.include "${.CURDIR}/../../Makefile.inc"
.endif

