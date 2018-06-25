/*-----------------------------------------------------------------
 * lcd_setting.c -- use for support different lcd pannel on line.
 * Author:	huaqiyan@zlg.cn
 * Modify by Aaron, shenguiting@zlg.cn
 *----------------------------------------------------------------*/

#include "lcd_setting.h"

void do_enable_parallel_lcd(struct lcd_panel_info_t const *dev)
{
    enable_lcdif_clock(dev->lcdif_base_addr);
    imx_iomux_v3_setup_multiple_pads(lcd_pads, ARRAY_SIZE(lcd_pads));
}

struct lcd_panel_info_t displays[5] = {
	/* ZLG TFT-4.3 V1.01*/
    {
        .lcdif_base_addr = LCDIF1_BASE_ADDR,
        .depth = 16,
        .enable = do_enable_parallel_lcd,
        .mode = {
            .name = "TFT435Q",
            .xres = 480,
            .yres = 272,
            .pixclock = 111111, /* unit is ps, 1x10e-12s, the value is 9MHz      */
            .left_margin = 43,
            .right_margin = 8,
            .hsync_len = 4,
            .upper_margin = 8,
            .lower_margin = 8,
            .vsync_len = 6,
            .sync = 0,
            .vmode = FB_VMODE_NONINTERLACED
        }
    },
    /* ZLG LCD-640480W056TR V1.00, @TM057QDH01 */
    {
        .lcdif_base_addr = LCDIF1_BASE_ADDR,
        .depth = 16,
        .enable = do_enable_parallel_lcd,
        .mode = {
            .name = "TM057QDH01",
            .xres = 640,
            .yres = 480,
            .pixclock = 40000, /* unit is ps, 1x10e-12s, this value is 25MH     z */
            .left_margin = 100,
            .right_margin = 40,
            .hsync_len = 20,
            .upper_margin = 20,
            .lower_margin = 20,
            .vsync_len = 5,
            .sync = 0,
            .vmode = FB_VMODE_NONINTERLACED
        }
    },
    /* ZLG LCD_TM070RDH12_24B V1.11*/
    {
        .lcdif_base_addr = LCDIF1_BASE_ADDR,
        .depth = 16,
        .enable = do_enable_parallel_lcd,
        .mode = {
            .name = "TM070RDH",
            .xres = 800,
            .yres = 480,
            .pixclock = 33333, /* unit is ps, 1x10e-12s, the value is 30MHz      */
            .left_margin = 88,
            .right_margin = 40,
            .hsync_len = 48,
            .upper_margin = 32,
            .lower_margin = 13,
            .vsync_len = 3,
            .sync = 0,
            .vmode = FB_VMODE_NONINTERLACED
        }
    },
    /* ZLG LCD-800600W080TR V1.00 */
    {
        .lcdif_base_addr = LCDIF1_BASE_ADDR,
        .depth = 16,
        .enable = do_enable_parallel_lcd,
        .mode = {
            .name = "TM080SDH01",
            .xres = 800,
            .yres = 600,
            .pixclock = 25000, /* unit is ps, 1x10e-12s, this value is 40MH     z */
            .left_margin = 88,
            .right_margin = 112,
            .hsync_len = 48,
            .upper_margin = 39,
            .lower_margin = 21,
            .vsync_len = 3,
            .sync = 0,
            .vmode = FB_VMODE_NONINTERLACED
        }
    },
    /* customize panel at runtime */
    {
        .lcdif_base_addr = LCDIF1_BASE_ADDR,
        .depth = 16,
        .enable = do_enable_parallel_lcd,
        .mode = {
            .name = "override",
            .xres = 480,
            .yres = 272,
            .pixclock = 111111, /* unit is ps, 1x10e-12s, the value is 9MHz      */
            .left_margin = 43,
            .right_margin = 8,
            .hsync_len = 4,
            .upper_margin = 8,
            .lower_margin = 8,
            .vsync_len = 6,
            .sync = 0,
            .vmode = FB_VMODE_NONINTERLACED
        }
    }
};

struct lcd_panel_info_t *get_lcd_panels(void)
{
    return &displays[0];
}

/* override splash_screen_prepare() in common/splash.c */
int splash_screen_prepare(void)
{
    char *s = NULL;

    /* load bmp from Nand/SD to RAM */
    if((s = getenv("load_splash")) != NULL) {
        /*
         * load_splash='nand read.i $(splashimage) 0x100000' or
         * load_splash='fatload mmc 0:1 $(splashimage) logo.bmp'
         */
        run_command(s,0);
    }

    return 0;
}

void override_lcd_settings(void)
{
    struct lcd_panel_info_t *panel = NULL;
    char *s = NULL;
    unsigned long pxl_clk_hz = 0;
    int ii = ARRAY_SIZE(displays) - 1;

    /* get the last one, "override" */
    panel = &displays[ii];

    /* parse environment to get the override parameters */
    s = getenv("lcd_bpp");
    panel->depth = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_xres");
    panel->mode.xres = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_yres");
    panel->mode.yres = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_hfp");
    panel->mode.right_margin = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_hbp");
    panel->mode.left_margin = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_hsw");
    panel->mode.hsync_len = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_vfp");
    panel->mode.lower_margin = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_vbp");
    panel->mode.upper_margin = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_vsw");
    panel->mode.vsync_len = simple_strtoul (s, NULL, 0);

    s = getenv("lcd_pxl_clk");
    pxl_clk_hz = simple_strtoul (s, NULL, 0);
    panel->mode.pixclock = 1000000000000/pxl_clk_hz;

    debug("override lcd panel parameters:\n"
            "  pxl_ckl %dMHz\n"
            "  width   %d\n"
            "  height  %d\n"
            "  hbp     %d\n"
            "  hfp     %d\n"
            "  hsw     %d\n"
            "  vbp     %d\n"
            "  vfp     %d\n"
            "  vsw     %d\n",
            (int)pxl_clk_hz/1000000,
            panel->mode.xres,
            panel->mode.yres,
            panel->mode.left_margin,
            panel->mode.right_margin,
            panel->mode.hsync_len,
            panel->mode.upper_margin,
            panel->mode.lower_margin,
            panel->mode.vsync_len );

	return;
}
