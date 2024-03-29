#	$NetBSD: Makefile,v 1.215.2.2 2015/04/30 06:07:34 riz Exp $
#	from: @(#)Makefile	5.25.1.1 (Berkeley) 5/7/91

.include <bsd.own.mk>

SUBDIR=		csu .WAIT

.if (${MKGCC} != "no")
SUBDIR+=	../external/gpl3/${EXTERNAL_GCC_SUBDIR}/lib/libgcc .WAIT
.endif

SUBDIR+=	libc
SUBDIR+=	.WAIT

#
# The SUBDIRs above are included here for completeness but should be built
# and installed prior to make(dependall) in this file, as libraries listed
# below will depend on versions from DESTDIR only.
#

SUBDIR+=	i18n_module

SUBDIR+=	libarch \
		libbluetooth libbsdmalloc libbz2 \
		libcompat libcrypt \
		libintl libipsec libkvm libm \
		libossaudio libpci libpmc libposix libprop libpthread \
		libpthread_dbg libpuffs libresolv librmt librpcsvc librt \
		libtelnet libterminfo \
		libusbhid libutil libwrap liby libz

.if !defined(BSD_MK_COMPAT_FILE)
SUBDIR+=	libkern
.endif

.if (${MACHINE_CPU} == "arm")
LIBC_MACHINE_ARCH?=${MACHINE_ARCH}
.if empty(LIBC_MACHINE_ARCH:M*hf*)
SUBDIR+=	libc_vfp
.endif
.endif
.if (${MKSKEY} != "no")
SUBDIR+=	libskey
.endif

.if (${MKMDNS} != "no")
SUBDIR+=	../external/apache2/mDNSResponder/lib
.endif

SUBDIR+=	../external/bsd/am-utils/lib
SUBDIR+=	../external/bsd/blacklist/lib

SUBDIR+=	../external/bsd/flex/lib
SUBDIR+=	../external/bsd/tre/lib
SUBDIR+=	../external/bsd/elftoolchain/lib/libelf
SUBDIR+=	../external/bsd/liblzf/lib
SUBDIR+=	../external/bsd/libpcap/lib

SUBDIR+=	../external/mit/expat/lib

SUBDIR+=	../external/public-domain/sqlite/lib
SUBDIR+=	../external/public-domain/xz/lib

SUBDIR+=	../gnu/lib/libmalloc

.if (${MKGCC} != "no")
SUBDIR+=	../external/gpl3/${EXTERNAL_GCC_SUBDIR}/lib/libobjc
SUBDIR+=	../external/gpl3/${EXTERNAL_GCC_SUBDIR}/lib/libgomp
# Should probably move GMP, MPFR and MPC builds into the GCC >= 4.5
# specific build area, but we get better parallelism this way.
# We don't build compat versions of these.
. if !defined(MLIBDIR)
SUBDIR+=	../external/lgpl3/gmp/lib/libgmp
SUBDIR+=	../external/lgpl3/mpfr/lib/libmpfr
SUBDIR+=	../external/lgpl3/mpc/lib/libmpc
. endif
.endif

#
# Libraries that depend upon any listed previously
# (and those that depend upon these [and ...])
#
#==================== 1st library dependency barrier ====================
SUBDIR+=	.WAIT

.if (${MKCRYPTO} != "no")
SUBDIR+=	../crypto/external/bsd/netpgp/libmj
SUBDIR+=	../crypto/external/bsd/netpgp/lib/verify # depends on libz
.endif

SUBDIR+=	../external/bsd/elftoolchain/lib/libdwarf # depends on libelf
SUBDIR+=	../external/mit/lua/lib # depends on libm
SUBDIR+=	libcurses	# depends on libterminfo
SUBDIR+=	libedit		# depends on libterminfo
SUBDIR+=	libexecinfo 	# depends on libelf
SUBDIR+=	libppath	# depends on libprop
SUBDIR+=	libperfuse	# depends on libpuffs
SUBDIR+=	libquota	# depends on libprop and librpcsvc
SUBDIR+=	librefuse	# depends on libpuffs

.if (${MKNPF} != "no")
SUBDIR+=	libnpf		# depends on libprop
.endif

.if (${MKCRYPTO} != "no")
SUBDIR+=	../crypto/external/bsd/openssl/lib # depends on libcrypt
.endif

SUBDIR+=	../external/bsd/file/lib	# depends on libz

SUBDIR+=	../external/bsd/libarchive/lib	# depends on libxz

.if (${MKBINUTILS} != "no")
SUBDIR+=	../external/gpl3/binutils/lib	# libbfd depends on libz
.endif

.if (${MKLIBCXX} != "no")
SUBDIR+=	../external/bsd/libc++
.endif

.if (${MKGCC} != "no" && ${MKCXX} != "no" && ${MKLIBSTDCXX} != "no")
SUBDIR+=	../external/gpl3/${EXTERNAL_GCC_SUBDIR}/lib/libstdc++-v3
SUBDIR+=	../external/gpl3/${EXTERNAL_GCC_SUBDIR}/lib/libsupc++
.endif
.if (${HAVE_GCC:U} == "48")
SUBDIR+=	../external/gpl3/${EXTERNAL_GCC_SUBDIR}/lib/libasan
.endif

#==================== 2nd library dependency barrier ====================
SUBDIR+=	.WAIT

.if (${MKNPF} != "no")
SUBDIR+=	npf		# depends on libnpf
.endif

SUBDIR+=	libform		# depends on libcurses
SUBDIR+=	libmenu		# depends on libcurses
SUBDIR+=	libradius	# depends on libcrypto if (${MKCRYPTO} != "no")

.if (${MKKERBEROS} != "no")
SUBDIR+=	../crypto/external/bsd/heimdal/lib	# depends on libcrypto
							# libedit, libterminfo,
.endif

.if (${MKCRYPTO} != "no")
SUBDIR+=	../crypto/external/bsd/openssh/lib # depends on libcrypto, libz
SUBDIR+=	../crypto/external/bsd/netpgp/lib  # depends on libcrypto, ...
.endif

SUBDIR+=	../external/bsd/libevent/lib	# depends on libcrypto
SUBDIR+=	../external/bsd/fetch/lib	# depends on libssl

.if (${MKLDAP} != "no")
SUBDIR+=	../external/bsd/openldap/lib	# depends on libcrypto, ...
.endif

#==================== 3rd library dependency barrier ====================
SUBDIR+=	.WAIT

SUBDIR+=	../external/bsd/bind/lib	# depends on heimdal, libcrypto

.if (${MKPAM} != "no")
SUBDIR+=	libpam		# depends on heimdal
.endif

.if (${MKCRYPTO} != "no")
SUBDIR+=	../crypto/external/bsd/libsaslc	# depends on heimdal, openssl
.endif

SUBDIR+=	../external/bsd/mdocml/lib

# Lua bindings come last, they might depend on anything
SUBDIR+=	lua

# Needed by rump and rescue which are outside the smbfs tree
SUBDIR+=	../external/bsd/smbfs/lib

.include <bsd.buildinstall.mk>
.include <bsd.subdir.mk>
