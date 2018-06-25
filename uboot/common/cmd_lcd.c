/*
 * cmd_lcd.c
 *
 *  Created on: 2015-11-06
 *      Author: huaqiyan@zlg.cn
 */

#include <config.h>
#include <common.h>
#include <command.h>
#include <stdarg.h>
#include <linux/types.h>
#include <stdio_dev.h>
#include <asm/gpio.h>
#include <lcd.h>
#include <linux/fb.h>

/*
 * LCD back light controlled by GPIO3_23
 * gpio = bankx32+offset = (3-1)x32 + 23 = 87
 */
#define BACKLIGHT_GPIO 87

struct lcd_panel_info_t {
	unsigned int lcdif_base_addr;
	int depth;
	void (*enable)(struct lcd_panel_info_t const *dev);
	struct fb_videomode mode;
};

extern struct lcd_panel_info_t* get_lcd_panels(void);

void cmd_lcd_info(void)
{
	struct lcd_panel_info_t* panel = get_lcd_panels();
	int ii = 0;

	printf("current build-in supported LCD panels:\n");
	for(ii = 1; ; ii++, panel++) {
		if (!strcmp(panel->mode.name, "override"))
			break;
		else {
			printf("  %02d %dx%d %dbpp name=\'%s\'\n",
					ii, panel->mode.xres, panel->mode.yres, panel->depth, panel->mode.name);
		}
	}

	printf("\n");
	return;
}

void cmd_lcd_select(int index)
{
	printf("the u-boot support selecting LCD panel by overriding build-in parameters.\n");
	printf("override panel's parameters step one by one:\n"
			"  1. set 'panel' value equal 'override'.\n"
			"  2. set 'lcd_xxx' variables value for the panel will be used.\n"
			"  3. save environment, the settings will efficient in next boot.\n");
	printf("Note:\n"
			"  if 'panel' equal build-in panels' name, it will be selected,\n");
	printf("the value of the name indicate to select a panel in the table displayed\n"
			"by command 'lcd info'.\n\n");

   return;
}

static void backlight_turn_on(void)
{
	if(!gpio_request(BACKLIGHT_GPIO, "")) {
		gpio_direction_output(BACKLIGHT_GPIO, 0);
		gpio_set_value(BACKLIGHT_GPIO, 1);
	}
	return;
}

static void backlight_turn_off(void)
{
	if(!gpio_request(BACKLIGHT_GPIO, "")) {
		gpio_direction_output(BACKLIGHT_GPIO, 0);
		gpio_set_value(BACKLIGHT_GPIO, 0);
	}
	return;
}

int do_lcd_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	char *cmd = NULL;
	char *s = NULL;
	int ii = 0;

	/* at least two arguments please */
	if (argc < 2)
		return cmd_usage(cmdtp);

	cmd = argv[1];

	if(strcmp(cmd, "info") == 0) {
		cmd_lcd_info();
		return 0;
	}

	if(strcmp(cmd, "select") == 0) {
		/*
		s = argv[2];
		ii = simple_strtoul (s, NULL, 10);
		*/
		cmd_lcd_select(ii);
		return 0;
	}

	if(strcmp(cmd, "backlight") == 0) {
		s = argv[2];
		if(strcmp(s, "on") == 0)
			backlight_turn_on();
		else if(strcmp(s, "off") == 0)
			backlight_turn_off();
		else
			printf("unknown command, except on/off.\n");
		return 0;
	}

	return 0;
}

U_BOOT_CMD(
    lcd,	3,	1,	do_lcd_cmd,
    "lcd subsystem",
    "backlight [on][off] - turn on or turn off the lcd backlight.\n"
    "info - show available LCD panels.\n"
    "select - comments for selecting a panel from available LCD panels."
);
