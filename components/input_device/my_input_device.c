#include "esp_log.h"
#include "driver/gpio.h"

#include "my_input_device.h"

void my_input_device_init(void)
{
    gpio_reset_pin(INPUT_GPIO1);
    gpio_set_direction(INPUT_GPIO1, GPIO_MODE_DEF_INPUT);

    gpio_reset_pin(INPUT_GPIO2);
    gpio_set_direction(INPUT_GPIO2, GPIO_MODE_DEF_INPUT);

    gpio_reset_pin(INPUT_GPIO3);
    gpio_set_direction(INPUT_GPIO3, GPIO_MODE_DEF_INPUT);

    gpio_reset_pin(INPUT_GPIO4);
    gpio_set_direction(INPUT_GPIO4, GPIO_MODE_DEF_INPUT);

}