/*
 * Copyright (C) 2015-2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/clock.h>
#include <asm/arch/iomux.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/io.h>
#include <common.h>
#include <fsl_esdhc.h>
#include <i2c.h>
#include <linux/sizes.h>
#include <linux/fb.h>
#include <miiphy.h>
#include <mmc.h>
#include <mxsfb.h>
#include <netdev.h>
#include <usb.h>
#include <usb/ehci-fsl.h>
#include "lcd_setting.h"

#ifdef CONFIG_FSL_FASTBOOT
#include <fsl_fastboot.h>
#ifdef CONFIG_ANDROID_RECOVERY
#include <recovery.h>
#endif
#endif /*CONFIG_FSL_FASTBOOT*/

/* add by Codebreaker */
#define MY_DEBUG 
#ifdef MY_DEBUG
#define pdebug(fmt, args...)  printf(fmt, ##args)
#else
#define pdebug(fmt, args...)  /*do nothing */
#endif
/* add end */

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_22K_UP  | PAD_CTL_SPEED_LOW |		\
	PAD_CTL_DSE_80ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_DAT3_CD_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_100K_DOWN  | PAD_CTL_SPEED_LOW |		\
	PAD_CTL_DSE_80ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)


#define ENET_PAD_CTRL  (PAD_CTL_PUS_100K_UP | PAD_CTL_PUE |     \
	PAD_CTL_SPEED_HIGH   |                                   \
	PAD_CTL_DSE_48ohm   | PAD_CTL_SRE_FAST)

#define MDIO_PAD_CTRL  (PAD_CTL_PUS_100K_UP | PAD_CTL_PUE |     \
	PAD_CTL_DSE_48ohm   | PAD_CTL_SRE_FAST | PAD_CTL_ODE)


#define ENET_CLK_PAD_CTRL  (PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST)

#define ENET_RX_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |          \
	PAD_CTL_SPEED_HIGH   | PAD_CTL_SRE_FAST)

#define I2C_PAD_CTRL    (PAD_CTL_PKE | PAD_CTL_PUE |            \
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |               \
	PAD_CTL_DSE_40ohm | PAD_CTL_HYS |			\
	PAD_CTL_ODE)

#define LCD_PAD_CTRL    (PAD_CTL_HYS | PAD_CTL_PUS_100K_UP | PAD_CTL_PUE | \
	PAD_CTL_PKE | PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm)

#define GPMI_PAD_CTRL0 (PAD_CTL_PKE | PAD_CTL_PUE | PAD_CTL_PUS_100K_UP)
#define GPMI_PAD_CTRL1 (PAD_CTL_DSE_40ohm | PAD_CTL_SPEED_MED | \
			PAD_CTL_SRE_FAST)
#define GPMI_PAD_CTRL2 (GPMI_PAD_CTRL0 | GPMI_PAD_CTRL1)

#define OTG_ID_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE | \
				PAD_CTL_PUS_47K_UP | PAD_CTL_SPEED_LOW | \
				PAD_CTL_DSE_80ohm  | PAD_CTL_SRE_FAST | PAD_CTL_HYS)

#define WEIM_NOR_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE | \
		PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED | \
		PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST)

#define SPI_PAD_CTRL (PAD_CTL_HYS |				\
	PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_40ohm | PAD_CTL_SRE_FAST)


#define IOX_SDI IMX_GPIO_NR(5, 10)
#define IOX_STCP IMX_GPIO_NR(5, 7)
#define IOX_SHCP IMX_GPIO_NR(5, 11)
#define IOX_OE IMX_GPIO_NR(5, 18)
/* add by Codebreaker */
#define IOX_PHY1_RESET IMX_GPIO_NR(5,3)
#define IOX_PHY2_RESET IMX_GPIO_NR(5,4)
#define IOX_WDT  	   IMX_GPIO_NR(5, 0)
#define IOX_BACKLIGHT  IMX_GPIO_NR(3,23)

/* modifed by Codebreaker */
static iomux_v3_cfg_t const iox_pads[] = {
	MX6_PAD_SNVS_TAMPER0__GPIO5_IO00 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER1__GPIO5_IO01 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER2__GPIO5_IO02 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER3__GPIO5_IO03 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER4__GPIO5_IO04 | MUX_PAD_CTRL(I2C_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER5__GPIO5_IO05 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER6__GPIO5_IO06 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER7__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER8__GPIO5_IO08 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_SNVS_TAMPER9__GPIO5_IO09 | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_LCD_RESET__GPIO3_IO04	 | MUX_PAD_CTRL(NO_PAD_CTRL),
};
/* end */

#ifdef CONFIG_SYS_I2C_MXC
#define PC MUX_PAD_CTRL(I2C_PAD_CTRL)
/* I2C1 for PMIC and EEPROM */
struct i2c_pads_info i2c_pad_info1 = {
	.scl = {
		.i2c_mode =  MX6_PAD_UART4_TX_DATA__I2C1_SCL | PC,
		.gpio_mode = MX6_PAD_UART4_TX_DATA__GPIO1_IO28 | PC,
		.gp = IMX_GPIO_NR(1, 28),
	},
	.sda = {
		.i2c_mode = MX6_PAD_UART4_RX_DATA__I2C1_SDA | PC,
		.gpio_mode = MX6_PAD_UART4_RX_DATA__GPIO1_IO29 | PC,
		.gp = IMX_GPIO_NR(1, 29),
	},
};
#endif

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_SIZE;

	return 0;
}

static iomux_v3_cfg_t const uart1_pads[] = {
	MX6_PAD_UART1_TX_DATA__UART1_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART1_RX_DATA__UART1_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

/* add by Codebreaker */
static iomux_v3_cfg_t const uart2_pads[] = {
	MX6_PAD_UART2_TX_DATA__UART2_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART2_RX_DATA__UART2_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const uart3_pads[] = {
	MX6_PAD_UART3_TX_DATA__UART3_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART3_RX_DATA__UART3_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const uart4_pads[] = {
	MX6_PAD_UART4_TX_DATA__UART4_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART4_RX_DATA__UART4_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const uart5_pads[] = {
	MX6_PAD_UART5_TX_DATA__UART5_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART5_RX_DATA__UART5_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const uart7_pads[] = {
	MX6_PAD_LCD_DATA16__UART7_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_LCD_DATA17__UART7_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const uart8_pads[] = {
	MX6_PAD_LCD_DATA20__UART8_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_LCD_DATA21__UART8_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};
/* add end */

static iomux_v3_cfg_t const usdhc1_pads[] = {
	MX6_PAD_SD1_CLK__USDHC1_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_CMD__USDHC1_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA0__USDHC1_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA1__USDHC1_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA2__USDHC1_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA3__USDHC1_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/* CD */
	MX6_PAD_UART1_RTS_B__GPIO1_IO19 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* WP */
	MX6_PAD_UART1_CTS_B__USDHC1_WP  | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

#if 0
#if defined(CONFIG_MX6UL_EVK_EMMC_REWORK)
static iomux_v3_cfg_t const usdhc2_emmc_pads[] = {
	MX6_PAD_NAND_RE_B__USDHC2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_WE_B__USDHC2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA00__USDHC2_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA01__USDHC2_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA02__USDHC2_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA03__USDHC2_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA04__USDHC2_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA05__USDHC2_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA06__USDHC2_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA07__USDHC2_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/*
	 * RST_B
	 */
	MX6_PAD_NAND_ALE__GPIO4_IO10 | MUX_PAD_CTRL(NO_PAD_CTRL),
};
#else
static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_NAND_RE_B__USDHC2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_WE_B__USDHC2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA00__USDHC2_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA01__USDHC2_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA02__USDHC2_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA03__USDHC2_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc2_cd_pads[] = {

	/* The evk board uses DAT3 to detect CD card plugin, in u-boot we mux the pin to
	  * GPIO when doing board_mmc_getcd.
	  */
	MX6_PAD_NAND_DATA03__GPIO4_IO05 | MUX_PAD_CTRL(USDHC_DAT3_CD_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc2_dat3_pads[] = {
	MX6_PAD_NAND_DATA03__USDHC2_DATA3 | MUX_PAD_CTRL(USDHC_DAT3_CD_PAD_CTRL),
};


#endif
#endif

#ifdef CONFIG_SYS_USE_NAND
static iomux_v3_cfg_t const nand_pads[] = {
	MX6_PAD_NAND_DATA00__RAWNAND_DATA00 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_DATA01__RAWNAND_DATA01 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_DATA02__RAWNAND_DATA02 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_DATA03__RAWNAND_DATA03 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_DATA04__RAWNAND_DATA04 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_DATA05__RAWNAND_DATA05 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_DATA06__RAWNAND_DATA06 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_DATA07__RAWNAND_DATA07 | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_CLE__RAWNAND_CLE | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_ALE__RAWNAND_ALE | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_CE0_B__RAWNAND_CE0_B | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
//	MX6_PAD_NAND_CE1_B__RAWNAND_CE1_B | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_RE_B__RAWNAND_RE_B | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_WE_B__RAWNAND_WE_B | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_WP_B__RAWNAND_WP_B | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
	MX6_PAD_NAND_READY_B__RAWNAND_READY_B | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
//	MX6_PAD_NAND_DQS__RAWNAND_DQS | MUX_PAD_CTRL(GPMI_PAD_CTRL2),
};

static void setup_gpmi_nand(void)
{
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;

	/* config gpmi nand iomux */
	imx_iomux_v3_setup_multiple_pads(nand_pads, ARRAY_SIZE(nand_pads));

	clrbits_le32(&mxc_ccm->CCGR4,
		     MXC_CCM_CCGR4_RAWNAND_U_BCH_INPUT_APB_MASK |
		     MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_BCH_MASK |
		     MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_GPMI_IO_MASK |
		     MXC_CCM_CCGR4_RAWNAND_U_GPMI_INPUT_APB_MASK |
		     MXC_CCM_CCGR4_PL301_MX6QPER1_BCH_MASK);

	/*
	 * config gpmi and bch clock to 100 MHz
	 * bch/gpmi select PLL2 PFD2 400M
	 * 100M = 400M / 4
	 */
	clrbits_le32(&mxc_ccm->cscmr1,
		     MXC_CCM_CSCMR1_BCH_CLK_SEL |
		     MXC_CCM_CSCMR1_GPMI_CLK_SEL);
	clrsetbits_le32(&mxc_ccm->cscdr1,
			MXC_CCM_CSCDR1_BCH_PODF_MASK |
			MXC_CCM_CSCDR1_GPMI_PODF_MASK,
			(3 << MXC_CCM_CSCDR1_BCH_PODF_OFFSET) |
			(3 << MXC_CCM_CSCDR1_GPMI_PODF_OFFSET));

	/* enable gpmi and bch clock gating */
	setbits_le32(&mxc_ccm->CCGR4,
		     MXC_CCM_CCGR4_RAWNAND_U_BCH_INPUT_APB_MASK |
		     MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_BCH_MASK |
		     MXC_CCM_CCGR4_RAWNAND_U_GPMI_BCH_INPUT_GPMI_IO_MASK |
		     MXC_CCM_CCGR4_RAWNAND_U_GPMI_INPUT_APB_MASK |
		     MXC_CCM_CCGR4_PL301_MX6QPER1_BCH_MASK);

	/* enable apbh clock gating */
	setbits_le32(&mxc_ccm->CCGR0, MXC_CCM_CCGR0_APBHDMA_MASK);
}
#endif

#ifdef CONFIG_FEC_MXC
/*
 * pin conflicts for fec1 and fec2, GPIO1_IO06 and GPIO1_IO07 can only
 * be used for ENET1 or ENET2, cannot be used for both.
 */
static iomux_v3_cfg_t const fec1_pads[] = {
	MX6_PAD_ENET1_TX_DATA0__ENET1_TDATA00 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET1_TX_DATA1__ENET1_TDATA01 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET1_TX_EN__ENET1_TX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET1_TX_CLK__ENET1_REF_CLK1 | MUX_PAD_CTRL(ENET_CLK_PAD_CTRL),
	MX6_PAD_ENET1_RX_DATA0__ENET1_RDATA00 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET1_RX_DATA1__ENET1_RDATA01 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET1_RX_ER__ENET1_RX_ER | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET1_RX_EN__ENET1_RX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
};

static iomux_v3_cfg_t const fec1_mdio_pads[] = {
	MX6_PAD_GPIO1_IO06__ENET1_MDIO | MUX_PAD_CTRL(MDIO_PAD_CTRL),
	MX6_PAD_GPIO1_IO07__ENET1_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),
};

static iomux_v3_cfg_t const fec2_pads[] = {
	MX6_PAD_GPIO1_IO06__ENET2_MDIO | MUX_PAD_CTRL(MDIO_PAD_CTRL),
	MX6_PAD_GPIO1_IO07__ENET2_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_TX_DATA0__ENET2_TDATA00 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_TX_DATA1__ENET2_TDATA01 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_TX_CLK__ENET2_REF_CLK2 | MUX_PAD_CTRL(ENET_CLK_PAD_CTRL),
	MX6_PAD_ENET2_TX_EN__ENET2_TX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),

	MX6_PAD_ENET2_RX_DATA0__ENET2_RDATA00 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_RX_DATA1__ENET2_RDATA01 | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_RX_EN__ENET2_RX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
	MX6_PAD_ENET2_RX_ER__ENET2_RX_ER | MUX_PAD_CTRL(ENET_PAD_CTRL),
};

static void setup_iomux_fec(int fec_id)
{
#ifdef CONFIG_BOARD_1C
	imx_iomux_v3_setup_multiple_pads(fec1_mdio_pads, \
								ARRAY_SIZE(fec1_mdio_pads));
	imx_iomux_v3_setup_multiple_pads(fec1_pads, ARRAY_SIZE(fec1_pads));
#else
	imx_iomux_v3_setup_multiple_pads(fec1_pads, ARRAY_SIZE(fec1_pads));
	imx_iomux_v3_setup_multiple_pads(fec2_pads, ARRAY_SIZE(fec2_pads));
#endif /* end of CONFIG_BOARD_1C */

}
#endif /* end of CONFIG_FEC_MXC */

static void setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart1_pads, ARRAY_SIZE(uart1_pads));
	imx_iomux_v3_setup_multiple_pads(uart2_pads, ARRAY_SIZE(uart2_pads));
	imx_iomux_v3_setup_multiple_pads(uart3_pads, ARRAY_SIZE(uart3_pads));
	imx_iomux_v3_setup_multiple_pads(uart4_pads, ARRAY_SIZE(uart4_pads));
	imx_iomux_v3_setup_multiple_pads(uart5_pads, ARRAY_SIZE(uart5_pads));
	imx_iomux_v3_setup_multiple_pads(uart7_pads, ARRAY_SIZE(uart7_pads));
	imx_iomux_v3_setup_multiple_pads(uart8_pads, ARRAY_SIZE(uart8_pads));

}

#ifdef CONFIG_FSL_QSPI

#define QSPI_PAD_CTRL1	\
	(PAD_CTL_SRE_FAST | PAD_CTL_SPEED_MED | \
	 PAD_CTL_PKE | PAD_CTL_PUE | PAD_CTL_PUS_47K_UP | PAD_CTL_DSE_60ohm)

static iomux_v3_cfg_t const quadspi_pads[] = {
	MX6_PAD_NAND_DATA02__QSPI_B_DATA00	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_DATA03__QSPI_B_DATA01	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_DATA04__QSPI_B_DATA02	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_DATA05__QSPI_B_DATA03	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_RE_B__QSPI_B_SCLK	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_WE_B__QSPI_B_SS0_B	| MUX_PAD_CTRL(QSPI_PAD_CTRL1),
};

int board_qspi_init(void)
{
	/* Set the iomux */
	imx_iomux_v3_setup_multiple_pads(quadspi_pads, ARRAY_SIZE(quadspi_pads));

	/* Set the clock */
	enable_qspi_clk(0);

	return 0;
}
#endif

#ifdef CONFIG_FSL_ESDHC
static struct fsl_esdhc_cfg usdhc_cfg[2] = {
	{USDHC1_BASE_ADDR, 0, 4},
#if defined(CONFIG_MX6UL_EVK_EMMC_REWORK)
	{USDHC2_BASE_ADDR, 0, 8},
#else
	{USDHC2_BASE_ADDR, 0, 4},
#endif
};

#define USDHC1_CD_GPIO	IMX_GPIO_NR(1, 19)

int mmc_get_env_devno(void)
{
	u32 soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	int dev_no;
	u32 bootsel;

	bootsel = (soc_sbmr & 0x000000FF) >> 6 ;

	/* If not boot from sd/mmc, use default value */
	if (bootsel != 1)
		return CONFIG_SYS_MMC_ENV_DEV;

	/* BOOT_CFG2[3] and BOOT_CFG2[4] */
	dev_no = (soc_sbmr & 0x00001800) >> 11;

	return dev_no;
}

int mmc_map_to_kernel_blk(int dev_no)
{
	return dev_no;
}

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
	case USDHC1_BASE_ADDR:
		ret = !gpio_get_value(USDHC1_CD_GPIO);
		break;
#if 0
	case USDHC2_BASE_ADDR:
#if defined(CONFIG_MX6UL_EVK_EMMC_REWORK)
		ret = 1;
#else
		imx_iomux_v3_setup_multiple_pads(
				usdhc2_cd_pads, ARRAY_SIZE(usdhc2_cd_pads));
		gpio_direction_input(USDHC2_CD_GPIO);

		/* Since it is the DAT3 pin, this pin is pulled to a low voltage if no card */
		ret = gpio_get_value(USDHC2_CD_GPIO);

		imx_iomux_v3_setup_multiple_pads(
				usdhc2_dat3_pads, ARRAY_SIZE(usdhc2_dat3_pads));
#endif
		break;
#endif
	/* add by Codebreaker */
	default:
		break;
	/* add end */
	}

	return ret;

}

int board_mmc_init(bd_t *bis)
{
	int i, ret;

	/*
	 * According to the board_mmc_init() the following map is done:
	 * (U-boot device node)    (Physical Port)
	 * mmc0                    USDHC1
	 * mmc1                    USDHC2
	 */
	for (i = 0; i < CONFIG_SYS_FSL_USDHC_NUM; i++) {
		switch (i) {
		case 0:
			imx_iomux_v3_setup_multiple_pads(
				usdhc1_pads, ARRAY_SIZE(usdhc1_pads));
			gpio_direction_input(USDHC1_CD_GPIO);
			usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);

/*			gpio_direction_output(USDHC1_PWR_GPIO, 0);
			udelay(500);
			gpio_direction_output(USDHC1_PWR_GPIO, 1);
*/
			break;
#if 0
		case 1:
#if defined(CONFIG_MX6UL_EVK_EMMC_REWORK)
			imx_iomux_v3_setup_multiple_pads(
				usdhc2_emmc_pads, ARRAY_SIZE(usdhc2_emmc_pads));
#else
			imx_iomux_v3_setup_multiple_pads(
				usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
#endif
			gpio_direction_output(USDHC2_PWR_GPIO, 0);
			udelay(500);
			gpio_direction_output(USDHC2_PWR_GPIO, 1);
			usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
			break;
#endif
		default:
			printf("Warning: you configured more USDHC controllers"
				"(%d) than supported by the board\n", i + 1);
			return -EINVAL;
		}

		ret = fsl_esdhc_initialize(bis, &usdhc_cfg[i]);
		if (ret) {
			printf("Warning: failed to initialize mmc dev %d\n", i);
		}
	}

	return 0;
}

int check_mmc_autodetect(void)
{
	char *autodetect_str = getenv("mmcautodetect");

	if ((autodetect_str != NULL) &&
		(strcmp(autodetect_str, "yes") == 0)) {
		return 1;
	}

	return 0;
}

void board_late_mmc_init(void)
{
	char cmd[32];
	char mmcblk[32];
	u32 dev_no = mmc_get_env_devno();

	if (!check_mmc_autodetect())
		return;

	setenv_ulong("mmcdev", dev_no);

	/* Set mmcblk env */
	sprintf(mmcblk, "/dev/mmcblk%dp2 rootwait rw",
		mmc_map_to_kernel_blk(dev_no));
	setenv("mmcroot", mmcblk);

	sprintf(cmd, "mmc dev %d", dev_no);
	run_command(cmd, 0);
}
#endif

#ifdef CONFIG_VIDEO_MXS
iomux_v3_cfg_t const lcd_pads[23] = {
	MX6_PAD_LCD_CLK__LCDIF_CLK | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_ENABLE__LCDIF_ENABLE | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_HSYNC__LCDIF_HSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_VSYNC__LCDIF_VSYNC | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA00__LCDIF_DATA00 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA01__LCDIF_DATA01 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA02__LCDIF_DATA02 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA03__LCDIF_DATA03 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA04__LCDIF_DATA04 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA05__LCDIF_DATA05 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA06__LCDIF_DATA06 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA07__LCDIF_DATA07 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA08__LCDIF_DATA08 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA09__LCDIF_DATA09 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA10__LCDIF_DATA10 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA11__LCDIF_DATA11 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA12__LCDIF_DATA12 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA13__LCDIF_DATA13 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA14__LCDIF_DATA14 | MUX_PAD_CTRL(LCD_PAD_CTRL),
	MX6_PAD_LCD_DATA15__LCDIF_DATA15 | MUX_PAD_CTRL(LCD_PAD_CTRL),

	/* add by Codebreaker */
	MX6_PAD_LCD_DATA22__MQS_RIGHT    | MUX_PAD_CTRL(NO_PAD_CTRL),
	MX6_PAD_LCD_DATA23__MQS_LEFT     | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* add end */

	/*
	 * Use GPIO for Brightness adjustment, duty cycle = period.
	 */
	MX6_PAD_LCD_DATA18__GPIO3_IO23   | MUX_PAD_CTRL(NO_PAD_CTRL),
};

int board_video_skip(void)
{
	int i;
	int ret;
	char const *panel = getenv("panel");

	/* get the customize parameters */
	override_lcd_settings();

	pdebug("panel name: %s\n", panel);
	if (!panel) {
		panel = displays[0].mode.name;
		printf("No panel detected: default to %s\n", panel);
		i = 0;
	} else {
		for (i = 0; i < ARRAY_SIZE(displays); i++) {
			if (!strcmp(panel, displays[i].mode.name)) 
				break;
		}
	}
	if (i < ARRAY_SIZE(displays)) {
		ret = mxs_lcd_panel_setup(displays[i].mode, displays[i].depth,
				    displays[i].lcdif_base_addr);
		if (!ret) {
			if (displays[i].enable)
				displays[i].enable(displays+i);
			printf("Display: %s (%ux%u)\n",
			       displays[i].mode.name,
			       displays[i].mode.xres,
			       displays[i].mode.yres);
		} else
			printf("LCD %s cannot be configured: %d\n",
			       displays[i].mode.name, ret);
	} else {
		printf("unsupported panel %s\n", panel);
		return -EINVAL;
	}

	/* turn on LCD backlight */
//	gpio_request(IOX_BACKLIGHT, "backlight");
//	gpio_direction_output(IOX_BACKLIGHT, 1);

	return 0;
}
#endif

#ifdef CONFIG_FEC_MXC
int board_eth_init(bd_t *bis)
{
	int ret;
	int enet_dev;

	enet_dev = CONFIG_FEC_ENET_DEV;
	setup_iomux_fec(enet_dev);

	ret = fecmxc_initialize_multi(bis, CONFIG_FEC_ENET_DEV,
		CONFIG_FEC_MXC_PHYADDR, IMX_FEC_BASE);
	if (ret)
		printf("FEC%d MXC: %s:failed\n", CONFIG_FEC_ENET_DEV, __func__);

	return 0;
}

static int setup_fec(int fec_id)
{
	struct iomuxc_gpr_base_regs *const iomuxc_gpr_regs
		= (struct iomuxc_gpr_base_regs *) IOMUXC_GPR_BASE_ADDR;
	int ret;

	if (0 == fec_id) {
		/* Use 50M anatop loopback REF_CLK1 for ENET1, clear gpr1[13], set gpr1[17]*/
		clrsetbits_le32(&iomuxc_gpr_regs->gpr[1], IOMUX_GPR1_FEC1_MASK,
				IOMUX_GPR1_FEC1_CLOCK_MUX1_SEL_MASK);
	} else {
		/* Use 50M anatop loopback REF_CLK2 for ENET2, clear gpr1[14], set gpr1[18]*/
		clrsetbits_le32(&iomuxc_gpr_regs->gpr[1], IOMUX_GPR1_FEC2_MASK,
				IOMUX_GPR1_FEC2_CLOCK_MUX1_SEL_MASK);
	}

	ret = enable_fec_anatop_clock(fec_id, ENET_50MHZ);
	if (ret)
		return ret;

	enable_enet_clk(1);

	return 0;
}

int board_phy_config(struct phy_device *phydev)
{
//	phy_write(phydev, MDIO_DEVAD_NONE, 0x1f, 0x8190);
	/* need to reset for work */
	gpio_request(IOX_PHY1_RESET, "PHY1_RESET");
	gpio_direction_output(IOX_PHY1_RESET, 0);

	udelay(500);
	gpio_direction_output(IOX_PHY1_RESET, 1);

#if !defined(CONFIG_BOARD_1C)
	gpio_request(IOX_PHY2_RESET, "PHY2_RESET");
	gpio_direction_output(IOX_PHY2_RESET, 0);

	udelay(500);
	gpio_direction_output(IOX_PHY2_RESET, 1);
#endif

	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}
#endif

#ifdef CONFIG_USB_EHCI_MX6
#define USB_OTHERREGS_OFFSET	0x800
#define UCTRL_PWR_POL		(1 << 9)

static iomux_v3_cfg_t const usb_otg_pads[] = {
	MX6_PAD_GPIO1_IO00__ANATOP_OTG1_ID | MUX_PAD_CTRL(OTG_ID_PAD_CTRL),
	MX6_PAD_GPIO1_IO05__ANATOP_OTG2_ID | MUX_PAD_CTRL(OTG_ID_PAD_CTRL),
};

/* At default the 3v3 enables the MIC2026 for VBUS power */
static void setup_usb(void)
{
	imx_iomux_v3_setup_multiple_pads(usb_otg_pads,
					 ARRAY_SIZE(usb_otg_pads));
}

int board_usb_phy_mode(int port)
{
	if (port == 1)
		return USB_INIT_HOST;
	else
		return usb_phy_mode(port);
}

int board_ehci_hcd_init(int port)
{
	u32 *usbnc_usb_ctrl;

	if (port > 1)
		return -EINVAL;

	usbnc_usb_ctrl = (u32 *)(USB_BASE_ADDR + USB_OTHERREGS_OFFSET +
				 port * 4);

	/* Set Power polarity */
	setbits_le32(usbnc_usb_ctrl, UCTRL_PWR_POL);

	return 0;
}
#endif

int board_early_init_f(void)
{
	setup_iomux_uart();
	
	return 0;
}

/* add by Codebreaker for feed wdt */
#ifdef HW_ZLG_WATCHDOG
void wdt_init(void)
{
	gpio_request(IOX_WDT, "watchdog");
	gpio_direction_output(IOX_WDT, 0);
	hw_watchdog_reset();
}


void hw_watchdog_reset(void)
{
	static int value = 0;
	value = !value;
	gpio_set_value(IOX_WDT, value);
	udelay(1);
}
#endif
/* add end */

extern void print_hello(void);
int board_init(void)
{
	/* Address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	imx_iomux_v3_setup_multiple_pads(iox_pads, ARRAY_SIZE(iox_pads));

#ifdef HW_ZLG_WATCHDOG
	wdt_init();
#endif

#ifdef CONFIG_SYS_I2C_MXC
//	setup_i2c(0, CONFIG_SYS_I2C_SPEED, 0x7f, &i2c_pad_info1);
#endif

#ifdef	CONFIG_FEC_MXC
	setup_fec(CONFIG_FEC_ENET_DEV);
#endif

#ifdef CONFIG_SYS_USE_NAND
	setup_gpmi_nand();
#endif
#ifdef CONFIG_USB_EHCI_MX6
	setup_usb();
#endif

#ifdef CONFIG_FSL_QSPI
	board_qspi_init();
#endif

	return 0;
}

#ifdef CONFIG_CMD_BMODE
static const struct boot_mode board_boot_modes[] = {
	/* 4 bit bus width */
	/* modify by Codebreaker */
	{"sd1", MAKE_CFGVAL(0x40, 0x20, 0x00, 0x00)},
	{"qspi1", MAKE_CFGVAL(0x10, 0x00, 0x00, 0x00)},
	{"nand", MAKE_CFGVAL(0x91, 0x00, 0x00, 0x00)},
	{NULL,	 0},
};
#endif

int board_late_init(void)
{
#ifdef CONFIG_CMD_BMODE
	add_board_boot_modes(board_boot_modes);
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
	board_late_mmc_init();
#endif

	return 0;
}

u32 get_board_rev(void)
{
	return get_cpu_rev();
}

int checkboard(void)
{
#ifdef CONFIG_BOARD_1C
	puts("Board: EPC-M6G1C\n");
#else
	puts("Board: EPC-M6G2C\n");
#endif /* CONFIG_BOARD_M6G1C */

	return 0;
}

#ifdef CONFIG_FSL_FASTBOOT

void board_fastboot_setup(void)
{
	switch (get_boot_device()) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc0");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "booa mmc0");
		break;
	case SD2_BOOT:
	case MMC2_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "mmc1");
		if (!getenv("bootcmd"))
			setenv("bootcmd", "booa mmc1");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
#if defined(CONFIG_FASTBOOT_STORAGE_NAND)
	case NAND_BOOT:
		if (!getenv("fastboot_dev"))
			setenv("fastboot_dev", "nand");
		if (!getenv("fbparts"))
			setenv("fbparts", ANDROID_FASTBOOT_NAND_PARTS);
		if (!getenv("bootcmd"))
			setenv("bootcmd",
				"nand read ${loadaddr} ${boot_nand_offset} "
				"${boot_nand_size};booa ${loadaddr}");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_NAND*/

	default:
		printf("unsupported boot devices\n");
		break;
	}
}

#ifdef CONFIG_ANDROID_RECOVERY
int check_recovery_cmd_file(void)
{
	int recovery_mode = 0;

	recovery_mode = recovery_check_and_clean_flag();

	return recovery_mode;
}

void board_recovery_setup(void)
{
	int bootdev = get_boot_device();

	switch (bootdev) {
#if defined(CONFIG_FASTBOOT_STORAGE_MMC)
	case SD1_BOOT:
	case MMC1_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery", "booa mmc0 recovery");
		break;
	case SD2_BOOT:
	case MMC2_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery", "booa mmc1 recovery");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_MMC*/
#if defined(CONFIG_FASTBOOT_STORAGE_NAND)
	case NAND_BOOT:
		if (!getenv("bootcmd_android_recovery"))
			setenv("bootcmd_android_recovery",
				"nand read ${loadaddr} ${recovery_nand_offset} "
				"${recovery_nand_size};booa ${loadaddr}");
		break;
#endif /*CONFIG_FASTBOOT_STORAGE_NAND*/

	default:
		printf("Unsupported bootup device for recovery: dev: %d\n",
			bootdev);
		return;
	}

	printf("setup env for recovery..\n");
	setenv("bootcmd", "run bootcmd_android_recovery");
}
#endif /*CONFIG_ANDROID_RECOVERY*/

#endif /*CONFIG_FSL_FASTBOOT*/
