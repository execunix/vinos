#	$NetBSD: Makefile,v 1.12 2013/05/02 03:56:41 matt Exp $

.include <bsd.endian.mk>

SUBDIR= 

.if ${MACHINE_CPU} == "arm" && ${TARGET_ENDIANNESS} == "1234"
# Little endian platforms
SUBDIR+= SMDK2410_flash_0x00000000
.endif

.include <bsd.subdir.mk>
