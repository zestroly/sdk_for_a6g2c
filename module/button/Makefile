# makefile for kernel module

MODNAME =button_input
CROSS_COMPILE = arm-linux-gnueabihf-
TESTPRE	= button_input_test

CC = arm-linux-gnueabihf-gcc
KERNELDIR ?= /opt/work/kernel

obj-m := $(MODNAME).o

default: test driver

test:
	@echo "Compiling test program..."
	$(CC) $(TESTPRE).c -o $(TESTPRE)
	@echo

driver:
	@echo "Compiling device driver..."
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	@echo

clean:
	@echo "Cleanup device driver..."
	@rm -rf *.o *.ko *.mod.* .$(MODNAME).* .tmp* module* Module*
	@echo
