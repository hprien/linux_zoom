# quad_tap_zoom
This programm reads the touchpad event device and toggles the gnome magnification glas on touchpad quad-tap-event.
# install
- build
- copy binary to `/opt/quad_tap_zoom`
- copy `quad_tap_zoom.service` to `/etc/systemd/user/quad_tap_zoom.service`
- find your event device with `libinput list-devices` command
- modify execution parameters in `/etc/systemd/user/quad_tap_zoom.service` according to your event device
- add you user to input group with `sudo usermod -aG input <your user name>`
- log out and in or reboot
- load daemons: `systemctl --user daemon-reload`
- autostart programm with `systemctl --user enable quad_tap_zoom`
# developer infos
## find coresponding event type and code
```sh
evtest /dev/input/eventX
```
