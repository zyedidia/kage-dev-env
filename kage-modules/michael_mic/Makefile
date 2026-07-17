MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
BASE_DIR := $(MAKEFILE_DIR)/../..
KDIR := $(BASE_DIR)/out/ack/common/arm64
ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-
BINARY := michael_mic

NATIVE_LLVM_DIR ?= $(BASE_DIR)/toolchain/clang
LFI_LLVM_DIR    ?= $(BASE_DIR)/toolchain/aarch64-lfi-clang

# LFI=1 (default) builds the sandboxed variant; LFI=0 builds native.
LFI ?= 1

ifeq ($(LFI),1)
  LLVM_DIR     := $(LFI_LLVM_DIR)
  KBUILD_IAS   := 0
  VARIANT      := lfi
  EXTRA_CFLAGS := --target=aarch64_lfi-linux-gnu -fintegrated-as -DCONFIG_64BIT
else
  LLVM_DIR     := $(NATIVE_LLVM_DIR)
  KBUILD_IAS   := 1
  VARIANT      := native
  EXTRA_CFLAGS :=
endif

CC := $(LLVM_DIR)/bin/clang
LD := $(LLVM_DIR)/bin/ld.lld
AR := $(LLVM_DIR)/bin/llvm-ar

obj-m += $(BINARY).o

DIST := $(MAKEFILE_DIR)/dist

KMAKE = $(MAKE) -C $(KDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) \
        M=$(MAKEFILE_DIR) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
        CC="$(CC)" LD="$(LD)" AR="$(AR)" LLVM=1 LLVM_IAS=$(KBUILD_IAS)

all: modules
	@mkdir -p $(DIST)
	cp $(BINARY).ko $(DIST)/$(BINARY)-$(VARIANT).ko

modules:
	$(KMAKE) modules

native:
	$(MAKE) build-clean
	$(MAKE) LFI=0 all

lfi:
	$(MAKE) build-clean
	$(MAKE) LFI=1 all

both: native lfi

install: all
	@test -d $(DIST) || { echo "Nothing to install; run 'make both' (or 'make lfi' / 'make native') first"; exit 1; }
	cp $(DIST)/*.ko $(BASE_DIR)/shared

# Remove kbuild artifacts only; keep dist/ so both variants survive `both`.
build-clean:
	$(KMAKE) clean
	rm -f modules.order Module.symvers

clean: build-clean
	rm -rf $(DIST)

.PHONY: all modules native lfi both install build-clean clean
