#!/bin/bash

if [ -z "$1" ];
then
	echo "*** ERROR - No target specified. ***"
	exit 1
fi

output="mx6g_built/$1"

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$output distclean
echo "clean ok" &&
rm -r $output
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$output $1
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- O=$output -j8

echo "The target is ${output}/u-boot.imx "
