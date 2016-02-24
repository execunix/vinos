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

if [ -f /usr/pkg/bin/xfce4-session ] ; then
xrandr -s 1280x960
xfce4-session
else
xrandr -s 1024x768
TWM &
XCLOCK -geometry 50x50-1+1 &
XTERM -geometry 80x25+494-1 &
XTERM -geometry 80x25+494+60 &
exec XTERM -geometry 80x50+0+0 -name login
fi
