/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp8266/spi_struct.h"
#include "driver/gpio.h"
#include "driver/spi.h"
#include "esp_partition.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lv_port_disp.h"
#include "image.h"
#include "lcd.h"
// #include "st7735s.h"

static const char* TAG = "main";

/**********************
 *  STATIC PROTOTYPES
 **********************/
// static void lv_tick_task(void *arg);
// static void gui_task(void *arg);
// static void create_demo_application(void);

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;

/**
 * @brief  espx gpio管脚初始化
 * @note
 * @param  None
 * @retval None
 */
void espx_gpio_init(void)
{
#define GPIO_OUTPUT_PIN_SEL(_x) ((1ULL << (_x)))
#define __ESPX_GPIOX_INIT(_pin, _mode, _itype, _up, _down, _mask) \
    {                                                             \
        .pin = _pin,                                              \
        {                                                         \
            .mode = _mode,                                        \
            .intr_type = _itype,                                  \
            .pull_up_en = _up,                                    \
            .pull_down_en = _down,                                \
            .pin_bit_mask = _mask,                                \
        },                                                        \
    }

    typedef struct espx_gpio esp_gpio;
    typedef struct espx_gpio *esp_gpio_t;
    struct espx_gpio
    {
        gpio_num_t pin;
        gpio_config_t cfg;
    };

    esp_gpio gpio_table[] = {
        __ESPX_GPIOX_INIT(GPIO_NUM_16, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE,
                          GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE,
                          GPIO_OUTPUT_PIN_SEL(GPIO_NUM_16)),
        __ESPX_GPIOX_INIT(LCD_RESET_PIN, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE,
                          GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE,
                          GPIO_OUTPUT_PIN_SEL(LCD_RESET_PIN)),
        __ESPX_GPIOX_INIT(LCD_DC_PIN, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE,
                          GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE,
                          GPIO_OUTPUT_PIN_SEL(LCD_DC_PIN)),
        __ESPX_GPIOX_INIT(LCD_BLK_PIN, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE,
                          GPIO_PULLUP_DISABLE, GPIO_PULLDOWN_DISABLE,
                          GPIO_OUTPUT_PIN_SEL(LCD_BLK_PIN)),
    };

    for (esp_gpio_t p = gpio_table; p &&
                                    p->pin < GPIO_NUM_MAX &&
                                    p < gpio_table + sizeof(gpio_table) / sizeof(gpio_table[0]);
         ++p)
    {
        /*configure GPIO with the given settings*/
        gpio_config(&p->cfg);
    }
#undef GPIO_OUTPUT_PIN_SEL
#undef __ESPX_GPIOX_INIT
}

/**
 * @brief  espx spi_master参数配置
 * @note
 * @param  None
 * @retval None
 */
void espx_spi_init(void)
{
    ESP_LOGI("spi", "init");
    spi_config_t spi_config = {
        // CS_EN:1, MISO_EN:0, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:1, CPOL:1
        .interface.val = 0x143,
        // .interface = {
        //     .cs_en = 1,
        //     .miso_en = 0,
        //     .mosi_en = 1,
        //     .cpha = 1,
        //     .cpol = 1,
        // },
        // Set SPI to master mode
        // ESP8266 Only support half-duplex
        .mode = SPI_MASTER_MODE,
        // TRANS_DONE: true, WRITE_STATUS: false, READ_STATUS: false, WRITE_BUFFER: false, READ_BUFFER: false
        .intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE,
        // Set the SPI clock frequency division factor
        .clk_div = SPI_40MHz_DIV,
        // Register SPI event callback function
        .event_cb = NULL,
    };
    // spi_config.interface.bit_tx_order = 1;
    // spi_config.interface.byte_tx_order = 1;
    spi_init(HSPI_HOST, &spi_config);
}

static void lv_tick_task(void *arg)
{
    (void)arg;

    lv_tick_inc(1U);
}



/**
 * @brief  lvgl 图形刷新任务
 * @note
 * @param  None
 * @retval None
 */
static void gui_task(void *arg)
{
    (void)arg;
    xGuiSemaphore = xSemaphoreCreateMutex();

    LCD_Init(LCD_NUMBER, LCD_ORIENTATION_PORTRAIT);
    // LCD_DisplayOn(0);
    SPI_LCD_BL(GPIO_PIN_SET);

    // vTaskDelay(1000 / portTICK_RATE_MS);
    // LCD_DrawBitmap(LCD_NUMBER, 0, 0, 40, 40, (uint8_t *)gImage_qq);

    // st7735s_init();
    // SPI_LCD_BL(GPIO_PIN_SET);
    // lv_area_t area = {
    //     .x1 = 0,
    //     .x2 = 40,
    //     .y1 = 0,
    //     .y2 = 40,
    // };
    // extern uint8_t qq_map[];
    // st7735s_flush(NULL, &area, (lv_color_t *)gImage_qq); // gImage_qq
    // extern void ST7735_DrawBitmap(uint32_t Xpos,
    //                               uint32_t Ypos,
    //                               uint32_t width,
    //                               uint32_t height,
    //                               uint8_t * pBmp);
    // ST7735_DrawBitmap(area.x1, area.y1, area.x2, area.y2, gImage_qq);

    lv_init();
    /*lvgl display interface initialization, placed after lv_init()*/
    lv_port_disp_init();
    /*lvgl input interface initialization, placed after lv_init()*/
    // lv_port_indev_init();

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui",
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 1000U));
    // esp_register_freertos_tick_hook(lv_tick_task);

    /* Create the demo application */
    // create_demo_application();
    ESP_LOGI("GUI", "gui task is running.\n");

    // lv_get_started_1();

    static lv_obj_t *img_bg = NULL;
    LV_IMG_DECLARE(qq);
    img_bg = lv_img_create(lv_scr_act());
    lv_img_set_src(img_bg, &qq);
    lv_obj_align(img_bg, LV_ALIGN_CENTER, 0, 0);
    lv_img_set_zoom(img_bg, 400);

    // extern void lv_example_btn_1(void);
    // lv_example_btn_1();

    for (;;)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        // vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

// static void gpio_task(void *arg)
// {

//     for (;;)
//     {
//     }
// }

#define GPIO_OUTPUT_IO_LED GPIO_NUM_16
// #define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_OUTPUT_IO_LED))
void app_main()
{
    bool state = false;
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("\t\tESP8266\ncores\tvision\tsize(MB)\ttype\n-----\t------\t--------\t---------\n%d\t%d\t%d\t\t%s\n",
           chip_info.cores, chip_info.revision, spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    // gpio_config_t io_conf = {
    //     .mode = GPIO_MODE_OUTPUT,            // set as output mode
    //     .intr_type = GPIO_INTR_DISABLE,      // disable interrupt
    //     .pin_bit_mask = GPIO_OUTPUT_PIN_SEL, // bit mask of the pins that you want to set,e.g.GPIO15/16
    //     .pull_down_en = 0,                   // disable pull-down mode
    //     .pull_up_en = 0,                     // disable pull-up mode
    // };
    // // configure GPIO with the given settings
    // gpio_config(&io_conf);
    espx_gpio_init();
    espx_spi_init();
    // ESP_ERROR_CHECK(nvs_flash_init());
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_LOGW(TAG, "nvs_flash_init failed (0x%x), erasing partition and retrying", err);
        const esp_partition_t* nvs_partition = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        assert(nvs_partition && "partition table must have an NVS partition");
        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    tcpip_adapter_init();
    // extern void wifi_init();
    // wifi_init();
    extern void littlefs_init(void);
    littlefs_init();
    extern void smartconfig_wifi(void);
    smartconfig_wifi();
    extern void console_init();
    console_init();
    // LCD_Init(LCD_NUMBER, LCD_ORIENTATION_PORTRAIT);
    // LCD_DisplayOn(0);
    // SPI_LCD_BL(GPIO_PIN_SET);

    // vTaskDelay(1000 / portTICK_RATE_MS);
    // LCD_DrawBitmap(LCD_NUMBER, 20, 100, 16, 32, (uint8_t *)gImage_1);
    // start gpio task
    // xTaskCreate(gpio_task, "gpio_task_example", 2048, NULL, 10, NULL);

    /* If you want to use a task to create the graphic, you NEED to create a Pinned task
     * Otherwise there can be problem such as memory corruption and so on.
     * NOTE: When not using Wi-Fi nor Bluetooth you can pin the guiTask to core 0 */
    // xTaskCreatePinnedToCore(gui_task, "gui", 4096 * 2, NULL, 0, NULL, 1);
    xTaskCreate(gui_task, "gui", 2048, NULL, 0, NULL);

    for (;;)
    {
        state ^= true;
        gpio_set_level(GPIO_OUTPUT_IO_LED, state);
        vTaskDelay(1000 / portTICK_RATE_MS);
        // printf("free heap: %u.\r\n", esp_get_free_heap_size());
    }
}
