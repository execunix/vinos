#	$NetBSD: Makefile,v 1.17 2013/08/11 05:56:55 dholland Exp $

.include <bsd.own.mk>

PROG=	modload
SRCS=	main.c
SRCS+=	${PROG}_hostops.c
MAN=	modload.8
LDADD=	-lprop

.include <bsd.prog.mk>
