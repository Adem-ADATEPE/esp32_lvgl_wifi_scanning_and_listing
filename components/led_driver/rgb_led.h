/**
  ******************************************************************************

  * Copyright (C) ADEM ADATEPE - All Rights Reserved
  *
  * This program and the accompanying materials are made available
  * under the terms described in the LICENSE file which accompanies
  * this distribution.
  * Written by ADEM ADATEPE
  ******************************************************************************
  */
// RGN  LED GPIOS
#define RGB_LED_RED_GPIO 13
#define RGB_LED_GREEN_GPIO 13
#define RGB_LED_BLUE_GPIO 13

#define RGB_LED_CHANNEL_NUM 3

typedef struct
{
    int channel;
    int gpio;
    int mode;
    int timer_index;
} ledc_info_t;

ledc_info_t ledc_ch[RGB_LED_CHANNEL_NUM];

/*
color to indicate wifi application has started.
*/
void rgb_led_pwm_init(void);

void rgb_led_set_color(uint8_t red, uint8_t green, uint8_t blue);
void rgb_led_flash(void);