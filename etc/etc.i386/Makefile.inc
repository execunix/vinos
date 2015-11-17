#	$NetBSD: Makefile.inc,v 1.67.12.3 2015/03/18 03:57:15 snj Exp $
#
#	etc.i386/Makefile.inc -- i386-specific etc Makefile targets
#

# If you change the list of distributed kernels, don't forget
# to update the release documentation in distrib/notes/common/contents

KERNEL_SETS=	GENERIC
KERNEL_SETS+=	MONOLITHIC
KERNEL_SETS+=	LEGACY
# KERNEL_SETS+=	GENERIC_TINY
# KERNEL_SETS+=	GENERIC_PS2TINY

BUILD_KERNELS+=	INSTALL
# BUILD_KERNELS+=	INSTALL_SMALL
# BUILD_KERNELS+=	INSTALL_TINY
# BUILD_KERNELS+=	INSTALL_PS2

INSTALLATION_DIRS+=	installation/misc
INSTALLATION_DIRS+=	installation/cdrom
INSTALLATION_DIRS+=	installation/miniroot

# i386 specific distrib stuff
snap_md_post:
	cd ${KERNSRCDIR}/arch/i386/stand/dosboot && ${MAKE} release
	cd ${KERNSRCDIR}/arch/i386/stand/pxeboot && ${MAKE} release
	cd ${KERNSRCDIR}/arch/i386/stand/misc && ${MAKE} release
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/cdrom '*.iso'
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/miniroot '*.*'
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/misc '*.*'
