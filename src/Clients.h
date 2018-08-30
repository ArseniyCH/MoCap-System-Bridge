/**
 * @brief 
 * 
 * @file Clients.h
 * @author Arseniy Churin
 * @date 2018-05-20
 */

#ifndef CLIENTS_H
#define CLIENTS_H

#include "Arduino.h"
#include <ESPAsyncWebServer.h>
#include <Ticker.h>

#include "WString.h"
#include <HashMap.h>

#define DELTA 5000

typedef struct
{
    int16_t r, g, b;
} RGB;

typedef struct
{
    uint8_t *mac;
    AsyncWebSocketClient *client;
    RGB color = {0, 0, 0};
    uint16_t id;
    Ticker client_ticker;
    bool confirmed = false;
} ClientInfo;

typedef struct
{
    std::function<void(uint8_t *)> timeout_func;
    uint8_t *mac_param;
} TimeOutCallback;

/**
 * @brief Class to work with nodes data
 * 
 * Register/Unregister nodes
 * 
 */
class Clients
{
  public:
    /**
     * @brief Construct a new Clients object
     * 
     */
    Clients();

    void loop();
    void addClient(uint8_t *mac, AsyncWebSocketClient *client = nullptr, uint16_t id = 0, RGB color = {0, 0, 0});
    Ticker addEmptyClient(uint8_t *mac);
    bool confirmClient(uint8_t *mac);
    bool isConfirmed(uint16_t id);
    void completeClientInfo(uint8_t *mac, AsyncWebSocketClient *client = nullptr, uint16_t id = 0, RGB color = {0, 0, 0});
    void updateClients();
    void printClients();
    void deleteClient(uint16_t id);
    void deleteClient(uint8_t mac[6]);
    void deleteClientAfterSeconds(uint8_t *mac, uint16_t seconds);

    void timeout(uint8_t *mac);

    static uint32_t shortMAC(uint8_t *mac)
    {
        uint32_t short_mac;
        memcpy(&short_mac, mac + 2, sizeof short_mac);
    }

  private:
    bool contains(uint8_t *mac);
    bool contains(uint32_t shortMac);
    bool getShortMacById(uint32_t &shortMac, uint16_t id);

    int16_t lastMillis = 0;

    HashMap<uint32_t, ClientInfo *, 20> clientsMap;
};

#endif