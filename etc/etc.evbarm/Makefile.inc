#	$NetBSD: Makefile.inc,v 1.63.2.5 2015/08/06 21:13:44 snj Exp $
#
#	etc.evbarm/Makefile.inc -- evbarm-specific etc Makefile targets
#

MKIMAGE= ${NETBSDSRCDIR}/distrib/utils/embedded/mkimage

# If you change the list of distributed kernels, don't forget
# to update the release documentation in distrib/notes/common/contents
EVBARM_BOARDS=
.for i in armv4 armv5t armv6 armv7
EVBARM_BOARDS.${i}=
.endfor

.if !empty(MACHINE_ARCH:M*eb)
# big endian boards
#KERNEL_SETS.armv4+=		IXM1200
.else
# little endian boards
#KERNEL_SETS.armv4+=		IXM1200
#EVBARM_BOARDS.armv4+=		MINI2440
EVBARM_BOARDS.armv4+=		SMDK2410
EVBARM_BOARDS.armv6+=		RPI
EVBARM_BOARDS.armv6hf+=	 	RPI
KERNEL_SETS.armv6+=		RPI2
KERNEL_SETS.armv6hf+=	 	RPI2
EVBARM_BOARDS.armv7+=		RPI2
EVBARM_BOARDS.armv7hf+=	 	RPI2
.endif

IMAGE.rel=	${RELEASEDIR}/${RELEASEMACHINEDIR}
IMAGE.dir=	${IMAGE.rel}/binary/gzimg
IMAGE.kern=	${IMAGE.rel}/binary/kernel
IMAGE.instk=	${IMAGE.rel}/installation/instkernel

__mkimage: .USE
	TOOL_MAKEFS=${TOOL_MAKEFS} \
	TOOL_FDISK=${TOOL_FDISK} \
	TOOL_GZIP=${TOOL_GZIP} \
	TOOL_MKNOD=${TOOL_MKNOD} \
	TOOL_PAX=${TOOL_PAX} \
	TOOL_MKUBOOTIMAGE=${TOOL_MKUBOOTIMAGE} \
	TOOL_MTREE=${TOOL_MTREE} \
	HOST_SH=${HOST_SH} \
	${HOST_SH} ${MKIMAGE} -x -h ${.TARGET:S/smp_//} -D ${DESTDIR} \
	    -S ${NETBSDSRCDIR} ${MKI_OPTS.${.TARGET}} \
	    ${IMAGE.dir}/${.TARGET:S/smp_//}.img.gz

ARCHES.arm=		armv4 armv5t armv6 armv7
ARCHES.armeb=		armv4 armv5t armv6
ARCHES.earm=		armv5t armv6 armv7
ARCHES.earmeb=		armv5t armv6
ARCHES.earmhf=		armv6hf armv7hf
ARCHES.earmhfeb=	armv6hf
ARCHES.earmv4=		armv4
ARCHES.earmv4eb=	armv4
ARCHES.earmv5=		armv5t
ARCHES.earmv5eb=	armv5t
ARCHES.earmv6=		armv6
ARCHES.earmv6hf=	armv6hf
ARCHES.earmv6eb=	armv6
ARCHES.earmv6hfeb=	armv6hf
ARCHES.earmv7=		armv7
ARCHES.earmv7hf=	armv7hf
ARCHES.earmv7eb=	armv7
ARCHES.earmv7hfeb=	armv7hf
.for arch in ${ARCHES.${MACHINE_ARCH}}
.for board in ${EVBARM_BOARDS.${arch}}
KERNEL_SETS+=		${board}
BUILD_KERNELS+=		${board}_INSTALL
.endfor
KERNEL_SETS+=		${KERNEL_SETS.${arch}}
.endfor

.if !empty(MACHINE_ARCH:M*armv7*) && empty(ALL_KERNELS)
smp_armv7: __mkimage
MKI_OPTS.smp_armv7=	-K ${IMAGE.kern}
SNAP_MD_POST_DEPS+=	smp_armv7
.endif

.if !empty(KERNEL_SETS:MRPI)
smp_rpi: __mkimage
.if empty(ALL_KERNELS) || !empty(ALL_KERNELS:MRPI)
MKI_OPTS.smp_rpi=	-K ${IMAGE.kern}/netbsd-RPI.bin.gz
SNAP_MD_POST_DEPS+=	smp_rpi
.endif
smp_rpi_inst: ${IMAGE.instk}/netbsd-RPI_INSTALL.bin.gz __mkimage
.if empty(ALL_KERNELS) || !empty(ALL_KERNELS:MRPI_INSTALL)
MKI_OPTS.smp_rpi_inst=	-b -K ${IMAGE.instk}/netbsd-RPI_INSTALL.bin.gz
SNAP_MD_POST_DEPS+=	smp_rpi_inst
.endif
.endif

snap_md_post: check_DESTDIR check_RELEASEDIR snap_post .WAIT ${SNAP_MD_POST_DEPS}

KERNEL_SUFFIXES=	bin srec ub

INSTALLATION_DIRS+=	binary/gzimg		\
			installation/instkernel
