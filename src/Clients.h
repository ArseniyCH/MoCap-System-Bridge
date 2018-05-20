#ifndef CLIENTS_H
#define CLIENTS_H

#include "Arduino.h"
#include <ESPAsyncWebServer.h>

#include "WString.h"
#include <Linkedlist.h>

#define DELTA 5000

typedef struct
{
    int16_t r, g, b;
} RGB;

typedef struct
{
    String mac;
    AsyncWebSocketClient *client;
    RGB color = {0, 0, 0};
    uint16_t id;
} ClientInfo;

class Clients
{
  public:
    Clients();

    void loop();
    void addClient(String mac, AsyncWebSocketClient *client, uint16_t id, RGB color = {0, 0, 0});
    void updateClients();
    void printClients();
    void deleteClient(uint16_t id);

  private:
    int16_t getClient(String mac);
    bool isContains(String val);

    int16_t lastMillis = 0;

    Linkedlist<ClientInfo> clientsList;
};

#endif