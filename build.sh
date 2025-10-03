#!/bin/bash
#Script to build buildroot configuration
#Author: Siddhant Jajoo
#Modified: Brandon Tardio

ln -s ext-tree base_external

LINE_TO_APPEND="sha256  01be32c9f2a1a48fffe81c1992ce6cea6bba7ebe5cb574533fd6d608d864e050  linux-5.15.19.tar.xz"

HASHFILE="buildroot/linux/linux.hash"

grep -qF -- "$LINE_TO_APPEND" "$HASHFILE" || echo "$LINE_TO_APPEND" >> "$HASHFILE"

# this patch is erroring
mv buildroot/board/qemu/patches/linux/0002-powerpc-boot-Fix-build-with-gcc-15.patch ./

source shared.sh

EXTERNAL_REL_BUILDROOT=../base_external
git submodule init
git submodule sync
git submodule update

if [ $# -lt 1 ]
then
        echo "Using default directory ${EXTERNAL_REL_BUILDROOT} for BR2_EXTERNAL"
    else
        EXTERNAL_REL_BUILDROOT=$1
        echo "Using passed directory ${EXTERNAL_REL_BUILDROOT} for BR2_EXTERNAL"
fi
 

mkdir -p buildroot
cp buildroot_working_menuconfig.config buildroot/.config

#p /repo/new_hardware_new_config /repo/buildroot/.config

if [ ! -d "arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu" ]; then
    wget -q https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
    tar -xf arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
    rm arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz
fi

PATH=$PATH:$(pwd)/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-linux-gnu/bin/
export PATH
ARCH=arm64
# CONFIG_DIR=$(pwd)/base_external/package/aesd-assignments/
# export CONFIG_DIR
BR2_EXTERNAL=$(pwd)/base_external/
#package/aesd-assignments/
export BR2_EXTERNAL
BR2_DEFCONFIG=$(pwd)/base_external/company/virt/qemu_aarch64_virt_defconfig
export BR2_DEFCONFIG

CC=aarch64-none-linux-gnu-gcc
export CC
CXX=aarch64-none-linux-gnu-g++
export CXX

CROSS_COMPILE=aarch64-none-linux-gnu-
export CROSS_COMPILE

MYDIR=$(pwd)

if [ ! -f ./Image ]; then
    bash ${MYDIR}/manual-linux.sh > outlogkernel
    cp /tmp/aeld/linux-stable/arch/${ARCH}/boot/Image ${MYDIR}
fi

# rm -r buildroot/package/ncurses/

make -C buildroot -j73

cd ${MYDIR}

set -e

# cp ${MYDIR}/Image ${MYDIR}/buildroot/output/images

set -e 
cd `dirname $0`

mkdir -p buildroot


if [ ! -e buildroot/.config ]
then
	echo "MISSING BUILDROOT CONFIGURATION FILE"

	if [ -e ${AESD_MODIFIED_DEFCONFIG} ]
	then
		echo "USING ${AESD_MODIFIED_DEFCONFIG}"
		make -C buildroot defconfig BR2_EXTERNAL=${EXTERNAL_REL_BUILDROOT} BR2_DEFCONFIG=${AESD_MODIFIED_DEFCONFIG_REL_BUILDROOT}
	else
		echo "Run ./save_config.sh to save this as the default configuration in ${AESD_MODIFIED_DEFCONFIG}"
		echo "Then add packages as needed to complete the installation, re-running ./save_config.sh as needed"
		make -C buildroot defconfig BR2_EXTERNAL=${EXTERNAL_REL_BUILDROOT} BR2_DEFCONFIG=${AESD_DEFAULT_DEFCONFIG}
	fi
else
	echo "USING EXISTING BUILDROOT CONFIG"
	echo "To force update, delete .config or make changes using make menuconfig and build again."
	make -C buildroot BR2_EXTERNAL=${EXTERNAL_REL_BUILDROOT} FORCE_UNSAFE_CONFIGURE=1

fi
