src_dir=kernel
build_dir=build

cc=i686-elf-gcc
c_flags=-std=c11 -ffreestanding -g

ld=i686-elf-gcc
ld_flags=-ffreestanding -nostdlib -g

term: $(src_dir)/include/term.h $(src_dir)/term.c
	$(cc) $(c_flags) -I$(src_dir)/include -c $(src_dir)/term.c -o $(build_dir)/term.o

all: $(src_dir)/start.s $(src_dir)/start.o $(src_dir)/linker.ld term
	$(cc) $(c_flags) -c $(src_dir)/start.s -o $(build_dir)/start.o
	$(cc) $(c_flags) -I$(src_dir)/include -c $(src_dir)/kernel.c -o $(build_dir)/kernel.o
	$(cc) $(c_flags) -I$(src_dir)/include -c $(src_dir)/printf.c -o $(build_dir)/printf.o
	$(cc) $(c_flags) -I$(src_dir)/include -c $(src_dir)/memory.c -o $(build_dir)/memory.o
	$(ld) $(ld_flags) -T $(src_dir)/linker.ld $(build_dir)/start.o $(build_dir)/memory.o $(build_dir)/kernel.o $(build_dir)/printf.o $(build_dir)/term.o -o $(build_dir)/mykernel.elf -lgcc

package: all
	mkdir -p $(build_dir)/isoroot/boot/grub
	cp grub.cfg $(build_dir)/isoroot/boot/grub
	cp $(build_dir)/mykernel.elf $(build_dir)/isoroot/boot
	grub-mkrescue $(build_dir)/isoroot -o $(build_dir)/mykernel.iso

qemu-iso: package
	qemu-system-i386 -boot d -cdrom $(build_dir)/mykernel.iso -m 4096

