
obj-m := chr_device_drv.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
CFLAGS := -C 

default:
	$(MAKE) $(CFLAGS) $(KDIR) M=$(PWD) modules
install:
	$(MAKE) $(CFLAGS) $(KDIR) M=$(PWD) module_install
clean:
	$(MAKE) $(CFLAGS) $(KDIR) M=$(PWD) clean
