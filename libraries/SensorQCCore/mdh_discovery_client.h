/*********************************************************************************************
 Title  :   H file include udp setup
 Author:    Abhisek Ghosh <aghosh@machinesense.com>

**********************************************************************************************/
#ifndef UDP_H
#define UDP_H

#include <Arduino.h>
#include <AsyncUDP.h>
#include <ArduinoJson.h>

typedef struct mdh_discovery_init_opts_t
{
  unsigned int server_port;
  unsigned int client_port;
  String client_name;
  String server_name;
  JsonArray targets;
  String default_server;
} mdh_discovery_init_opts_t;

class MdhDiscoveryClient
{
  unsigned int server_port;
  unsigned int client_port;
  bool is_setup = false;
  bool mdh_responded = false;
  AsyncUDP Udp;
  String client_name;
  String server_name;
  JsonArray targets;

public:
  bool init(mdh_discovery_init_opts_t &mdh_discovery_init_opts);
  void startBroadcast();
  void handlePacket(AsyncUDPPacket &packet);
  void createRequestMessage(char *buffer, int max_len);  
  bool broadcastPacket(uint8_t *data, size_t len);
  bool isMDHResponded();
  void updateSenders(String &mdh_server);
};

extern MdhDiscoveryClient mdh_discovery_client;
void checkMDHConnectivityAndBroadcast(TimerHandle_t pxTimer);

#endif
