
obj-m := io_port_MemoryMap_drv.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
install:
	$(MAKE) -C $(KDIR) M=$(PWD) module_install
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
