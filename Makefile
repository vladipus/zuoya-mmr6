obj-m = hid-zuoya-mmr6.o
KVERSION = $(shell uname -r)
MDIR = $(shell pwd)
MODULE_DST = /lib/modules/$(KVERSION)/extra/hid-zuoya-mmr6.ko

all: hid-zuoya-mmr6.c
	make -C /lib/modules/$(KVERSION)/build M=$(MDIR) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(MDIR) clean
install: all
	make -C /lib/modules/$(KVERSION)/build M=$(MDIR) modules_install
	cp -f 99-hid-zuoya-mmr6.rules /etc/udev/rules.d/
	depmod
uninstall:
	bash ./unbind.sh
	modprobe -rf hid-zuoya-mmr6
	rm -f $(MODULE_DST)
	depmod