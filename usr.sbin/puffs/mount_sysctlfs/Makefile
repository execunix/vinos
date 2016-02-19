#	$NetBSD: Makefile,v 1.4 2010/08/06 15:04:13 pooka Exp $

PROG=	mount_sysctlfs
SRCS=	sysctlfs.c

DPAPP+=	${LIBPUFFS} ${LIBUTIL}
LDADD+=	-lpuffs -lutil

MAN=	mount_sysctlfs.8

.include <bsd.prog.mk>
