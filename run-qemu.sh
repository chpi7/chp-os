#!/bin/bash

qemu-system-i386 -boot d -cdrom ${1}/mykernel.iso -m 4096
