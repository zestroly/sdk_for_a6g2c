#!/bin/bash

#
# Build U-Boot image when `mkimage' tool is available.
#

MKIMAGE=$(type -path "${CROSS_COMPILE}mkimage")

if [ -z "${MKIMAGE}" ]; then
	MKIMAGE=$(type -path mkimage)
	if [ -z "${MKIMAGE}" ]; then
		# Doesn't exist
		echo '"mkimage" command not found - U-Boot images will not be built' >&2
		exit 1;
	fi
fi

# Call "mkimage" to create U-Boot image
${MKIMAGE} "$@"

echo "${MKIMAGE} \"$@\""

mv arch/arm/boot/dts/a6g2c-wifi-256m.dtb /tftpboot/
echo "mv arch/arm/boot/dts/a6g2c-wifi-256m.dtb /tftpboot/"
mv arch/arm/boot/zImage                 /tftpboot/
echo "arch/arm/boot/zImage                 /tftpboot/"

