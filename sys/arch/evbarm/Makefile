#	$NetBSD: Makefile,v 1.9.14.1 2015/05/11 11:00:33 msaitoh Exp $

# Makefile for evbarm tags file and boot blocks

# Find where ARM source files are for inclusion in tags
.include <../arm/Makefile.inc>

TEVBARM=	${SYSDIR}/arch/evbarm/tags
SEVBARM+=	${SYSDIR}/arch/evbarm/dev/*.[ch]
SEVBARM+=	${SYSDIR}/arch/evbarm/evbarm/*.[ch]
SEVBARM+=	${SYSDIR}/arch/evbarm/include/*.h
SEVBARM+=	${SYSDIR}/arch/evbarm/rpi/*.[ch]
SEVBARM+=	${SYSDIR}/arch/evbarm/smdk2xx0/*.[ch]

AEVBARM+=	${SYSDIR}/arch/evbarm/rpi/*.S
AEVBARM+=	${SYSDIR}/arch/evbarm/smdk2xx0/*.S

# Directories in which to place tags links
DEVBARM=	include

.include "../../kern/Make.tags.inc"

tags:
	-rm -f ${TEVBARM}
	-echo ${SEVBARM} ${SARM} | xargs ctags -wadtf ${TEVBARM}
	-${FINDCOMM} | xargs ctags -wadtf ${TEVBARM}
	egrep "^ENTRY(.*)|^ALTENTRY(.*)" ${AEVBARM} ${AARM} | \
	    ${TOOL_SED} -e \
		"s;\([^:]*\):\([^(]*\)(\([^, )]*\)\(.*\);\3	\1	/^\2(\3\4$$/;" \
	    >> ${TEVBARM}
	sort -o ${TEVBARM} ${TEVBARM}

links:
	-for i in ${DEVBARM}; do \
	    cd $$i && rm -f tags; ln -s ../tags tags; done


SUBDIR=	compile include stand

.include <bsd.subdir.mk>
