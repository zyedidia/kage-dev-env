# Kage Development Environment

Kage is an experimental subsystem of the Linux kernel for running kernel
modules in LFI sandboxes. The system is built on top of the Android Common
Kernel (ACK), which is the version of Linux used in Android devices.

To get started after cloning this repository, initialize the `ack/common` and
`ack/common-modules/virtual-device` modules.

```
git submodule update --init --recursive ack/common ack/common-modules/virtual-device
```

Create a `.env` file that sets

```
ACK=1
ARCH=arm64
```

Create the default rootfs image.

```
make rootfs-init
```

Build and run the Linux kernel.

```
make
make run
```

To exit QEMU, use `ctrl+a x`.

To attach with GDB, set the `GDB` environment variable before running:

```
GDB=1 make run
```

Then in another terminal, attach with:

```
scripts/gdb.sh
```

## Test module

There is a test module in `kage-modules/chardev`.

In order to build the module, you must have an LFI toolchain installed in
`toolchain/aarch64-lfi-clang`. You can get build one from
https://github.com/lfi-project/lfi-llvm-toolchain.

Once you have the toolchain, build and install the module.

```
cd kage-modules/chardev && make install
```

This will place `chardev.ko` into `shared/` in the repository root.

Boot the kernel and install the module.

```
make run
localhost:/# insmod shared/chardev.ko
```
