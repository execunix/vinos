#	$NetBSD: Makefile,v 1.39.8.1 2015/01/08 11:01:01 martin Exp $
#	from: @(#)Makefile	8.1 (Berkeley) 6/12/93

.include <bsd.own.mk>

USE_FORT?= yes	# setgid

PROG=	netstat
SRCS=	atalk.c bpf.c fast_ipsec.c if.c inet.c inet6.c \
	main.c mbuf.c mroute.c mroute6.c pfkey.c pfsync.c route.c \
	unix.c vtw.c rtutil.c
SRCS+=	${PROG}_hostops.c
BINGRP=	kmem
BINMODE=2555
LDADD=	-lkvm
DPADD=	${LIBKVM}
CPPFLAGS+= -DIPSEC -I${.CURDIR}
CPPFLAGS+= -I${NETBSDSRCDIR}/sys/dist/pf
CPPFLAGS+= -I${NETBSDSRCDIR}/sbin/route

CWARNFLAGS.clang+=	-Wno-format
COPTS.show.c += -Wno-format-nonliteral

.PATH:  ${.CURDIR}/../../sbin/route

.if (${USE_INET6} != "no")
CPPFLAGS+= -DINET6
.endif

.include <bsd.prog.mk>
