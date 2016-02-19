#	$NetBSD: Makefile,v 1.16 2010/12/15 18:37:55 pooka Exp $

PROG=	raidctl 
SRCS=	rf_configure.c raidctl.c
SRCS+=	prog_ops.c
MAN=	raidctl.8

DPADD=  ${LIBUTIL}
LDADD=  -lutil

.include <bsd.prog.mk>
