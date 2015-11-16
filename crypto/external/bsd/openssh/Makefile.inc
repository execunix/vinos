#	$NetBSD: Makefile.inc,v 1.5.22.2 2015/04/30 06:07:29 riz Exp $

WARNS?=	4

.include <bsd.own.mk>

USE_FORT?= yes	# network client/server
WITH_OPENSSL=1

SSHDIST?= ${NETBSDSRCDIR}/crypto/external/bsd/openssh/dist

CPPFLAGS+=-I${SSHDIST}

CPPFLAGS+=-DHAVE_DLOPEN
CPPFLAGS+=-DHAVE_HEADER_AD
CPPFLAGS+=-DHAVE_LOGIN_CAP
CPPFLAGS+=-DHAVE_STDLIB_H

CPPFLAGS+=-DWITH_SSH1 -DWITH_OPENSSL -DENABLE_PKCS11 -D_OPENBSD_SOURCE
.if !defined(NOPIC)
CPPFLAGS+=-DHAVE_DLOPEN
.endif
.PATH: ${SSHDIST}

.if (${USE_PAM} != "no")
CPPFLAGS+=-DUSE_PAM
.else	# USE_PAM == no
.if (${USE_SKEY} != "no")
CPPFLAGS+=-DSKEY
.endif
.endif	# USE_PAM == no

.if (${USE_KERBEROS} != "no")
CPPFLAGS+=-DGSSAPI
CPPFLAGS+=-DKRB5 -DHEIMDAL
.endif

.if (${USE_LDAP} != "no")
CPPFLAGS+=-DWITH_LDAP_PUBKEY
.endif

.if ${X11FLAVOUR} == "Xorg"
CPPFLAGS+=-DX11BASE=\"/usr/X11R7\"
.endif

CPPFLAGS+=-DSUPPORT_UTMP -DSUPPORT_UTMPX
CPPFLAGS+=-DLIBWRAP