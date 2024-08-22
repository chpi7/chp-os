# CHP-OS

## Env setup + tools

Tree-sitter grammars:
```
linkerscript
asm
```

```sh
sudo apt install build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```

```sh
export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

### binutils
- version 2.38
- https://ftp.gnu.org/gnu/binutils/

```sh
mkdir build && cd build
../configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
# make install
```

### gcc
- version 11.4
- https://ftp.gnu.org/gnu/gcc/gcc-11.4.0/

Check that PATH is set up properly:
```sh
which i686-elf-as
```

Then build:
```sh
mkdir build && cd build
../configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```

## Building
```sh
mkdir build
make
```

## Run with QEMU
```sh
sudo apt install qemu-system-x86 qemu-system qemu
```
```sh
```
