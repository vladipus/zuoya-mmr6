obj-m = hid-mmr6.o
KVERSION = $(shell uname -r)
all:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
install:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules_install