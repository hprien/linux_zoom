# quad_tap_zoom
This programm reads the touchpad event device and toggles the gnome magnification glas on touchpad quad-tap-event.
# install
- build
- copy binary to `/opt/quad_tap_zoom/`
- if you are using SELinux execute `semanage fcontext -a -t bin_t "/opt/quad_tap_zoom/quad_tap_zoom"` and `restorecon -v /opt/quad_tap_zoom`
- copy `set_magnification_enabled_for_user.sh` to `/opt/quad_tap_zoom/`
- copy `quad_tap_zoom.service` to `/etc/systemd/user/quad_tap_zoom.service`
- find the path of the event device of your touchpad with `libinput list-devices` command (something like `/dev/input/event6`)
- device paths (`/dev/input/event6`) may change on reboot. More consistent links to this device can be found with `ls -l /dev/input/by-id`. Search for a link to your device path
- The linkname sould look something like `/dev/input/by-id/usb-...-event-mouse`
- modify execution parameters in `/etc/systemd/user/quad_tap_zoom.service` according to your username and linkname
- load daemons: `sudo systemctl daemon-reload`
- autostart programm with `systemctl enable quad_tap_zoom`
# developer infos
## find coresponding event type and code
```sh
evtest /dev/input/eventX
```
