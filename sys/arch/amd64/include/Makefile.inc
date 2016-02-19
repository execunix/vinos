# $NetBSD: Makefile.inc,v 1.4.14.1 2014/10/19 16:11:06 martin Exp $

CFLAGS+=	-mno-red-zone -mno-mmx -mno-sse -mno-avx
CFLAGS+=	-msoft-float
CFLAGS+=	-mcmodel=kernel -fno-omit-frame-pointer

USE_SSP?=	yes
