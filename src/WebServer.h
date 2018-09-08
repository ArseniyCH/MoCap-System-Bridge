/**
 * @brief 
 * 
 * @file WebServer.h
 * @author Arseniy Churin
 * @date 2018-08-28
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WIFI.h>
#include <ESP8266mDNS.h>
#include <cppQueue.h>
#include "Clients.h"

typedef std::function<void()> Event;
typedef std::function<void(uint8_t *, uint16_t)> WsBinaryEvent;
typedef std::function<void(const WiFiEventSoftAPModeStationConnected &)> WiFiConnectedEvent;
typedef std::function<void(const WiFiEventSoftAPModeStationDisconnected &)> WiFiDisconnectedEvent;

class WebServer
{
public:
  /**
     * @brief Construct a new Web Server object
     * 
     */
  WebServer();

  /**
     * @brief Sends inary message to all users
     * 
     * @param buf 
     * @param len 
     */
  void broadcastBin(uint8_t *buf, uint16_t len);

  void sendBin(uint8_t *mac, uint8_t *buf, uint16_t len);

  void msg_handler(WsBinaryEvent event);

  String getActiveNodes();

  void BindMode();
  void NormalMode();

  const uint32_t bridge_id = 0;

private:
  /**
     * @brief Recieve ws events
     * 
     * @param server 
     * @param client 
     * @param type 
     * @param arg 
     * @param data 
     * @param len 
     */
  void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

  void establishConnection(const char *hostname);

  void onClientChange(uint8_t cmd, uint8_t *mac);
  void OnAddClient(uint8_t *mac);
  void OnDeleteClient(uint8_t *mac);

  AsyncWebServer server = AsyncWebServer(80);
  AsyncWebSocket ws = AsyncWebSocket("/ws");

  Event _connect;
  WiFiConnectedEvent _wificonnect = [](const WiFiEventSoftAPModeStationConnected &) {};
  Event _disconnect;
  WiFiDisconnectedEvent _wifidisconnect = [](const WiFiEventSoftAPModeStationDisconnected &) {};

  WiFiEventHandler connectHandler;
  WiFiEventHandler disconnectHandler;

  const char *hostName = "mcs_0";
  const char *bindHostName = "mcsbnd_0";

  Clients clients = Clients();

  WsBinaryEvent _msghandler;

  Ticker ping_ticker;
};

#endif