#ifndef _WEB_HELPERS_H
#define _WEB_HELPERS_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern String json_success_response(String json_data, int &error);
extern int send_json_success_response(AsyncWebServerRequest *s, String json);
extern void send_json_error_response(AsyncWebServerRequest *s, String msg);
extern String json_success_response(String json_data);
extern void send_json_auth_error_response(AsyncWebServerRequest *s, String msg);
extern bool web_authenticate(AsyncWebServerRequest *s, const char *user, const char *pass, const char *msg = NULL);


#if USE_SQLITE_STORAGE==1
#include "db_manager.h"

extern void send_json_success_response(AsyncWebServerRequest *srv_handle, DBQueryResult &qresult);
#endif

#endif
