/*========== Includes =======================================================*/

#include "rest_api_controller.h"

#include "driver/twai.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "nvs_flash.h" //non volatile storage

#include "controllers/fsm_controller/fsm_controller.h"
#include "data/can_repository/can_repository.h"

/*========== Macros and Definitions =========================================*/

#define JSON_CAN_MESSAGES_SIZE 100

/*========== Static Constant and Variable Definitions =======================*/

static const char *log_tag = "rest_api_controller";

static const char *ssid = "CAN-TO-GO";
static const char *pass = "SpalterNr1";

static struct CanMessage json_can_messages[JSON_CAN_MESSAGES_SIZE];

/*========== Static Function Prototypes =====================================*/

/*========== Static Function Implementations ================================*/

static void update_can_messages()
{
    twai_message_t twai_message = {0};
    struct CanMessage received_can_message = {0, twai_message};
    can_repository_received_message_to_rest_api(&received_can_message);

    if (received_can_message.micros != 0)
    {
        // Check if the array is already full
        if (json_can_messages[JSON_CAN_MESSAGES_SIZE - 1].micros != 0)
        {
            // Array is full, shift messages to make room for the new one
            for (int i = 0; i < JSON_CAN_MESSAGES_SIZE - 1; i++)
            {
                json_can_messages[i] = json_can_messages[i + 1];
            }
            // Insert the latest message at the last index
            json_can_messages[JSON_CAN_MESSAGES_SIZE - 1] = received_can_message;
        }
        else
        {
            // Array is not full, find the first empty slot and insert the message
            for (int i = 0; i < JSON_CAN_MESSAGES_SIZE; i++)
            {
                if (json_can_messages[i].micros == 0)
                {
                    json_can_messages[i] = received_can_message;
                    break;
                }
            }
        }
    }
}

static void build_json_string(char *json_string)
{
    json_string[0] = '\0';
    strcat(json_string, "{\"messages\" : [");
    for (int i = 0; i < JSON_CAN_MESSAGES_SIZE; ++i)
    {
        char data_str[50];
        size_t index = 0;

        for (size_t data_index = 0; data_index < TWAI_FRAME_MAX_DLC; ++data_index)
        {
            if (data_index == 0)
            {
                index += sprintf(data_str + index, "[");
            }
            index += sprintf(data_str + index, "%u", (unsigned int)json_can_messages[i].message.data[data_index]);
            if (data_index < TWAI_FRAME_MAX_DLC - 1)
            {
                index += sprintf(data_str + index, ",");
            }
            if (data_index == TWAI_FRAME_MAX_DLC - 1)
            {
                index += sprintf(data_str + index, "]");
            }
        }
        char temp_str[250]; // temporary string buffer
        sprintf(temp_str, "{\"micros\": %lld, \"id\": %ld, \"ext\": %d, \"dlc\": %d, \"data\": %s}", json_can_messages[i].micros, json_can_messages[i].message.identifier, json_can_messages[i].message.extd, json_can_messages[i].message.data_length_code, data_str);
        strcat(json_string, temp_str);
        if (i < JSON_CAN_MESSAGES_SIZE - 1)
        {
            strcat(json_string, ",");
        }
    }
    strcat(json_string, "]}");
}

static esp_err_t hello_get_handler(httpd_req_t *req)
{
    const char *resp_str = "Hello, world!";
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

static esp_err_t can_get_handler(httpd_req_t *req)
{
    char *json_string = (char *)malloc(JSON_CAN_MESSAGES_SIZE * 250);
    build_json_string(json_string);

    httpd_resp_send(req, json_string, strlen(json_string));
    free(json_string);
    return ESP_OK;
}

static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_START)
    {
        ESP_LOGI(log_tag, "Access Point started. SSID: %s, Password: %s\n", ssid, pass);
    }
}

static void wifi_ap_setup()
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    wifi_init_config.wifi_task_core_id = 1;
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

/* Function to start the web server */
static httpd_handle_t start_webserver(void)
{
    /* URI handler structure for GET /hello */
    static httpd_uri_t get_hello = {
        .uri = "/hello",
        .method = HTTP_GET,
        .handler = hello_get_handler,
        .user_ctx = NULL};

    static httpd_uri_t get_can = {
        .uri = "/can",
        .method = HTTP_GET,
        .handler = can_get_handler,
        .user_ctx = NULL};

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.task_priority = configMAX_PRIORITIES - 10;
    config.core_id = 1;
    httpd_handle_t server = NULL;

    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Set URI handlers
        httpd_register_uri_handler(server, &get_hello);
        httpd_register_uri_handler(server, &get_can);
        return server;
    }

    ESP_LOGE(log_tag, "Error starting server!");
    return NULL;
}

static void rest_api_controller_task_handler()
{
    nvs_flash_init();
    wifi_ap_setup();

    // Start the web server
    httpd_handle_t server = start_webserver();

    // Check if the server started successfully
    if (server == NULL)
    {
        ESP_LOGE(log_tag, "Failed to start web server!");
    }

    while (1)
    {
        switch (fsm_controller_current_state)
        {
        case STARTING:
            break;
        case CONFIGURATION:
            break;
        case OPERATION:
            update_can_messages();
            break;
        }

        vTaskDelay(1);
    }
}

/*========== Extern Function Implementations ================================*/
extern void rest_api_controller_create_task()
{
    xTaskCreate(rest_api_controller_task_handler, "rest_api_controller", 4096, NULL, 1, NULL);
}