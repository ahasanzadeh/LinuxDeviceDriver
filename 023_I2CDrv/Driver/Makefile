
obj-m := i2c_client_drv.o

#export KERNEL_ROOT =$(BASE_INSTALL_DIR)/STEP-3/linux-4.15.3

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
install:
	$(MAKE) -C $(KDIR) M=$(PWD) module_install
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
