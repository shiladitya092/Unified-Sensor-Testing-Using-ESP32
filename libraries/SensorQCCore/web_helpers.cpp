#include "web_helpers.h"


// If an error occurs the json will be incomplete.
String json_success_response(String json_data, int &error)
{
  String json = "{";

  error = 0;
  json += "\"success\":true";
  if (json_data.length() > 0)
  {
    json += ",\"data\":";
    if (!json.concat(json_data))
    {
      ESP_LOGD(TAG,"String concat failed\n");
      error = 1;
      return json;
    }
  }
  json += "}";
  return json;
}

int send_json_success_response(AsyncWebServerRequest *s, String json)
{
  int error;

  s->send(200, "application/json", json_success_response(json, error));
  return error;
}

// Wrap function call with error for other functions that don't look for error.
String json_success_response(String json_data)
{
  int error;

  return json_success_response(json_data, error);
}
void send_json_error_response(AsyncWebServerRequest *s, String msg)
{
  String json = "{\"error\":\"" + msg + "\"}";
  s->send(400, "application/json", json);
}

void send_json_auth_error_response(AsyncWebServerRequest *s, String msg)
{
  String json = "{\"error\":\"" + msg + "\"}";
  // s->sendHeader("WWW-Authenticate", "Basic realm=\"Virawarn Authentication\"");
  s->send(401, "application/json", json);
}

bool web_authenticate(AsyncWebServerRequest *s, const char *user, const char *pass, const char *msg)
{
  if (!msg)
    msg = "Auth Error";
  if (!s->authenticate(user, pass))
  {
    send_json_auth_error_response(s, msg);
    return false;
  }
  return true;
}

#if USE_SQLITE_STORAGE == 1

void send_json_success_response(AsyncWebServerRequest *srv_handle, DBQueryResult &qresult)
{
  String json = "{";
  int recCount = 0;
  String jsonRow;
  int step_res;
  char buffer[200];

  json += "\"data\":[";
  srv_handle->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  srv_handle->sendHeader("Pragma", "no-cache");
  srv_handle->sendHeader("Expires", "-1");
  srv_handle->setContentLength(CONTENT_LENGTH_UNKNOWN);
  srv_handle->send(200, "application/json", json);

  qresult.reset();
  while ((step_res = qresult.nextRowToJson(jsonRow)) == SQLITE_ROW)
  {
    if (recCount)
      srv_handle->sendContent(",");
    srv_handle->sendContent(jsonRow);
    recCount++;
  }
  srv_handle->sendContent("]");
  if (step_res == SQLITE_DONE)
  {
    srv_handle->sendContent(",\"success\":true");
  }
  else
  {
    snprintf(buffer, 200, ",\"error\":\"%s\",\"error_code\":%d", qresult.errMsg(), step_res);
    srv_handle->sendContent(buffer);
  }
  srv_handle->sendContent("}");
}
#endif
