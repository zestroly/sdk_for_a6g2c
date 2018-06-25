CONFIG_CMD_SOURCE=y
CONFIG_BOOTCOMMAND="echo Booting from NAND...; if run loaddtb; then if run loadimage_0; then if bootz ${loadaddr} - ${fdt_addr}; then echo Booting from kernle_0;else if run loadimage_1; then if bootz ${loadaddr} - ${fdt_addr}; then echo Bootring from kernel_1;else echo Both of kernels are error; fi; fi; fi; else if run loadimage_1; then if bootz ${loadaddr} - ${fdt_addr}; then echo Booting from kernel_1; else echo both kernel are error; fi; fi; fi;else echo dtb is error; fi;"
CONFIG_BOARD_EARLY_INIT_F=y
CONFIG_VGA_AS_SINGLE_DEVICE=y
CONFIG_MXC_USB_FLAGS=0
CONFIG_IMX_CONFIG="board/freescale/mx6g2c/imximage.cfg"
CONFIG_CMD_ITEST=y
CONFIG_BOOTM_VXWORKS=y
CONFIG_CMD_EDITENV=y
CONFIG_CMD_MTDPARTS=y
CONFIG_SYS_MAX_NAND_DEVICE=y
CONFIG_FEC_DMA_MINALIGN=64
CONFIG_CMD_ENV_EXISTS=y
CONFIG_SYS_LONGHELP=y
CONFIG_SYS_NAND_5_ADDR_CYCLE=y
CONFIG_VIDEO_MXS=y
CONFIG_SYS_GENERIC_BOARD=y
CONFIG_SYS_LOAD_ADDR=$(CONFIG_LOADADDR)
CONFIG_DISPLAY_BOARDINFO=y
CONFIG_CMD_XIMG=y
CONFIG_CMD_CACHE=y
CONFIG_NETBOOT_ENV_SETTING="root_path=/home/vmuser/nfs_shared/rootfs0gw-ip=192.168.1.10netmask=255.255.255.00hostname=EPC-M6G2C0device=eth00image=zImage0netargs=setenv bootargs console=${console},${baudrate} root=/dev/nfs rw nfsroot=${serverip}:${root_path},v3,tcp ip=${ipaddr}:${serverip}:${gw-ip}:${netmask}:${hostname}:${device}0netboot=echo Booting from net ...; run netargs; tftp ${image}; if tftp ${fdt_addr} ${fdt_file}; then bootz ${loadaddr} - ${fdt_addr}; else if test ${boot_fdt} = try; then bootz; else echo WARN: Cannot load the DT; fi; fi;0"
CONFIG_STACKSIZE="SZ_128K"
CONFIG_BOOTDELAY=y
CONFIG_SYS_NAND_BASE=0x40000000
CONFIG_SYS_HELP_CMD_WIDTH=8
CONFIG_NR_DRAM_BANKS=y
CONFIG_FS_FAT=y
CONFIG_SYS_CBSIZE=1024
CONFIG_CMD_UBI=y
CONFIG_SYS_NAND_ONFI_DETECTION=y
CONFIG_EHCI_HCD_INIT_AFTER_RESET=y
CONFIG_BOOTM_LINUX=y
CONFIG_BOARD_LATE_INIT=y
CONFIG_CMD_CONSOLE=y
CONFIG_MII=y
CONFIG_CMD_FAT=y
CONFIG_SYS_CACHELINE_SIZE=64
CONFIG_MMC=y
CONFIG_MTD_PARTITIONS=y
CONFIG_REVISION_TAG=y
CONFIG_SYS_FSL_SEC_ADDR="CAAM_BASE_ADDR"
CONFIG_CMD_MISC=y
CONFIG_SYSCOUNTER_TIMER=y
CONFIG_ENV_OFFSET="(3 << 20)"
CONFIG_MXC_OCOTP=y
CONFIG_MX6=y
CONFIG_USB_MAX_CONTROLLER_COUNT=2
CONFIG_ENV_OVERWRITE=y
CONFIG_CMD_NET=y
CONFIG_CMD_NFS=y
CONFIG_ENV_SIZE=$(CONFIG_ENV_SECT_SIZE)
CONFIG_CMD_PING=y
CONFIG_SYS_MALLOC_LEN="(4 * SZ_1M)"
CONFIG_CMD_MEMTEST=y
CONFIG_SYS_MMC_ENV_DEV=0
CONFIG_SYS_I2C_SPEED=100000
CONFIG_SYS_TEXT_BASE=0x86800000
CONFIG_CMD_FLASH=y
CONFIG_CMD_SAVEENV=y
CONFIG_MXC_GPT_HCLK=y
CONFIG_MXC_UART=y
CONFIG_ENV_SECT_SIZE="(128 << 10)"
CONFIG_SPLASH_SCREEN=y
CONFIG_SYS_BARGSIZE=$(CONFIG_SYS_CBSIZE)
CONFIG_VIDEO_BMP_RLE8=y
CONFIG_MXC_USB_PORTSC="(PORT_PTS_UTMI | PORT_PTS_PTW)"
CONFIG_CMD_NAND=y
CONFIG_BOOTSTAGE_USER_COUNT=20
CONFIG_SYS_FSL_JR0_ADDR="(CAAM_BASE_ADDR + 0x1000)"
CONFIG_CMD_MEMORY=y
CONFIG_SYS_MAXARGS=256
CONFIG_BMP_16BPP=y
CONFIG_CMD_RUN=y
CONFIG_SYS_PBSIZE="(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)"
CONFIG_FEC_XCV_TYPE="RMII"
CONFIG_MXC_GPIO=y
CONFIG_BOARDDIR="board/freescale/mx6g2c"
CONFIG_OF_LIBFDT=y
CONFIG_PHYLIB=y
CONFIG_BOARD_POSTCLK_INIT=y
CONFIG_CMDLINE_EDITING=y
CONFIG_CMD_USB=y
CONFIG_MFG_ENV_SETTINGS="mfgtool_args=setenv bootargs console=${console},${baudrate} rdinit=/linuxrc g_mass_storage.stall=0 g_mass_storage.removable=1 g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF g_mass_storage.iSerialNumber=" "clk_ignore_unused 0initrd_addr=0x838000000initrd_high=0xffffffff0bootcmd_mfg=run mfgtool_args;bootz ${loadaddr} ${initrd_addr} ${fdt_addr};0"
CONFIG_SYS_CONSOLE_IS_IN_ENV=y
CONFIG_USB_EHCI=y
CONFIG_CMD_SETGETDCR=y
CONFIG_ZLIB=y
CONFIG_LOADADDR=0x80800000
CONFIG_ETHPRIME="FEC"
CONFIG_CMD_BOOTD=y
CONFIG_CMD_BOOTZ=y
CONFIG_AUTO_COMPLETE=y
CONFIG_SYS_MMC_IMG_LOAD_PART=y
CONFIG_FSL_USDHC=y
CONFIG_FEC_ENET_DEV=y
CONFIG_DOS_PARTITION=y
CONFIG_GZIP=y
CONFIG_APBH_DMA=y
CONFIG_CMD_FPGA=y
CONFIG_SYS_FSL_ESDHC_ADDR="USDHC2_BASE_ADDR"
CONFIG_SYS_INIT_RAM_SIZE="IRAM_SIZE"
CONFIG_FEC_MXC_PHYADDR=0x5
CONFIG_SYS_BAUDRATE_TABLE="{ 9600, 19200, 38400, 57600, 115200 }"
CONFIG_SYS_HUSH_PARSER=y
CONFIG_VIDEO=y
CONFIG_MFG_NAND_PARTITION="mtdparts=gpmi-nand:4m(boot),1m(dtb),8m(kernel0),8m(kernel1),4m(logo),73m(rootfs),-(data)"
CONFIG_SYS_SDRAM_BASE="PHYS_SDRAM"
CONFIG_IMAGE_FORMAT_LEGACY=y
CONFIG_SYS_BOOT_RAMDISK_HIGH=y
CONFIG_CFB_CONSOLE=y
CONFIG_BOARD_A6G2C=y
CONFIG_CMD_BDI=y
CONFIG_CMD_DHCP=y
CONFIG_ENV_IS_IN_NAND=y
CONFIG_SYS_FSL_USDHC_NUM=y
CONFIG_USB_ETHER_ASIX=y
CONFIG_CMD_ECHO=y
CONFIG_GENERIC_MMC=y
CONFIG_SYS_INIT_SP_OFFSET="(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)"
CONFIG_SYS_I2C=y
CONFIG_SYS_INIT_RAM_ADDR="IRAM_BASE_ADDR"
CONFIG_EXTRA_ENV_SETTINGS="CONFIG_MFG_ENV_SETTINGS SPLASH_SCREEN_ENV_SETTINGS "fdt_addr=0x830000000fdt_high=0xffffffff0fdt_file= FDT_FILE "0console=ttymxc00" CONFIG_NETBOOT_ENV_SETTING "bootargs=console=ttymxc0,115200 ubi.mtd=5 ro root=ubi0:rootfs rootfstype=ubifs g_mass_storage.stall=0 g_mass_storage.removable=1 g_mass_storage.idVendor=0x066F g_mass_storage.idProduct=0x37FF g_mass_storage.iSerialNumber=""0loaddtb=nand read ${fdt_addr} dtb0loadimage_0=nand read ${loadaddr} kernel00loadimage_1=nand read ${loadaddr} kernel10mtdids=" MTDIDS_DEFAULT "0mtdparts=" MTDPARTS_DEFAULT "0uplogo=tftp logo.bmp; nand erase.part logo; nand write ${loadaddr} logo ${filesize};0upkernel=if tftp zImage; then nand erase.part kernel0; nand write ${loadaddr} kernel0 ${filesize}; nand erase.part kernel1; nand write ${loadaddr} kernel1 ${filesize}; fi;0updtb=if tftp ${fdt_file}; then nand erase.part dtb; nand write ${loadaddr} dtb ${filesize}; fi;0uprootfs=if tftp rootfs.ubifs.img; then nand erase.part rootfs; ubi part rootfs; ubi create rootfs; ubi write ${loadaddr} rootfs ${filesize}; fi;0upsystem=run updtb;run upkernel;run uprootfs0"
CONFIG_SYS_INIT_SP_ADDR="(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)"
CONFIG_FSL_ESDHC=y
CONFIG_IMX_THERMAL=y
CONFIG_BAUDRATE=115200
CONFIG_CMD_BMODE=y
CONFIG_CMDLINE_TAG=y
CONFIG_ROM_UNIFIED_SECTIONS=y
CONFIG_MXC_UART_BASE="UART1_BASE"
CONFIG_SPLASH_SCREEN_ALIGN=y
CONFIG_INITRD_TAG=y
CONFIG_USB_HOST_ETHER=y
CONFIG_PARTITIONS=y
CONFIG_CMD_NAND_TRIMFFS=y
CONFIG_SYS_MEMTEST_END="(CONFIG_SYS_MEMTEST_START + (PHYS_SDRAM_SIZE >> 1))"
CONFIG_CMD_I2C=y
CONFIG_CMD_UBIFS=y
CONFIG_SYS_MMC_ENV_PART=0
CONFIG_FEC_MXC=y
CONFIG_NAND_MXS=y
CONFIG_SYS_NO_FLASH=y
CONFIG_SYS_DEF_EEPROM_ADDR=0
CONFIG_SYS_USE_NAND=y
CONFIG_MTD_DEVICE=y
CONFIG_SYS_PROMPT="MX6UL > "
CONFIG_USB_STORAGE=y
CONFIG_USB_EHCI_MX6=y
CONFIG_APBH_DMA_BURST8=y
CONFIG_DISPLAY_CPUINFO=y
CONFIG_MX6UL=y
CONFIG_MMCROOT="/dev/mmcblk0p2"
CONFIG_SETUP_MEMORY_TAGS=y
CONFIG_SYS_MEMTEST_START=0x80000000
CONFIG_CMD_LOADB=y
CONFIG_CMD_LOADS=y
CONFIG_CMD_IMI=y
CONFIG_CONS_INDEX=y
CONFIG_LMB=y
CONFIG_APBH_DMA_BURST=y
CONFIG_RBTREE=y
CONFIG_SYS_I2C_MXC=y
CONFIG_PHY_MICREL=y
CONFIG_SYS_BOOT_NAND=y
CONFIG_SC_TIMER_CLK=8000000
CONFIG_DDR_NAND_256M=y
CONFIG_SYS_VSNPRINTF=y
CONFIG_LZO=y
CONFIG_CMD_MII=y
CONFIG_CMD_BMP=y
CONFIG_VIDEO_LOGO=y
CONFIG_CMD_MMC=y
CONFIG_CMD_FUSE=y