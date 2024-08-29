#!/bin/bash

src_dir=$1
build_dir=$2

echo "Creating iso src_dir=${src_dir} build_dir=${build_dir}"

mkdir -p $build_dir/isoroot/boot/grub
cp $src_dir/grub.cfg $build_dir/isoroot/boot/grub
cp $build_dir/mykernel.elf $build_dir/isoroot/boot
grub-mkrescue $build_dir/isoroot -o $build_dir/mykernel.iso
