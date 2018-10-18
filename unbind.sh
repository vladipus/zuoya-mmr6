#!/bin/sh

# Run as root (sudo).

# Unbind:
for dev in `ls /sys/bus/hid/drivers/hid-generic/ | egrep '18F8:1486'`
do 
    echo -n $dev > /sys/bus/hid/drivers/hid-zuoya-mmr6/unbind
done

# Bind:
for dev in `ls /sys/bus/hid/devices/ | egrep '18F8:1486'`
do 
   echo -n $dev > /sys/bus/hid/drivers/hid-generic/bind
done
