#!/bin/bash

set -e

export PREFIX="$(pwd)/gcc"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"

export BINUTILS_VERSION=2.35
export GCC_VERSION=10.2.0

mkdir -p build-binutils
( cd build-binutils &&
  ../binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror &&
  make $MAKEFLAGS &&
  make $MAKEFLAGS install
)

mkdir -p build-gcc
( cd build-gcc &&
  ../gcc-$GCC_VERSION/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers &&
  make $MAKEFLAGS all-gcc &&
  make $MAKEFLAGS all-target-libgcc CFLAGS_FOR_TARGET='-g -O2 -mcmodel=large -mno-red-zone' &&
  make $MAKEFLAGS install-gcc &&
  make $MAKEFLAGS install-target-libgcc
)
