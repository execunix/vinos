#	$NetBSD: archdirs.mk,v 1.6.2.1 2015/06/01 19:38:35 snj Exp $

# list of subdirs used per-platform

.if ${MACHINE} == "amd64"
ARCHDIR_SUBDIR=	amd64/i386
.endif

.if !empty(MACHINE_ARCH:Mearm*)
ARCHDIR_SUBDIR=	arm/oabi
.endif

.if (${MACHINE_ARCH} == "aarch64")
ARCHDIR_SUBDIR+= arm/eabi
ARCHDIR_SUBDIR+= arm/eabihf
ARCHDIR_SUBDIR+= arm/oabi
.elif (${MACHINE_ARCH} == "aarch64eb")
ARCHDIR_SUBDIR= arm/eabi
.endif
