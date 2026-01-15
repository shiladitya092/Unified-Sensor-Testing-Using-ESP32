#ifndef SPIFFS_FILE_WRITER_H
#define SPIFFS_FILE_WRITER_H

#include <FS.h>
#include "sender.h"

typedef struct spiffs_file_opts_t
{
} spiffs_file_opts_t;

typedef struct spiffs_file_writer_opts_t
{
    String filename;
    bool binary;
    bool append;
    float capacity_threshold;
    float usage_check_interval;
} spiffs_file_writer_opts_t;

class SpiffsFileWriter : public Sender
{
protected:
    String filename;
    bool binary;
    bool append;
    float capacity_threshold;
    float usage_check_interval;
    unsigned int start_count;
    File file;
    unsigned long last_usage_check_time;
    bool write_enabled;
    bool fileWrite(uint8_t *payload, size_t length, spiffs_file_opts_t &spiffs_file_opts);
    bool updateStartCount();
    bool readStartCount();
    bool openFile();
    bool writeHeader();
    bool closeFile();
    bool createStartupFile();

public:
    bool init(void *sender_options);
    void newSenderInitOptions(void **sender_init_opts, JsonObject &json_data);
    void loop();
    bool sendPayload(uint8_t *payload, size_t length, JsonObject &options);
    bool parseJsonOptions(JsonObject &options, void *sender_opts);
};

#endif
