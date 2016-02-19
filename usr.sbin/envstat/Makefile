# $NetBSD: Makefile,v 1.10 2010/12/14 08:04:14 pooka Exp $

PROG=		envstat
SRCS+=		envstat.c config.c config_yacc.y config_lex.l
SRCS+=		prog_ops.c

LDADD=		-lprop
DPADD=		${LIBPROP}

MAN=		envstat.8 envsys.conf.5

CPPFLAGS+=      -I${.CURDIR} -I.

YHEADER=	yes

.include <bsd.prog.mk>
