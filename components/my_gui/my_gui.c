#include "my_gui.h"
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "my_input_device.h"

lv_indev_t *my_indev;
lv_group_t *g;

#define TAG "my_gui"

static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static uint32_t keypad_get_key(void);

void my_gui_init(void)
{

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    static lv_disp_draw_buf_t disp_buf;

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[320 * 10];
    static lv_color_t buf_2[320 * 10];

    /*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
    lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, 320 * 10);

    static lv_disp_drv_t disp_drv;         /*A variable to hold the drivers. Must be static or global.*/
    lv_disp_drv_init(&disp_drv);           /*Basic initialization*/
    disp_drv.draw_buf = &disp_buf;         /*Set an initialized buffer*/
    disp_drv.flush_cb = disp_driver_flush; /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = 320;                /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = 240;                /*Set the vertical resolution in pixels*/

    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);         /*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_KEYPAD; /*See below.*/
    indev_drv.read_cb = keypad_read;       /*See below.*/

    /*Register the driver in LVGL and save the created input device object*/
    my_indev = lv_indev_drv_register(&indev_drv);
    g = lv_group_create();
    lv_indev_set_group(my_indev, g);
    lv_group_set_default(g); // bütün nesleri otomatik olarak sisteme dahil eder.
}

static uint32_t keypad_get_key(void)
{
    int durum = 0;
    if (gpio_get_level(INPUT_GPIO1) == 1)
    {
        ESP_LOGI(TAG, "LOGIC SEVIYE 1.:%s", gpio_get_level(INPUT_GPIO1) == true ? "YUKSEK" : "DUSUK");
        durum = 1;
    }
    if (gpio_get_level(INPUT_GPIO2) == 1)
    {
        ESP_LOGI(TAG, "LOGIC SEVIYE 2.:%s", gpio_get_level(INPUT_GPIO2) == true ? "YUKSEK" : "DUSUK");
        durum = 2;
    }
    if (gpio_get_level(INPUT_GPIO3) == 1)
    {
        ESP_LOGI(TAG, "LOGIC SEVIYE 3.:%s", gpio_get_level(INPUT_GPIO2) == true ? "YUKSEK" : "DUSUK");
        durum = 3;
    }

    if (gpio_get_level(INPUT_GPIO4) == 1)
    {
        ESP_LOGI(TAG, "LOGIC SEVIYE 4.:%s", gpio_get_level(INPUT_GPIO2) == true ? "YUKSEK" : "DUSUK");
        durum = 4;
    }

    // ESP_LOGI(TAG, "buton durumu..:%d", durum);
    return durum;
}
static void mouse_get_xy(lv_coord_t *x, lv_coord_t *y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}
static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if (act_key != 0)
    {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch (act_key)
        {
        case 1:
            act_key = LV_KEY_NEXT;
            ESP_LOGI(TAG, "buton durumu..LV_KEY_UP");
            break;
        case 2:
            act_key = LV_KEY_PREV;
            ESP_LOGI(TAG, "buton durumu..LV_KEY_DOWN");
            break;
        case 3:
            act_key = LV_KEY_ENTER;
            ESP_LOGI(TAG, "buton durumu..LV_KEY_ENTER");
            break;
        case 4:
            act_key = LV_KEY_ESC;
            ESP_LOGI(TAG, "buton durumu..LV_KEY_ESC");
            break;
        case 5:
            act_key = LV_KEY_END;
            ESP_LOGI(TAG, "buton durumu..LV_KEY_END");
            break;
        }

        last_key = act_key;
    }
    else
    {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
}