#ifndef HTTP_SENDER_H
#define HTTP_SENDER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "sender.h"

#define HTTP_TIMEOUT 1

typedef struct http_sender_opts_t
{
    JsonObject sender_options_json;
} http_sender_opts_t;

typedef enum http_methods_t
{
    HTTP_METHOD_GET = 0,
    HTTP_METHOD_POST
} http_methods_t;

typedef struct http_options_t
{
    http_methods_t method;
    String content_type;
    String topic;
} http_options_t;

class HttpSender: public Sender
{
    protected:
        JsonObject sender_options_json;
        WiFiClient client;
        HTTPClient http;
        bool send(uint8_t *payload, size_t length, http_options_t *http_options);
    public:
        bool init(void *sender_options);
        void newSenderInitOptions(void **sender_init_opts, JsonObject &json_data);
        bool updateBaseUrl(String new_url);
        bool parseJsonOptions(JsonObject &options, void *sender_opts);
        void loop();
        bool sendPayload(uint8_t *payload, size_t length, JsonObject &options);
};

class HttpsSender: public HttpSender
{
    private:
        WiFiClientSecure secure_client;
    public:
        bool init(void *sender_options);
        bool sendPayload(uint8_t *payload, size_t length, JsonObject &options);
};

#endif