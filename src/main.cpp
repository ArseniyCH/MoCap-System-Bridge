/**
 * @brief Bridge main
 * 
 * @file main.cpp
 * @author Arseniy Churin
 * @date 2018-05-20
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "Clients.h"
#include <WString.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

Clients clients = Clients();

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
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    client->keepAlivePeriod(1);
    client->printf("whois");
    os_printf("ws[%s][%u] connect\n", server->url(), client->id());
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.print("Client #");
    Serial.print(client->id());
    Serial.println(" deleted.");
    clients.deleteClient(client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len)
    {
      //the whole message is in a single frame and we got all of it's data
      //Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < info->len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < info->len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      if (msg.startsWith("id:"))
      {

        clients.addClient(msg.substring(3), client, client->id());
      }

      Serial.printf("%s\n", msg.c_str());

      // if (info->opcode == WS_TEXT)
      //   client->text("I got your text message");
      // else
      //   client->binary("I got your binary message");
    }
    else
    {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0)
      {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < info->len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < info->len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len)
      {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final)
        {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        }
      }
    }
  }
}

const char *hostName = "mcs_0";

/**
 * @brief Main setup method
 * 
 */
void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.hostname(hostName);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(hostName);

  MDNS.addService("http", "tcp", 80);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.begin();
}

String inString;

uint32_t lastMillis = 0;

/**
 * @brief Main loop method
 * 
 */
void loop()
{
  clients.loop();
  String inString = "";
  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    if (inChar != '\n')
      inString += (char)inChar;
  }

  uint8_t buf[1];

  if (inString.compareTo(""))
  {

    uint8_t numb = inString.toInt();

    buf[0] = numb;
    ws.binaryAll(buf, 1);

    Serial.print("Send binary; length: ");
    Serial.println(1);
    Serial.print("content :");
    for (int i = 0; i < 1; ++i)
    {
      Serial.print(buf[i]);
    }
    Serial.println();

    inString = "";
  }
  //clients.printClients();

  delay(100);
}