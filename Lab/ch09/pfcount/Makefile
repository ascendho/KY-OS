ifneq($(KERNELRELEASE),)
	Obj-m:=readpfcount.o
else
	KDIR:=/lib/modules/$(shell uname –r)/bulid
	PWD:= $(shell pwd)
default:
	$(MAKE) –C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) –C $(KDIR) M=$(PWD) clean
endif