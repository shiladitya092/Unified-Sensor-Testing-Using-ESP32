/*********************************************************************************************
 Title  :   C file include udp setup
 Author:    Abhisek Ghosh <aghosh@machinesense.com>

**********************************************************************************************/
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <cstdio>
#include <functional>
#include "mdh_discovery_client.h"
#include "configurator.h"

MdhDiscoveryClient mdh_discovery_client;

bool MdhDiscoveryClient::init(mdh_discovery_init_opts_t &mdh_discovery_init_opts)
{
  this->server_name = mdh_discovery_init_opts.server_name;
  this->server_port = mdh_discovery_init_opts.server_port;
  this->client_name = mdh_discovery_init_opts.client_name;
  this->client_port = mdh_discovery_init_opts.client_port;
  this->targets = mdh_discovery_init_opts.targets;
  this->Udp.listen(client_port);
  this->Udp.onPacket(std::bind(&MdhDiscoveryClient::handlePacket, this, std::placeholders::_1));
  if(mdh_discovery_init_opts.default_server.length() > 0)
  {
    updateSenders(mdh_discovery_init_opts.default_server);
    global_configurator.saveConfigs();
  }
  this->is_setup = true;
  startBroadcast();
  return true;
}

void MdhDiscoveryClient::startBroadcast()
{
  TimerHandle_t timer;
  ESP_LOGD(TAG, "Starting broadcast...");
  timer = xTimerCreate("broadcast_timer", 10000, pdTRUE, (void *)client_port, checkMDHConnectivityAndBroadcast);
  checkMDHConnectivityAndBroadcast(timer);
  xTimerStart(timer, 0);
}

void checkMDHConnectivityAndBroadcast(TimerHandle_t pxTimer)
{
  int max_len = 255;
  char buffer[max_len];

  if (!mdh_discovery_client.isMDHResponded())
  {
    ESP_LOGD(TAG, "No response from MDH yet, sending broadcast...");
    mdh_discovery_client.createRequestMessage(buffer, max_len);
    mdh_discovery_client.broadcastPacket((uint8_t *)buffer, String(buffer).length());
  }
  else
  {
    ESP_LOGD(TAG, "Response from MDH has been received. Stopping broadcasts...");
    xTimerStop(pxTimer, 0);
  }
}

bool MdhDiscoveryClient::isMDHResponded()
{
  return mdh_responded;
}

void MdhDiscoveryClient::createRequestMessage(char *buffer, int max_len)
{
  snprintf(buffer, max_len, "REQUEST HOST:%s", client_name.c_str());
}

bool MdhDiscoveryClient::broadcastPacket(uint8_t *data, size_t len)
{
  this->Udp.broadcastTo(data, len, server_port);
  return true;
}

void MdhDiscoveryClient::handlePacket(AsyncUDPPacket &packet)
{
  String data;

  int packetSize = packet.length();
  if (packetSize <= 0)
    return;
  ESP_LOGD(TAG, "Packet Received: %d bytes from %s, server_port %d\n", packetSize, packet.remoteIP().toString().c_str(), packet.remotePort());
  data = String((char *)packet.data());
  ESP_LOGD(TAG, "UDP packet contents: %s\n", data.c_str());
  String expected_response = "RESPONSE HOST:" + server_name;
  if (data == expected_response)
  {
    ESP_LOGD(TAG, "Match found! Updating...");
    String mdh_server = packet.remoteIP().toString();
    global_configurator.config_json["mdh_discovery_client"]["mdh_server"] = mdh_server;
    global_configurator.config_json["mdh_discovery_client"]["default_server"] = mdh_server;
    updateSenders(mdh_server);
    global_configurator.saveConfigs();
    mdh_responded = true;
  }
}

void MdhDiscoveryClient::updateSenders(String &mdh_server)
{
  for (JsonObject target : targets)
  {
    String name = target["name"].as<String>();
    String config_item = target["config_item"].as<String>();
    JsonArray senders = global_configurator.config_json["senders"].as<JsonArray>();
    for (JsonObject sender : senders)
    {
      if (sender["name"].as<String>() == name)
      {
        sender["sender_options"][config_item] = mdh_server;
      }
    }
  }
}
