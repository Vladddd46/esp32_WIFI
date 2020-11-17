#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define USER_SSID              "ПРИВЕТ_ЮРА"
#define USER_PASSWORD          "qwerty1234"
#define WIFI_CHANNEL           1
#define MAX_NUM_OF_CONNECTIONS 10

void wifi_init_ap(void);
