obj-m = hid-zuoya-mmr6.o
KVERSION = $(shell uname -r)
MDIR = $(shell pwd)

all:
	make -C /lib/modules/$(KVERSION)/build M=$(MDIR) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(MDIR) clean
install: all
	make -C /lib/modules/$(KVERSION)/build M=$(MDIR) modules_install
	cp -f 99-hid-zuoya-mmr6.rules /etc/udev/rules.d/
	depmod