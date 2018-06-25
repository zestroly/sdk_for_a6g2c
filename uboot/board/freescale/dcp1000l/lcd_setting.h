#ifndef LCD_SETTING_H
#define LCD_SETTING_H

#include <linux/fb.h>
#include <asm/arch/clock.h>
#include <asm/imx-common/iomux-v3.h> 
#include <asm/arch/imx-regs.h>

struct lcd_panel_info_t {
    unsigned int lcdif_base_addr;
    int depth;
    void (*enable)(struct lcd_panel_info_t const *dev);
    struct fb_videomode mode;
};

extern iomux_v3_cfg_t const lcd_pads[23];
extern struct lcd_panel_info_t displays[5];
extern struct lcd_panel_info_t *get_lcd_pannels(void);
extern int splash_screen_prepare(void);
extern void override_lcd_settings(void);
extern void do_enable_parallel_lcd(struct lcd_panel_info_t const *dev);

#endif
