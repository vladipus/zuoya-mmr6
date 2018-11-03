# Zuoya MMR6 Linux Driver

Fixes some issues with a standard, generic USB driver for this mouse.

[Product Link](https://ru.aliexpress.com/item/ZUOYA-3200/32878117410.html)

Build with `make`
Install with `sudo make install`

Tested on Linux Mint 19.


## DKMS Installation

Automatic recompilation for new Linux kernels via DKMS.

```bash
cd zuoya-mmr6
VER=$(cat ./version.txt)
sudo rsync -rvhP ./ /usr/src/zuoya-mmr6-${VER}
sudo dkms add -m zuoya-mmr6 -v ${VER}
sudo dkms build -m zuoya-mmr6 -v ${VER}
sudo dkms install -m zuoya-mmr6 -v ${VER}
sudo modprobe hid-zuoya-mmr6
```
