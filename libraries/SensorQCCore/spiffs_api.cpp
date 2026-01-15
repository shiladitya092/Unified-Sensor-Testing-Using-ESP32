#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "spiffs_api.h"
#include "web_helpers.h"
#include "qc_helpers.h"

// static AsyncWebServer *webSvr_handle;

void spiffs_api_files(AsyncWebServerRequest *request)
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    String response = "";
    float usage = (float)SPIFFS.usedBytes() / SPIFFS.totalBytes();
    response += "<center><H1>Virawarn R&D - File List<br>";
    response += "OPTEEV Research</H1></center><br>";
    while (file)
    {
        ESP_LOGD(TAG, "FILE: ");
        ESP_LOGD(TAG, "%s", file.name());
        ESP_LOGD(TAG, "\n");
        response += String("<A href=\"/spiffs/file?name=") + file.name() + "\">" + file.name() + "</a> " + "<A href=\"/spiffs/remove?name=" + file.name() + "\"> remove </a><br>\n";
        file = root.openNextFile();
    }
    response += "<br><br>Usage: " + String(usage);
    request->send(200, "text/html", response);
}

// May be put in seperate web helper lib
// Copied directly from: https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/FSBrowser/FSBrowser.ino
String getContentType(String filename)
{
    if (filename.endsWith(".htm"))
    {
        return "text/html";
    }
    else if (filename.endsWith(".html"))
    {
        return "text/html";
    }
    else if (filename.endsWith(".css"))
    {
        return "text/css";
    }
    else if (filename.endsWith(".js"))
    {
        return "application/javascript";
    }
    else if (filename.endsWith(".png"))
    {
        return "image/png";
    }
    else if (filename.endsWith(".gif"))
    {
        return "image/gif";
    }
    else if (filename.endsWith(".jpg"))
    {
        return "image/jpeg";
    }
    else if (filename.endsWith(".ico"))
    {
        return "image/x-icon";
    }
    else if (filename.endsWith(".xml"))
    {
        return "text/xml";
    }
    else if (filename.endsWith(".pdf"))
    {
        return "application/x-pdf";
    }
    else if (filename.endsWith(".zip"))
    {
        return "application/x-zip";
    }
    else if (filename.endsWith(".gz"))
    {
        return "application/x-gzip";
    }
    return "application/octet-stream";
}

void spiffs_api_file(AsyncWebServerRequest *request)
{
    // File fp;

    String name = request->arg("name");
    // fp = SPIFFS.open(name, FILE_READ);
    // if (fp)
    // {
    request->send(SPIFFS, name, getContentType(name));
    // fp.close();
    return;
    // }
    ESP_LOGD(TAG, "Could not open file for download: %s\n", name);
    send_json_error_response(request, "Could not open file for download: " + name);
}

void spiffs_api_remove(AsyncWebServerRequest *request)
{
    String name = request->arg("name");
    int rc;

    if ((rc = SPIFFS.remove(name)))
    {
        send_json_success_response(request, jsonStringVal("File Removed! Please restart the device if it was a streaming file!"));
    }
    else
    {
        send_json_error_response(request, "Failed to remove file: " + name);
    }
}

void spiffs_api_usage(AsyncWebServerRequest *request)
{
    float usage = (float)SPIFFS.usedBytes() / SPIFFS.totalBytes();
    String json = "{";
    json += jsonStringVal("Usage") + ":" + jsonStringVal(usage);
    json += "}";
    send_json_success_response(request, json);
}

void highcharts(AsyncWebServerRequest *request)
{
    String name = "/highcharts.js.gz";
    // File fp;
    // fp = SPIFFS.open(name, FILE_READ);
    // int len = fp.size();

    // if (fp)
    // {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, name, "application/javascript");
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);
        // fp.close();
    // }
    // else
    // {
    //     send_json_error_response(request, "Failed to open file: " + name);
    // }
}

void register_spiffs_api(AsyncWebServer &webSvr_handle)
{
    // webSvr_handle = &lserver;
    webSvr_handle.on("/spiffs/files", spiffs_api_files);
    webSvr_handle.on("/spiffs/file", spiffs_api_file);
    webSvr_handle.on("/spiffs/remove", spiffs_api_remove);
    webSvr_handle.on("/spiffs/usage", spiffs_api_usage);

    webSvr_handle.on("/highcharts.js", highcharts);
}