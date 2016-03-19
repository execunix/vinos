#	$NetBSD: Makefile,v 1.269.2.1 2015/04/21 05:22:58 snj Exp $
#	from: @(#)Makefile	5.20 (Berkeley) 6/12/93

.include <bsd.own.mk>

SUBDIR=	ac accton acpitools altq apm apmd arp \
	bootp btattach btconfig btdevctl bthcid btpand catman \
	chroot cnwctl cpuctl crash \
	dev_mkdb dumpfs \
	edquota eeprom envstat eshconfig etcupdate extattrctl \
	flashctl fssconfig fusermount fwctl \
	gpioctl gspa \
	hdaudioctl \
	i2cscan ifwatchd inetd installboot iopctl iostat ipwctl irdaattach \
	isdn isibootd iteconfig iwictl \
	kgmon \
	lastlogin ldpd link lmcconfig lockstat lpr \
	mailwrapper makefs map-mbone mdconfig mlxctl mmcformat \
	mopd mountd moused mrinfo mrouted mscdlabel mtrace mtree \
	netgroup_mkdb nfsd \
	ofctl \
	paxctl pcictl perfused psrset pstat pwd_mkdb postinstall \
	powerd \
	quot quotacheck quotaon quotarestore \
	rarpd rbootd rdate repquota rmt rpc.bootparamd rpc.lockd \
	rpc.pcnfsd rpc.statd rpcbind rwhod \
	sa screenblank sdpd service services_mkdb sesd schedctl \
	sliplogin spray \
	srtconfig sti sup sysinst syslogd \
	tcpdchk tcpdmatch tcpdrop timed tpctl tprof traceroute trpt \
	unlink usbdevs user \
	videomode vipw veriexecgen vnconfig \
	wakeonlan wiconfig wlanctl wsconscfg wsfontload wsmoused wsmuxctl \
	zdump zic

.if ${MKMAKEMANDB} != "no"
SUBDIR+= makemandb
.endif

.if (${MKYP} != "no")
SUBDIR+= rpc.yppasswdd ypbind yppoll ypserv ypset
.endif

SUBDIR+= puffs

.if ${TOOLCHAIN_MISSING} != "no"
SUBDIR+= mdsetimage
.endif

.if (${MKINET6} != "no")
SUBDIR+=faithd ifmcstat ndp rip6query rtsold
.endif
.if (${USE_INET6} != "no")
SUBDIR+=mld6query route6d rtadvd traceroute6 
.endif

# ATM PVC
SUBDIR+=pvcsif pvctxctl

.if (${MKCRYPTO} != "no")
SUBDIR+= racoon racoonctl
.endif

# NPF
.if (${MKNPF} != "no")
SUBDIR+=npf
.endif

# PF
.if (${MKPF} != "no")
SUBDIR+=pf
.endif

.include <bsd.subdir.mk>
