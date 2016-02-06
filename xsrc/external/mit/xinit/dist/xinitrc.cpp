XCOMM!SHELL_CMD

userresources=$HOME/.Xresources
usermodmap=$HOME/.Xmodmap
sysresources=XINITDIR/.Xresources
sysmodmap=XINITDIR/.Xmodmap

XCOMM merge in defaults and keymaps

if [ -f $sysresources ]; then
    XRDB -merge $sysresources
fi

if [ -f $sysmodmap ]; then
    XMODMAP $sysmodmap
fi

if [ -f "$userresources" ]; then
    XRDB -merge "$userresources"
fi

if [ -f "$usermodmap" ]; then
    XMODMAP "$usermodmap"
fi

XCOMM start some nice programs

if [ -d XINITDIR/xinitrc.d ] ; then
	for f in XINITDIR/xinitrc.d/?*.sh ; do
		[ -x "$f" ] && . "$f"
	done
	unset f
fi

xrandr -s 1024x768

#ifdef __TBD_XFCE4__
xfce4-session
#else
TWM &
XCLOCK -geometry 50x50-1+1 &
XTERM -geometry 80x25+486+60 &
XTERM -geometry 80x25+486-1 &
exec XTERM -geometry 80x50+0+0 -name login
#endif
