/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"

#if 1
/* The examples use smartconfig type that you can set via project configuration menu.

   If you'd rather not, just change the below entries to enum with
   the config you want - ie #define EXAMPLE_ESP_SMARTCOFNIG_TYPE SC_TYPE_ESPTOUCH
*/
#define EXAMPLE_ESP_SMARTCOFNIG_TYPE      SC_TYPE_ESPTOUCH_V2
#define EXAMPLE_ESP_MAXIMUM_RETRY 3

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
// static const int CONNECTED_BIT = BIT0;
// static const int ESPTOUCH_DONE_BIT = BIT1;
#define CONNECTED_BIT BIT0
#define ESPTOUCH_DONE_BIT BIT1
#define WIFI_FAIL_BIT BIT2
static const char* TAG = "smartconfig";
static int s_retry_num = 0;

static void smartconfig_task(void* parm);

static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
#define WIFI_NVS_NAME "wifi"
#define WIFI_NVS_KEY "wifi_key"
    nvs_handle wifi_nvs_handle;
    wifi_config_t wifi_config;
    size_t len = sizeof(wifi_config_t);
    esp_err_t err;
    // esp_wifi_set_storage(WIFI_STORAGE_FLASH);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        // xTaskCreate(smartconfig_task, "smartconfig_task", 1024, NULL, 3, NULL);

        bzero(&wifi_config, sizeof(wifi_config_t));
        ESP_ERROR_CHECK(nvs_open(WIFI_NVS_NAME, NVS_READWRITE, &wifi_nvs_handle));
        err = nvs_get_blob(wifi_nvs_handle, WIFI_NVS_KEY, &wifi_config, &len);//读取wifi配置信息
        if (err == ESP_ERR_NVS_NOT_FOUND){ //读取失败)
            xTaskCreate(smartconfig_task, "smartconfig_task", 1024, NULL, 3, NULL);
        }
        else if (ESP_OK == err){
            ESP_LOGI(TAG, "already config.");
            ESP_ERROR_CHECK(esp_wifi_disconnect());
            ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config))
            esp_wifi_connect();
            nvs_close(wifi_nvs_handle);//关闭nvs操作句柄
        }
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
       } else { 
            ESP_ERROR_CHECK(nvs_open(WIFI_NVS_NAME, NVS_READWRITE, &wifi_nvs_handle));
            ESP_ERROR_CHECK(nvs_erase_key(wifi_nvs_handle, WIFI_NVS_KEY));
            usleep(5000);
            // ESP_ERROR_CHECK( nvs_erase_all(wifi_nvs_handle));
            // usleep(5000);
            ESP_ERROR_CHECK( nvs_commit(wifi_nvs_handle) );
            ESP_LOGI(TAG, "Erase WiFi information.");
            // xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        tcpip_adapter_ip_info_t ip;
        memset(&ip, 0, sizeof(tcpip_adapter_ip_info_t));
        if (tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_ETH, &ip) == 0) {
            ESP_LOGI(TAG, "~~~~~~~~~~~");
            ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip.ip));
            ESP_LOGI(TAG, "ETHPMASK:" IPSTR, IP2STR(&ip.netmask));
            ESP_LOGI(TAG, "ETHPGW:" IPSTR, IP2STR(&ip.gw));
            ESP_LOGI(TAG, "~~~~~~~~~~~");
        }
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE)
    {
        ESP_LOGI(TAG, "Scan done");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL)
    {
        ESP_LOGI(TAG, "Found channel");
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD)
    {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t* evt = (smartconfig_event_got_ssid_pswd_t*)event_data;
        // uint8_t ssid[33] = { 0 };
        // uint8_t password[65] = { 0 };
        uint8_t rvd_data[33] = { 0 };

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;

        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        // memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        // memcpy(password, evt->password, sizeof(evt->password));
        // ESP_LOGI(TAG, "SSID:%s", ssid);
        // ESP_LOGI(TAG, "PASSWORD:%s", password);

        ESP_LOGI(TAG, "SSID:%s", evt->ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", evt->password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK( esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)) );
            ESP_LOGI(TAG, "RVD_DATA:%s", rvd_data);
        }
        /*将获取的密码保存起来 nvs分区*/
        ESP_ERROR_CHECK( nvs_open( WIFI_NVS_NAME, NVS_READWRITE, &wifi_nvs_handle) );//打开新的nvs_flash
        ESP_ERROR_CHECK( nvs_set_blob( wifi_nvs_handle, WIFI_NVS_KEY, &wifi_config, sizeof(wifi_config)) );//将smartconfig获取到的wifi配置信息保存到外部flash
        ESP_ERROR_CHECK( nvs_commit(wifi_nvs_handle) );//提交保存信息
        nvs_close(wifi_nvs_handle);

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
    else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE)
    {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

void smartconfig_wifi(void)
{
    // tcpip_adapter_init();
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void smartconfig_task(void* parm)
{
    EventBits_t uxBits;
// __try:
//     uxBits = 0;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(EXAMPLE_ESP_SMARTCOFNIG_TYPE));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, 
                                     true, false, portMAX_DELAY);

        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
        }

        if (uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            esp_smartconfig_stop();
            vTaskDelete(NULL);
        }

        // if (uxBits & WIFI_FAIL_BIT) {
        //     ESP_LOGI(TAG, "WiFi Connected fail.");
        //     goto __try;
        // }
    }
}

// void app_main()
// {
//     ESP_ERROR_CHECK(nvs_flash_init());
//     initialise_wifi();
// }
#endif
