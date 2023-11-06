#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "nvs_flash.h" //non volatile storage
#include "esp_http_server.h"

static const int uart_num = UART_NUM_0;

static const int uart_baudrate = 115200;
static const uart_word_length_t uart_word_length = UART_DATA_8_BITS;
static const uart_parity_t uart_parity = UART_PARITY_DISABLE;
static const uart_stop_bits_t uart_stop_bits = UART_STOP_BITS_1;
static const uart_hw_flowcontrol_t uart_hw_flowcontrol = UART_HW_FLOWCTRL_DISABLE;
static const uart_sclk_t uart_sclk = UART_SCLK_APB;

static const uart_config_t uart_config = {
    .baud_rate = uart_baudrate,
    .data_bits = uart_word_length,
    .parity = uart_parity,
    .stop_bits = uart_stop_bits,
    .flow_ctrl = uart_hw_flowcontrol,
    .source_clk = uart_sclk,
};

static const char *log_tag = "uart_controller";

const char *ssid = "ESPAP";
const char *pass = "abc12345";

static const char *TAG = "wifi station";

static int retry_num = 0;

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_START)
    {
        ESP_LOGI(TAG, "Access Point started. SSID: %s, Password: %s\n", ssid, pass);
    }
}

void wifi_ap_setup()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_init_config);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK}};

    strcpy((char *)wifi_config.ap.ssid, ssid);
    strcpy((char *)wifi_config.ap.password, pass);

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
    esp_wifi_start();
}

/* An HTTP GET handler */
esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char *resp_str = "Hello, world!";
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

/* URI handler structure for GET /hello */
httpd_uri_t hello = {
    .uri = "/hello",
    .method = HTTP_GET,
    .handler = hello_get_handler,
    .user_ctx = NULL};

/* Function to start the web server */
httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        httpd_register_uri_handler(server, &hello);
        return server;
    }

    ESP_LOGE(TAG, "Error starting server!");
    return NULL;
}

/* Function to stop the web server */
void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void installUartDriver()
{
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(uart_num, 256, 0, 0, NULL, 0));
}

void app_main()
{
    nvs_flash_init();
    installUartDriver();
    wifi_ap_setup();

    // Start the web server
    httpd_handle_t server = start_webserver();

    // Check if the server started successfully
    if (server == NULL)
    {
        ESP_LOGE(TAG, "Failed to start web server!");
        return;
    }
    int64_t previousMillis = 0UL;
    int64_t interval = 1000000UL;

    while (1)
    {
        vTaskDelay(1);
    }
}