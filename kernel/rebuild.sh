#!/bin/sh
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- all -j8

cp  arch/arm/boot/zImage /tftpboot
cp  arch/arm/boot/dts/a6g2c-rota.dtb /tftpboot

cp drivers/net/wireless/brcm80211/brcmutil/brcmutil.ko /opt/AppDriver/build/arm/

cp drivers/net/wireless/brcm80211/brcmfmac/brcmfmac.ko /opt/AppDriver/build/arm/


