#	$NetBSD: Makefile,v 1.2 2014/07/26 19:56:30 dholland Exp $
#
# sysinst is usually built when the crunched install binary is built,
# but can be built here as a normal program for testing.

.if exists(arch/${MACHINE})
SUBDIR+=arch/${MACHINE}
.else
SUBDIR+=arch/dummy
.endif


.if ALL_MACHINES

SUBDIR  = arch/amd64
SUBDIR += arch/evbarm
SUBDIR += arch/i386

# XXX notyet SUBDIR += arch/iyonix

# These dirs are present in sys/arch but not here:
#    amigappc cesfic dreamcast epoc32 evbsh3 ia64 ibmnws iyonix luna68k
#    mmeye mvmeppc netwinder next68k rs6000 sbmips sun2 sun3
# (also evbsh5, but it doesn't really exist)
SUBDIR += arch/dummy

.endif

install:

.include <bsd.subdir.mk>
