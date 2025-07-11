TARGET = simp_blkdev
obj-m += $(TARGET).o
cc=gcc
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all: modules
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean