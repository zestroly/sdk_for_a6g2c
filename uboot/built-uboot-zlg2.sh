#!/bin/bash

if [ -z "$1" ];then
	var=a6g2c_nand_256m_defconfig
	output=a6g2c-nand-256m
else
	var=a6g2c_nand_128m_defconfig
	output=a6g2c-nand-128m
fi

echo $var $output

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$output distclean
echo "clean ok" &&
rm -r $output
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$output $var
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$output -j8
