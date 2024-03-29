#	$NetBSD: Makefile,v 1.140.2.2 2015/03/21 17:11:35 snj Exp $

.include <bsd.own.mk>

# For all platforms

SUBDIR=		accf_dataready
SUBDIR+=	accf_httpready
SUBDIR+=	aio
SUBDIR+=	blowfish
SUBDIR+=	bpf
SUBDIR+=	camellia
SUBDIR+=	cast128
SUBDIR+=	cd9660
SUBDIR+=	chfs
SUBDIR+=	crypto
SUBDIR+=	compat
SUBDIR+=	coredump
SUBDIR+=	dbcool
SUBDIR+=	des
SUBDIR+=	ext2fs
SUBDIR+=	exec_script
SUBDIR+=	fdesc
SUBDIR+=	ffs
SUBDIR+=	filemon
SUBDIR+=	flash
SUBDIR+=	fss
SUBDIR+=	gpio
SUBDIR+=	gpioiic
SUBDIR+=	gpioow
SUBDIR+=	gpiosim
SUBDIR+=	hythygtemp
SUBDIR+=	i2cexec
SUBDIR+=	if_axe
SUBDIR+=	if_axen
SUBDIR+=	if_npflog
SUBDIR+=	if_smsc
SUBDIR+=	iic
SUBDIR+=	kernfs
SUBDIR+=	layerfs
SUBDIR+=	lua
SUBDIR+=	luasystm
SUBDIR+=	luapmf
SUBDIR+=	mfs
SUBDIR+=	miiverbose
SUBDIR+=	miniroot
SUBDIR+=	mqueue
SUBDIR+=	msdos
SUBDIR+=	nand
SUBDIR+=	nandemulator
SUBDIR+=	nfs
SUBDIR+=	nfsserver
SUBDIR+=	npf
SUBDIR+=	npf_alg_icmp
SUBDIR+=	npf_ext_log
SUBDIR+=	npf_ext_normalize
SUBDIR+=	npf_ext_rndblock
SUBDIR+=	ntfs
SUBDIR+=	null
SUBDIR+=	onewire
SUBDIR+=	opencrypto
SUBDIR+=	overlay
SUBDIR+=	pciverbose
SUBDIR+=	pf
SUBDIR+=	ppp_bsdcomp
SUBDIR+=	ppp_deflate
SUBDIR+=	procfs
SUBDIR+=	ptyfs
SUBDIR+=	puffs
SUBDIR+=	putter
SUBDIR+=	scsiverbose
SUBDIR+=	sdtemp
SUBDIR+=	secmodel_bsd44
SUBDIR+=	secmodel_extensions
SUBDIR+=	secmodel_overlay
SUBDIR+=	securelevel
SUBDIR+=	skipjack
SUBDIR+=	smbfs
SUBDIR+=	spdmem
SUBDIR+=	suser
SUBDIR+=	swcrypto
SUBDIR+=	swsensor
SUBDIR+=	tmpfs
SUBDIR+=	uatp
SUBDIR+=	udf
SUBDIR+=	umap
SUBDIR+=	union
SUBDIR+=	usbverbose
SUBDIR+=	zlib
SUBDIR+=	tprof
.if (defined(NOTYET))
SUBDIR+=	hifn		# Builds on architectures with PCI bus
SUBDIR+=	unionfs
.endif
.if ${MKBINUTILS} != "no"
SUBDIR+=	xldscripts
.endif

# Machine dependent section
.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_CPU} == "m68k"
SUBDIR+=	exec_aout
.endif

.if ${MACHINE_ARCH} != "alpha" && ${MACHINE_CPU} != "aarch64"
SUBDIR+=	exec_elf32
.endif

.if ${MACHINE_CPU} == "aarch64" || \
    ${MACHINE_ARCH} == "alpha" || \
    ${MACHINE_ARCH} == "sparc64" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+=	exec_elf64
.endif

.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+=	amdtemp
SUBDIR+=	coretemp
SUBDIR+=	est
SUBDIR+=	hdafg
SUBDIR+=	hdaudio
SUBDIR+=	hpet
SUBDIR+=	odcm
SUBDIR+=	powernow
SUBDIR+=	tprof_pmi
SUBDIR+=	tprof_amdpmi
SUBDIR+=	vmt
.endif

.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+=	ubsec		# Builds on architectures with PCI bus
.endif

#
# ACPI modules
#
.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "ia64" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+=	acpiverbose
.endif

.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+=	acpiacad
SUBDIR+=	acpibat
SUBDIR+=	acpibut
SUBDIR+=	acpicpu
SUBDIR+=	acpidalb
SUBDIR+=	acpifan
SUBDIR+=	acpilid
SUBDIR+=	acpipmtr
SUBDIR+=	acpitz
SUBDIR+=	acpivga
SUBDIR+=	acpiwdrt
SUBDIR+=	acpiwmi
SUBDIR+=	aibs
SUBDIR+=	asus
SUBDIR+=	fujbp
SUBDIR+=	fujhk
SUBDIR+=	hpacel
SUBDIR+=	hpqlb
SUBDIR+=	thinkpad
SUBDIR+=	wmidell
SUBDIR+=	wmieeepc
SUBDIR+=	wmihp
SUBDIR+=	wmimsi
.endif

.if ${MACHINE_ARCH} == "x86_64"
SUBDIR+=	compat_netbsd32
.endif

.if ${MACHINE_ARCH} == "i386"
SUBDIR+=	ati_pcigart
SUBDIR+=	mach64drm
SUBDIR+=	mgadrm
SUBDIR+=	nsclpcsio
SUBDIR+=	padlock
SUBDIR+=	r128drm
SUBDIR+=	radeondrm
SUBDIR+=	savagedrm
SUBDIR+=	sisdrm
SUBDIR+=	tdfxdrm
SUBDIR+=	viac7temp
SUBDIR+=	viadrm
.endif

.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+=	azalia
SUBDIR+=	drm
SUBDIR+=	drmkms
SUBDIR+=	drmkms_linux
SUBDIR+=	drmkms_pci
SUBDIR+=	i915drm
SUBDIR+=	i915drmkms
SUBDIR+=	pad
#
# ISA modules
#
SUBDIR+=	aps
SUBDIR+=	finsio
SUBDIR+=	itesio
SUBDIR+=	lm
SUBDIR+=	lm_isa
SUBDIR+=	lm_isa_common
SUBDIR+=	lm_wbsio
SUBDIR+=	wbsio
.endif

.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64" || \
    ${MACHINE_ARCH} == "sparc64"
SUBDIR+=	pwdog
.endif

.include <bsd.own.mk>

#
# NetBSD/usermode support
#
.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64"
#SUBDIR+=	../arch/usermode/modules/syscallemu
.endif

.if (${MKCOMPATMODULES} != "no")
SUBDIR+=	arch
.endif

#
# DTV modules
#
SUBDIR+=	dtv
SUBDIR+=	dtv_math
.if ${MACHINE_ARCH} == "i386" || ${MACHINE_ARCH} == "x86_64"
# tuners/demods
SUBDIR+=	au8522
SUBDIR+=	cx24227
SUBDIR+=	lg3303
SUBDIR+=	mt2131
SUBDIR+=	nxt2k
SUBDIR+=	tvpll
SUBDIR+=	xc3028
SUBDIR+=	xc5k
SUBDIR+=	zl10353
# hardware drivers
SUBDIR+=	auvitek
SUBDIR+=	coram
SUBDIR+=	cxdtv
SUBDIR+=	emdtv
.endif

.include <bsd.subdir.mk>
