#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <asm/gpio.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/gpio.h>
#include <linux/pinctrl/pinctrl.h>

#define IMX_GPIO_NR(bank, nr)       (((bank) - 1) * 32 + (nr))

struct button_data {
	char *name; //按键名称
	unsigned long pin;//GPIO管教
	int code; //按键值
    int defineValue;
};

static struct button_data button_desc[] = {
	{ "KEY_1", IMX_GPIO_NR(3,4),  KEY_1, 1},
	{ "KEY_2", IMX_GPIO_NR(5,6),  KEY_2, 1},
	{ "KEY_3", IMX_GPIO_NR(1,22), KEY_3, 1},
	{ "KEY_4", IMX_GPIO_NR(1,23), KEY_4, 1},
	{ "KEY_5", IMX_GPIO_NR(1,26), KEY_5, 1},
	{ "KEY_6", IMX_GPIO_NR(1,27), KEY_6, 1},
};

static struct input_dev *button_dev;
static struct timer_list g_timer;

void report_value(void)
{
    int i;

    int keyvalue;

	for (i = 0; i < ARRAY_SIZE(button_desc); i++) 
    {
       keyvalue  = gpio_get_value(button_desc[i].pin);
       if(keyvalue != button_desc[i].defineValue)
       {
          //printk("key event:[key, value](%d , %d)\n", button_desc[i].pin, keyvalue);
          // input_report_key(button_dev, button_desc[i].code, 1);
           input_event(button_dev,EV_KEY, button_desc[i].code, 1);
           input_sync(button_dev);
          // input_report_key(button_dev, button_desc[i].code, 0);
           input_event(button_dev,EV_KEY, button_desc[i].code, 0);
           input_sync(button_dev);
       }
    }


}

void f_timer_timeout(unsigned long arg)
{
    report_value();
    mod_timer(&g_timer, jiffies+HZ/10);
}


static void button_init_timer(void)
{
    printk("%s:%d\n",__FUNCTION__, __LINE__ );

    init_timer(&g_timer);
    g_timer.function = f_timer_timeout;
    add_timer(&g_timer);
    mod_timer(&g_timer, jiffies+HZ/1);
}


static int button_init(void)
{
    int i, ret;

    printk("%s:%d\n",__FUNCTION__, __LINE__ );

    /*input_dev*/
    button_dev = input_allocate_device();

    /*设置事件*/
    set_bit(EV_KEY, button_dev->evbit); //按键类事件
    set_bit(EV_REP, button_dev->evbit); //重复类事件

    /*设置支持键值*/
    for (i = 0; i < ARRAY_SIZE(button_desc); i++) {
        printk("add key:%s, %d\n",button_desc[i].name, button_desc[i].code);
        set_bit(button_desc[i].code, button_dev->keybit);
    }

    button_dev->name = "my_button";

    ret = input_register_device(button_dev);

    /*request io */
    for (i = 0; i < ARRAY_SIZE(button_desc); i++) {
        ret = gpio_request(button_desc[i].pin, button_desc[i].name);
        if(ret)
            printk("add %s, %ld failed\n", button_desc[i].name, button_desc[i].pin);
    }

    //init timer
    button_init_timer();

    return 0;
}

static void button_exit(void)
{
    int i;
    printk("%s:%d\n",__FUNCTION__, __LINE__ );

    del_timer(&g_timer);

    for (i = 0; i < ARRAY_SIZE(button_desc); i++) {
        gpio_free(button_desc[i].pin);
    }

    input_unregister_device(button_dev);
    input_free_device(button_dev);
}

module_init(button_init);
module_exit(button_exit);
MODULE_LICENSE("GPL");
