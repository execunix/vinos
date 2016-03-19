#	$NetBSD: Makefile,v 1.177 2014/08/10 05:57:31 matt Exp $

.include <bsd.own.mk>
.include <bsd.endian.mk>

# Make sure that the ordered build/install processing applies when using
# plain make.
.MAIN: build_install

# TOOLDIR must be valid, unless MKTOOLS=no
.if ${MKTOOLS:Uyes} != "no"
.if "${TOOLDIR}" == ""
.error "TOOLDIR is undefined or empty"
.elif "${TOOLDIR:tW:M/*}" == ""
.error "TOOLDIR is not an absolute path: ${TOOLDIR}"
#.elif !exists(TOOLDIR) # XXX .exists fails for directories
#.error "TOOLDIR does not exist: ${TOOLDIR}"
.endif
.endif # MKTOOLS != no

.if ${TOOLCHAIN_MISSING} == "no"
. if defined(HAVE_GCC) && ${HAVE_GCC} > 0
TOOLCHAIN_BITS= gmake .WAIT
TOOLCHAIN_BITS+= gmp .WAIT
TOOLCHAIN_BITS+= mpfr .WAIT
TOOLCHAIN_BITS+= mpc .WAIT
. endif
.endif

.if ${TOOLCHAIN_MISSING} == "no"
TOOLCHAIN_BITS+= binutils .WAIT
.endif

.if defined(HAVE_GCC) && ${HAVE_GCC} > 0
. if ${TOOLCHAIN_MISSING} == "no"
TOOLCHAIN_BITS+= gcc
.  if ${MKCROSSGDB:Uno} != "no" || make(obj)
TOOLCHAIN_BITS+= gdb
.  endif
TOOLCHAIN_BITS+= .WAIT
. endif
.endif

.if ${TOOLCHAIN_MISSING} == "no"
# XXX Eventually, we want to be able to build dbsym and mdsetimage
# XXX if EXTERNAL_TOOLCHAIN is set.
TOOLCHAIN_BITS+= dbsym mdsetimage
.endif

LINT_BITS=
.if ${MKLINT} != "no"
LINT_BITS= lint lint2
.endif

# All of host-mkdep, compat, and binstall are needed before anything
# else.  Within this group, they must be built in a specific order, and
# all of them must be built before any of them is installed.  They may
# be installed in any order.  This can't be expressed using the .WAIT
# notation inside the SUBDIR list.
#
# XXX .ORDER does not work when multiple targets are passed on the
# make command line without "-j", so use dependencies in addition to .ORDER.
#
.ORDER: dependall-host-mkdep dependall-compat dependall-binstall
.if make(dependall-host-mkdep) && make(dependall-compat)
dependall-compat: dependall-host-mkdep
.endif
.if make(dependall-compat) && make(dependall-binstall)
dependall-binstall: dependall-compat
.endif

# Dependencies in SUBDIR below ordered to maximize parallel ability.
# See above for special treatment for host-mkdep, compat, and binstall.
#
SUBDIR=	host-mkdep compat binstall \
	.WAIT mktemp .WAIT sed .WAIT
SUBDIR+= cap_mkdb crunchgen ctags genassym gencat hexdump \
		${LINT_BITS} \
		makewhatis mtree nbperf .WAIT uudecode

SUBDIR+= cat rpcgen join lorder m4 mkdep tsort .WAIT yacc .WAIT awk .WAIT lex

SUBDIR += .WAIT texinfo \
	.WAIT tic \
	.WAIT pax \
	.WAIT ${TOOLCHAIN_BITS} \
		asn1_compile cksum compile_et db \
		file lint1 slc \
		makefs .WAIT menuc mkcsmapper mkesdb mklocale mknod msgc \
		.WAIT paxctl \
		.WAIT fdisk \
		.WAIT installboot \
		pwd_mkdb strfile vgrind zic
SUBDIR+= stat .WAIT config

.if ${MKMAN} != "no" || ${MKDOC} != "no" || ${MKHTML} != "no"
. if ${MKGROFF} != "no"
SUBDIR+=	groff
. endif
SUBDIR+=	mandoc
.endif

.if ${MKMAINTAINERTOOLS:Uno} != "no"
SUBDIR+=	autoconf .WAIT gettext
.endif

.if ${USE_PIGZGZIP} != "no"
SUBDIR+=	pigz
.endif

.if ${MACHINE} == "evbarm" \
    && ${MACHINE_CPU} == "arm" \
    && ${TARGET_ENDIANNESS} == "1234"
SUBDIR+=	elftosb
.endif

.if ${MACHINE} == "evbarm"
SUBDIR+=	mkubootimage
.endif

check_MKTOOLS: .PHONY .NOTMAIN
.if ${MKTOOLS:Uyes} == "no"
	@echo '*** WARNING: "MKTOOLS" is set to "no"; this will prevent building and'
	@echo '*** updating your host toolchain.  This should be used only as a'
	@echo '*** temporary workaround for toolchain problems, as it will result'
	@echo '*** in version skew and build errors over time!'
.endif

.if ${MKTOOLS:Uyes} == "no" || ${USETOOLS} != "yes"	# {
SUBDIR= # empty
realall realdepend install: check_MKTOOLS
.endif							# }

.include <bsd.subdir.mk>
.include <bsd.buildinstall.mk>
.include <bsd.obj.mk>

.if !defined(PREVIOUSTOOLDIR)
.  if exists(PREVIOUSTOOLDIR)
PREVIOUSTOOLDIR!=	cat PREVIOUSTOOLDIR
.  else
PREVIOUSTOOLDIR=
.  endif
.endif

CLEANFILES+=	PREVIOUSTOOLDIR

realall realdepend: .MAKE
.if !empty(PREVIOUSTOOLDIR) && "${PREVIOUSTOOLDIR}" != "${TOOLDIR}"
	@echo "*** WARNING: TOOLDIR has moved?"
	@echo "*** PREVIOUSTOOLDIR '${PREVIOUSTOOLDIR}'"
	@echo "***     !=  TOOLDIR '${TOOLDIR}'"
	@echo "*** Cleaning mis-matched tools"
	rm -f PREVIOUSTOOLDIR
	(cd ${.CURDIR} && ${MAKE} PREVIOUSTOOLDIR=${TOOLDIR} cleandir)
.endif
	echo ${TOOLDIR} >PREVIOUSTOOLDIR

cleandir:
	rm -f ${CLEANFILES}
