obj-m := myext2.o 
myext2-y := balloc.o dir.o file.o ialloc.o inode.o \
	  ioctl.o namei.o super.o symlink.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd) 
default: 
	make -C $(KDIR) M=$(PWD) modules