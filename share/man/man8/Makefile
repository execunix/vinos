#	$NetBSD: Makefile,v 1.104 2014/02/24 07:23:41 skrll Exp $
#	from: @(#)Makefile	8.1 (Berkeley) 6/5/93

MAN=	MAKEDEV.8 MAKEDEV.local.8 afterboot.8 boot.8 \
	diskless.8 hpcboot.8 \
	intro.8 nis.8 pam.8 rc.8 rc.subr.8 rescue.8 \
	sysinst.8 veriexec.8 \
	wizd.8
MLINKS+=MAKEDEV.8 makedev.8
MLINKS+=MAKEDEV.local.8 makedev.local.8
MLINKS+=nis.8 yp.8
MLINKS+=rc.8 rc.d.8
MLINKS+=rc.8 rc.local.8
MLINKS+=rc.8 rc.shutdown.8

SUBDIR= man8.evbarm \
	man8.x86

# create MAKEDEV.8 from ../../etc/MAKEDEV.tmpl
makedevs:
	cd ${.CURDIR} && ${HOST_SH} MAKEDEV2manpage.sh

.include <bsd.man.mk>
.include <bsd.subdir.mk>
