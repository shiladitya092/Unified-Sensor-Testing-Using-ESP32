#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "http_sender.h"

bool HttpSender::init(void *sender_options)
{
    http_sender_opts_t *http_opts = (http_sender_opts_t *)sender_options;

    sender_options_json = http_opts->sender_options_json;
    return true;
}

void HttpSender::newSenderInitOptions(void **sender_init_opts, JsonObject &json_data)
{
    *sender_init_opts = calloc(1, sizeof(http_sender_opts_t));
    http_sender_opts_t *http_sender_opts = (http_sender_opts_t *)*sender_init_opts;
    if (!http_sender_opts)
    {
        ESP_LOGD(TAG, "Error creating Http Sender options!\n");
        return;
    }
    http_sender_opts->sender_options_json = json_data;
}

void HttpSender::loop()
{
    // No maintenance operations for HTTP, hence empty
}

bool HttpSender::sendPayload(uint8_t *payload, size_t length, JsonObject &options)
{
    http_options_t http_options;
    parseJsonOptions(options, (void *)&http_options);
    String route = "/" + http_options.topic;
    String base_url = sender_options_json["base_url"].as<String>();
    if (base_url.length() <= 0)
    {
        ESP_LOGD(TAG, "Base URL is empty!");
        return false;
    }
    int port = sender_options_json["port"].as<int>();
    ESP_LOGD(TAG, "HTTP Sending on URL: %s:%d route: %s\n", base_url.c_str(), port, route.c_str());
    http.begin(client, base_url, port, route, false);
    http.setTimeout(2000);
    return send(payload, length, &http_options);
}

bool HttpSender::parseJsonOptions(JsonObject &options, void *sender_opts)
{
    http_options_t *http_options = (http_options_t *)sender_opts;
    String method_str = options["method"].as<String>();
    if (method_str == "POST")
        http_options->method = HTTP_METHOD_POST;
    else // (method_str == "GET")
        http_options->method = HTTP_METHOD_GET;
    http_options->topic = options["route"].as<String>();
    http_options->content_type = options["content_type"].as<String>();
}

bool HttpSender::send(uint8_t *payload, size_t length, http_options_t *http_options)
{
    ESP_LOGD(TAG, "Payload: %s", (char *)payload);
    Serial.printf("Payload: %s", (char *)payload);
    int http_response_code = 0;
    if (WiFi.status() != WL_CONNECTED)
    {
        ESP_LOGD(TAG, "Wifi is disconnected! Aborting send...\n");
        return false;
    }
    try
    {
        bool basic_auth = sender_options_json["basic_auth"].as<bool>();
        if (basic_auth)
        {
            String basic_auth_username = sender_options_json["basic_auth_username"].as<String>();
            String basic_auth_password = sender_options_json["basic_auth_password"].as<String>();
            http.setAuthorization(basic_auth_username.c_str(), basic_auth_password.c_str());
        }
        switch (http_options->method)
        {
        case HTTP_METHOD_GET:
            http_response_code = http.GET();
            break;
        case HTTP_METHOD_POST:
            if (http_options->content_type.length() > 0)
            {
                http.addHeader("Content-Type", http_options->content_type.c_str());
            }
            http_response_code = http.POST(payload, length);
            break;
        default:
            break;
        }
        http.end();
    }
    catch (...)
    {
        http_response_code = 0;
        ESP_LOGD(TAG, "An error occurred while making http request!\n");
    }
    Serial.printf("HTTP response code: %d\n", http_response_code);
    ESP_LOGD(TAG, "HTTP response code: %d\n", http_response_code);
    if (http_response_code == HTTP_CODE_OK)
        return true;
    else
        return false;
}

bool HttpsSender::init(void *options)
{
    secure_client.setInsecure();
    // secure_client.setCACert(CA_CERT);
    HttpSender::init(options);
    return true;
}

bool HttpsSender::sendPayload(uint8_t *payload, size_t length, JsonObject &options)
{
    http_options_t http_options;
    parseJsonOptions(options, (void *)&http_options);
    String route = "/" + http_options.topic;
    String base_url = sender_options_json["base_url"].as<String>();
    if (base_url.length() <= 0)
    {
        ESP_LOGD(TAG, "Base URL is empty!");
        return false;
    }
    int port = sender_options_json["port"].as<int>();
    ESP_LOGD(TAG, "HTTPS Sending on URL: %s:%d route: %s\n", base_url.c_str(), port, route.c_str());
    http.begin(secure_client, base_url, port, route, true);
    http.setTimeout(2000);
    return send(payload, length, &http_options);
}