MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
BASE_DIR := $(MAKEFILE_DIR)/../..
LFI_LLVM_DIR ?= $(BASE_DIR)/toolchain/aarch64-lfi-clang
KDIR := $(BASE_DIR)/out/ack/common/arm64
ARCH := arm64
BINARY := chardev
CROSS_COMPILE := aarch64-linux-gnu-
EXTRA_CFLAGS := $(C_FLAGS) --target=aarch64_lfi-linux-gnu -v -fintegrated-as -DCONFIG_64BIT

CC := $(LFI_LLVM_DIR)/bin/clang
LD := $(LFI_LLVM_DIR)/bin/ld.lld
AR := $(LFI_LLVM_DIR)/bin/llvm-ar

# Kernel include directories - these are crucial for compiling kage_ files
KBUILD_INC := -I$(BASE_DIR)/ack/common/arch/$(ARCH)/include \
              -I$(KDIR)/arch/$(ARCH)/include/generated \
              -I$(BASE_DIR)/ack/common/include \
              -I$(KDIR)/include \
              -I$(BASE_DIR)/ack/common/arch/$(ARCH)/include/uapi \
              -I$(KDIR)/arch/$(ARCH)/include/generated/uapi \
              -I$(BASE_DIR)/ack/common/include/uapi \
              -I$(KDIR)/include/generated/uapi \
              -include $(KDIR)/include/generated/autoconf.h

chardev-objs := chardev_main.o
obj-m += chardev.o

all: modules

install: all
	cp chardev.ko $(BASE_DIR)/shared

modules:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" CC="$(CC)" LD="$(LD)" LLVM=1 LLVM_IAS=0 modules

clean:
	$(MAKE) -C $(KDIR) ARCH=$(ARCH) M=$(PWD) clean
	rm -f modules.order
	rm -f Module.symvers
	rm -f *.ko *.mod.c *.mod.o *.o

.PHONY: install all modules clean
