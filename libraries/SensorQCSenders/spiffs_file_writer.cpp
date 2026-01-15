#include <FS.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "sender.h"
#include "spiffs_file_writer.h"



bool SpiffsFileWriter::init(void *sender_options)
{
    spiffs_file_writer_opts_t *spiffs_file_writer_opts = (spiffs_file_writer_opts_t *)sender_options;

    filename = "/" + String(spiffs_file_writer_opts->filename);
    binary = spiffs_file_writer_opts->binary;
    append = spiffs_file_writer_opts->append;
    capacity_threshold = spiffs_file_writer_opts->capacity_threshold;
    usage_check_interval = spiffs_file_writer_opts->usage_check_interval;

    openFile();
    // writeHeader();
    closeFile();
    last_usage_check_time = millis();
    write_enabled = true;
    return true;
}

bool SpiffsFileWriter::writeHeader()
{
    readStartCount();
    file.printf("\n\n%lu,START,%u\n", millis(), start_count);
    updateStartCount();
    return true;
}

bool SpiffsFileWriter::openFile()
{
    if (append && SPIFFS.exists(filename))
        file = SPIFFS.open(filename, FILE_APPEND);
    else
    {
        file = SPIFFS.open(filename, FILE_WRITE);
    }
    if (!file)
    {
        ESP_LOGD(TAG, "Failed to open file: %s\n", filename.c_str());
        return false;
    }
    return true;
}

bool SpiffsFileWriter::closeFile()
{
    if (file)
        file.close();
    return true;
}

bool SpiffsFileWriter::createStartupFile()
{
    start_count = 0;
    return updateStartCount();
}

bool SpiffsFileWriter::readStartCount()
{
    File startup_file = SPIFFS.open("/startup.json", FILE_READ);
    DynamicJsonDocument startup_json(128);
    DeserializationError error;

    if (!startup_file)
    {
        ESP_LOGD(TAG, "Failed to open file: /startup.json! Creating...\n");
        createStartupFile();
        return false;
    }
    error = deserializeJson(startup_json, startup_file);
    startup_file.close();
    if (error)
    {
        ESP_LOGD(TAG, "DeserializeJson() failed with code: ");
        ESP_LOGD(TAG, "%s", error.f_str());
        return false;
    }
    start_count = startup_json["start_count"].as<unsigned int>();
    return true;
}

bool SpiffsFileWriter::updateStartCount()
{
    DynamicJsonDocument startup_json(128);
    File startup_file = SPIFFS.open("/startup.json", FILE_WRITE);
    DeserializationError error;

    startup_json["start_count"] = start_count + 1;
    if (!startup_file)
    {
        ESP_LOGD(TAG, "Failed to open file: /startup.json\n");
        return false;
    }
    serializeJson(startup_json, startup_file);
    startup_file.close();
    return true;
}

bool SpiffsFileWriter::fileWrite(uint8_t *payload, size_t length, spiffs_file_opts_t &spiffs_file_opts)
{
    size_t write_len = 0;
    if (!openFile())
        return false;
    if (binary)
    {
        write_len = file.write(payload, length);
    }
    else
    {
        if (payload[length - 1] != 0)
        {
            char tmp[length + 1];
            memcpy(tmp, payload, length);
            tmp[length] = 0;
            write_len = file.print(tmp);
        }
        else
        {
            write_len = file.print((char *)payload);
        }
    }
    file.print("\n");
    closeFile();
    if (write_len == length)
    {
        ESP_LOGD(TAG, "File write successful!\n");
        return true;
    }
    ESP_LOGD(TAG, "Failed to write to file!\n");
    return false;
}

void SpiffsFileWriter::newSenderInitOptions(void **sender_init_opts, JsonObject &json_data)
{
    *sender_init_opts = calloc(1, sizeof(spiffs_file_writer_opts_t));
    spiffs_file_writer_opts_t *spiffs_file_writer_opts = (spiffs_file_writer_opts_t *)*sender_init_opts;
    if (!spiffs_file_writer_opts)
    {
        ESP_LOGD(TAG, "Error creating SpiffsFileWriter options!\n");
        return;
    }
    spiffs_file_writer_opts->filename = json_data["filename"].as<String>();
    spiffs_file_writer_opts->binary = json_data["binary"].as<bool>();
    spiffs_file_writer_opts->append = json_data["append"].as<bool>();
    spiffs_file_writer_opts->capacity_threshold = json_data["capacity_threshold"].as<float>();
    spiffs_file_writer_opts->usage_check_interval = json_data["usage_check_interval"].as<float>();
}

void SpiffsFileWriter::loop()
{
    if (millis() - last_usage_check_time >= usage_check_interval)
    {
        float spiffs_usage = (float)SPIFFS.usedBytes() / SPIFFS.totalBytes();
        ESP_LOGD(TAG, "Spiffs usage: %lu of %lu bytes\n", SPIFFS.usedBytes(), SPIFFS.totalBytes());
        if (spiffs_usage > capacity_threshold)
        {
            ESP_LOGD(TAG, "SPIFFS usage is over threshold %f%. Disabling writes...\n", capacity_threshold);
            if (file)
                file.close();
            write_enabled = false;
        }
        last_usage_check_time = millis();
    }
}

bool SpiffsFileWriter::sendPayload(uint8_t *payload, size_t length, JsonObject &options)
{
    spiffs_file_opts_t spiffs_file_opts;

    parseJsonOptions(options, (void *)&spiffs_file_opts);
    return fileWrite(payload, length, spiffs_file_opts);
}

bool SpiffsFileWriter::parseJsonOptions(JsonObject &options, void *sender_opts)
{
    spiffs_file_opts_t *spiffs_file_opts = (spiffs_file_opts_t *)sender_opts;
    return true;
}