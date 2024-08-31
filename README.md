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

### grub
```sh
sudo apt install grub2

# Download latest version from here https://www.gnu.org/software/xorriso
wget https://www.gnu.org/software/xorriso/xorriso-1.5.6.pl02.tar.gz
tar -xzf xorriso-1.5.6.pl02.tar.gz
cd xorriso-1.5.6.pl02
mkdir build && cd build
../configure
make
sudo make install
```

## Building
```sh
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=./i686-elf-none.cmake
cmake --build build -t iso
```

## Run with QEMU
```sh
# install
sudo apt install qemu-system-x86 qemu-system qemu
# then
./run-qemu.sh build/kernel
```
