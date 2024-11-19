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

#include "esp_err.h"
#include "esp_netif.h"

#define MY_CONNECTION_WIFI_SSID "wifi_name"
#define MY_CONNECTION_WIFI_PASSWORD "wifi_password"
#define MY_CONNECTION_SCAN_METHOD WIFI_ALL_CHANNEL_SCAN
#define MY_CONNECTION_CONNECT_AP_SORT_METHOD WIFI_CONNECT_AP_BY_SIGNAL
#define MY_CONNECTION_WIFI_SCAN_RSSI_THRESHOLD -127
#define MY_CONNECTION_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#define NR_OF_IP_ADDRESSES_TO_WAIT_FOR (s_active_interfaces)

#define DEFAULT_SCAN_LIST_SIZE 50
uint16_t number;
uint16_t ap_count;
wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];

esp_err_t my_wifi_sta_init(void);
void wifi_scan(void);