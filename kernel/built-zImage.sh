#!/bin/sh

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean &&

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- epc_m6g2c_wifi_defconfig &&
#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- epc_m6g2c_defconfig &&
#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- epc_m6g2c_mfg_defconfig &&
#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- epc_m6g2c_no_encrypt_defconfig &&
#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- epc_m6g2c_wifi_no_encrypt_defconfig &&
#make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- dcp_1000l_defconfig &&

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- all -j8
