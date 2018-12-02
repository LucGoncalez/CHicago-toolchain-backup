# File author is Ítalo Lima Marconato Matias
#
# Created on November 11 of 2018, at 20:35 BRT
# Last edited on November 15 of 2018, at 14:48 BRT

ARCH ?= x86
VERBOSE ?= false

ROOT_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
PATH := $(ROOT_DIR)/$(ARCH)/bin:$(PATH)
SHELL := env PATH=$(PATH) /bin/bash

ifeq ($(ARCH),x86)
	TARGET ?= i686-chicago
else
	UNSUPPORTED_ARCH := true
endif

ifneq ($(VERBOSE),true)
NOECHO := @
endif

all: $(ARCH)
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

clean:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf build

clean-all:
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)rm -rf $(ARCH) build

remake: clean-all all
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif

$(ARCH):
ifeq ($(UNSUPPORTED_ARCH),true)
	$(error Unsupported architecture $(ARCH))
endif
	$(NOECHO)if [ -d $@ ]; then rm -rf $@; fi
	$(NOECHO)if [ -d build ]; then rm -rf build; fi
	$(NOECHO)mkdir build
	$(NOECHO)mkdir -p $@/sysroot/{Development,System}
	$(NOECHO)mkdir -p $@/sysroot/Development/{Headers,Libraries,Programs,Sources}
	$(NOECHO)mkdir -p $@/sysroot/System/{Configurations,Libraries,Programs}
	$(NOECHO)cp -RT dummy/ $@/sysroot/Development/Headers/
	$(NOECHO)echo Downloading Binutils source code
	$(NOECHO)cd build && wget -q https://ftp.gnu.org/gnu/binutils/binutils-2.29.1.tar.gz
	$(NOECHO)echo Downloading GCC source code
	$(NOECHO)cd build && wget -q https://ftp.gnu.org/gnu/gcc/gcc-7.2.0/gcc-7.2.0.tar.gz
	$(NOECHO)echo Extracting Binutils source code
	$(NOECHO)cd build && tar xpf binutils-2.29.1.tar.gz
	$(NOECHO)echo Extracting GCC source code
	$(NOECHO)cd build && tar xpf gcc-7.2.0.tar.gz
	$(NOECHO)echo Patching the Binutils source code
	$(NOECHO)cd build/binutils-2.29.1 && patch -Np1 -s -i ../../binutils.patch
	$(NOECHO)echo Patching the GCC source code
	$(NOECHO)cd build/gcc-7.2.0 && patch -Np1 -s -i ../../gcc.patch
	$(NOECHO)echo Compiling Binutils
	$(NOECHO)mkdir build/binutils-2.29.1/build && cd build/binutils-2.29.1/build && ../configure --target=$(TARGET) --prefix=$(ROOT_DIR)/$@ --with-sysroot=$(ROOT_DIR)/$@/sysroot --disable-nls --disable-werror 1>/dev/null 2>1 && make -j$(BUILD_CORES) 1>/dev/null 2>1 && make install -j$(BUILD_CORES) 1>/dev/null 2>1
	$(NOECHO)echo Compiling GCC
	$(NOECHO)mkdir build/gcc-7.2.0/build && cd build/gcc-7.2.0/build && ../configure --target=$(TARGET) --prefix=$(ROOT_DIR)/$@ --with-sysroot=$(ROOT_DIR)/$@/sysroot --enable-languages=c,c++ --enable-default-pie --with-linker-hash-style=sysv --disable-nls --disable-werror 1>/dev/null 2>1 && make all-gcc all-target-libgcc -j$(BUILD_CORES) 1>/dev/null 2>1 && make install-gcc install-target-libgcc 1>/dev/null 2>1
	$(NOECHO)echo Compiling Objconv
	$(NOECHO)gcc -std=gnu11 -Iobjconv/include -o $@/bin/objconv objconv/*.c
