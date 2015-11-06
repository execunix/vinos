#	$NetBSD: Makefile,v 1.63.10.2 2015/03/09 08:47:19 snj Exp $

# xf86-input drivers

SUBDIR= \
	xf86-input-keyboard \
	xf86-input-mouse \
	xf86-input-ws

.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+= \
	xf86-input-vmmouse

.endif

# xf86-video drivers

.include <bsd.own.mk>

# Set this to "no" to build the UMS-happy 6.x driver, vs KMS 7.x
.if ${MKX11RADEONKMS} != "no"
RADEON_SUBDIR=radeon-kms
.else
RADEON_SUBDIR=radeon
.endif

.if ${MACHINE_ARCH} == "i386" || \
    ${MACHINE_ARCH} == "x86_64"
SUBDIR+= \
	xf86-video-apm \
	xf86-video-ark \
	xf86-video-ast \
	xf86-video-ati \
	xf86-video-chips \
	xf86-video-cirrus \
	xf86-video-cirrus_alpine \
	xf86-video-cirrus_laguna \
	xf86-video-glint \
	xf86-video-i128 \
	xf86-video-i740 \

# needs porting for xorg-server 1.6
#	xf86-video-imstt \

SUBDIR+= \
	xf86-video-intel \
	xf86-video-mach64 \
	xf86-video-mga \
	xf86-video-neomagic \
	xf86-video-nv \
	xf86-video-openchrome \
	xf86-video-r128 \
	xf86-video-${RADEON_SUBDIR} \
	xf86-video-radeonhd \
	xf86-video-vmware \
	xf86-video-vesa \
	xf86-video-wsfb \

.endif	# ${MACHINE_ARCH} == "i386" || ${MACHINE_ARCH} == "x86_64"

.if ${MACHINE} == "evbarm"
SUBDIR+= \
	xf86-video-wsfb
.endif # ${MACHINE} == "evbarm"

# broken with xorg-server 1.6
# x86
#	xf86-video-nsc \
#	xf86-video-vga \
# alpha
#	xf86-video-vga \
# bebox
#	xf86-video-vga
# prep
#	xf86-video-vga

.include <bsd.subdir.mk>
