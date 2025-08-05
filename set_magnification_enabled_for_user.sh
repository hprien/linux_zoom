#!/bin/bash

# this script can enable and disable the gnome magnification glas for a specific user
# usage: set_magnification_enabled_for_user.sh <username> <status>

# building user context
USER_NAME=$1
DISPLAY=:0
GNOME_PID=$(pgrep -u "$USER_NAME" gnome-session | head -n 1)

if [ -z "$GNOME_PID" ]; then
    echo "no gnome-session found for given user" >&2
    exit 1
fi

DBUS_SESSION_BUS_ADDRESS=$(grep -z DBUS_SESSION_BUS_ADDRESS /proc/$GNOME_PID/environ | tr '\0' '\n' | grep DBUS_SESSION_BUS_ADDRESS | cut -d= -f2-)

# executing magnification command
sudo -u "$USER_NAME" -H \
  DBUS_SESSION_BUS_ADDRESS=$DBUS_SESSION_BUS_ADDRESS \
  DISPLAY=$DISPLAY \
  gsettings set org.gnome.desktop.a11y.applications \
    screen-magnifier-enabled $2
