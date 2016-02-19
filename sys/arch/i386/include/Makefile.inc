# $NetBSD: Makefile.inc,v 1.2.2.1 2014/10/19 16:11:06 martin Exp $

CFLAGS+=	-msoft-float
CFLAGS+=	-mno-mmx -mno-sse -mno-avx
CFLAGS+=	-fno-omit-frame-pointer

USE_SSP?=	yes
