add-symbol-file build/kernel/mykernel.elf
target remote | qemu-system-i386 -boot d -cdrom build/kernel/mykernel.iso -m 4096 -S -gdb stdio
