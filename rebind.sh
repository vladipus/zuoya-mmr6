#!/bin/sh

# run me as root

# first unbind them all
for dev in `ls /sys/bus/hid/drivers/hid-generic/ | egrep '18F8:1486'`
do echo -n $dev > /sys/bus/hid/drivers/hid-generic/unbind
done

# now bind to correct module
for dev in `ls /sys/bus/hid/devices/ | egrep '18F8:1486'`
do 
   echo -n $dev > /sys/bus/hid/drivers/hid-zuoya-mmr6/bind
done
