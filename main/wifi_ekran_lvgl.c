#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "my_wifi_sta.h"
#include "my_gui.h"
#include "lvgl.h"
#include "lvgl_helpers.h"
#include "my_input_device.h"

#define LV_TICK_PERIOD_MS 1

static const char *TAG = "scan";
lv_obj_t *list;
lv_obj_t *BtnTara;
lv_obj_t *lblTara;
lv_obj_t *lblwifiCnt;

void wifi_tarama_print(void);
static void guiTask(void *pvParameter);
void ui_init(void);
static void lv_tick_task(void *arg);

static void ui_event_BtnTara(lv_event_t *e)
{
    lv_event_code_t event = lv_event_get_code(e);
    lv_obj_t *ta = lv_event_get_target(e);
    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_clean(list); // oluşturunlan objenin tüm alt nesnelerini temizlemek için kullanılır.
        wifi_tarama_print();
        printf("wifi tarama butonuna basıld.\n\r");
    }
}
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_scan();
    esp_wifi_scan_start(NULL, true);

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);

    my_input_device_init();
    xTaskCreatePinnedToCore(guiTask, "gui", 4096 * 2, NULL, 0, NULL, 1);
}
SemaphoreHandle_t xGuiSemaphore;
static void guiTask(void *pvParameter)
{

    (void)pvParameter;
    xGuiSemaphore = xSemaphoreCreateMutex();

    my_gui_init();

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    ui_init();

    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(5));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }

    /* A task should NEVER return */

    vTaskDelete(NULL);
}

static void lv_tick_task(void *arg)
{
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void ui_init(void)
{
    list = lv_list_create(lv_scr_act());
    lv_obj_set_size(list, 320, 200);
    lv_obj_align(list, LV_ALIGN_TOP_LEFT, 0, 0);

    // wifi_tarama_print();

    BtnTara = lv_btn_create(lv_scr_act());

    lv_obj_set_width(BtnTara, 152);
    lv_obj_set_height(BtnTara, 35);
    lv_obj_align(BtnTara, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(BtnTara, ui_event_BtnTara, LV_EVENT_ALL, NULL);

    lv_obj_add_flag(BtnTara, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_clear_flag(BtnTara, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(BtnTara, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(BtnTara, 255, LV_PART_MAIN | LV_STATE_PRESSED);
    /*     lv_obj_set_style_bg_color(BtnTara, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(BtnTara, 255, LV_PART_MAIN | LV_STATE_FOCUSED); */

    lblTara = lv_label_create(BtnTara);
    lv_obj_set_width(lblTara, LV_SIZE_CONTENT);
    lv_obj_set_height(lblTara, LV_SIZE_CONTENT);

    lv_obj_set_x(lblTara, 0);
    lv_obj_set_y(lblTara, 0);

    lv_obj_set_align(lblTara, LV_ALIGN_CENTER);

    lv_label_set_text(lblTara, "Wifi-Tara");
    //########################### lbl wifi sayac
    lblwifiCnt = lv_label_create(lv_scr_act());
    lv_obj_set_width(lblwifiCnt, LV_SIZE_CONTENT);
    lv_obj_set_height(lblwifiCnt, LV_SIZE_CONTENT);

    lv_obj_set_x(lblwifiCnt, 0);
    lv_obj_set_y(lblwifiCnt, 0);

    lv_obj_align(lblwifiCnt, LV_ALIGN_BOTTOM_RIGHT, -10, 0);

    lv_label_set_text(lblwifiCnt, "0");

    lv_obj_set_style_text_color(lblTara, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lblTara, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lblTara, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void wifi_tarama_print(void)
{

    for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
    {
        if (ap_info[i].rssi != 0)
        {
            char buf[32];
            lv_snprintf(buf, sizeof(buf), "%d  %s", ap_info[i].rssi, ap_info[i].ssid);
            lv_list_add_btn(list, LV_SYMBOL_WIFI, buf);
            ESP_LOGI(TAG, "%d..SSID \t\t%s----db.:%d", i, ap_info[i].ssid, ap_info[i].rssi);
            lv_label_set_text_fmt(lblwifiCnt, "%d", i);
        }
    }
}