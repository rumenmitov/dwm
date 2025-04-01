# DWM startup script

dbus-daemon --session &
eval $(dbus-launch &) && export DBUS_SESSION_BUS_ADDRESS
exec ssh-agent zsh &
xrandr --output eDP --primary --mode 1920x1200 &
xrandr --output HDMI-A-0 --mode 1920x1080 --right-of eDP &
xset dpms 0 0 0 && xset s noblank  && xset s off &
setxkbmap -layout us,de,bg -variant ,,phonetic -option 'grp:shift_caps_toggle' &
~/.local/share/scripts/battery-warning.sh &
~/.fehbg &
slstatus &
pipewire &
picom --backend glx &
numlockx &
xsetroot -cursor_name left_ptr &
xinput --set-prop 9 310 0.8 &
greenclip daemon &
/usr/libexec/polkit-gnome-authentication-agent-1 &
kdeconnectd & 
nextcloud --background &
emacs --daemon &

exec dwm

