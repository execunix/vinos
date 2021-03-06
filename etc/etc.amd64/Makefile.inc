#	$NetBSD: Makefile.inc,v 1.14.12.3 2015/03/18 03:57:15 snj Exp $
#
#	etc.amd64/Makefile.inc -- amd64-specific etc Makefile targets
#

# If you change the list of distributed kernels, don't forget
# to update the release documentation in distrib/notes/common/contents

KERNEL_SETS=		GENERIC

BUILD_KERNELS=		INSTALL

INSTALLATION_DIRS+= 	installation/cdrom
INSTALLATION_DIRS+= 	installation/misc
INSTALLATION_DIRS+= 	installation/miniroot

snap_md_post:
	cd ${KERNSRCDIR}/arch/i386/stand/pxeboot && ${MAKE} release
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/cdrom '*.iso'
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/miniroot '*.*'
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/misc '*.*'
