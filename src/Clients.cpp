/**
 * @brief 
 * 
 * @file Clients.cpp
 * @author Arseniy Churin
 * @date 2018-05-20
 */

#include "Clients.h"

Clients::Clients()
{
    clientsMap.setNullValue(nullptr);
}

void Clients::loop()
{
    updateClients();
}

void Clients::addClient(uint8_t *mac, AsyncWebSocketClient *client, uint16_t id, RGB color)
{
    ClientInfo *s;
    for (int i = 0; i < 6; ++i)
        s->mac[i] = mac[i];
    s->color = color;
    s->client = client;
    s->id = id;
    uint32_t m = shortMAC(mac);
    if (!contains(m))
        clientsMap[m] = s;
    else if (client->id() != clientsMap[m]->id)
        clientsMap[m] = s;
}

Ticker Clients::addEmptyClient(uint8_t *mac)
{
    //Distruct if not confirmed
    ClientInfo *eci;
    eci->mac = mac;
    clientsMap[shortMAC(mac)] = eci;
    return eci->client_ticker;
}

bool Clients::confirmClient(uint8_t *mac)
{
    ClientInfo *ci = clientsMap[shortMAC(mac)];
    if (ci)
    {
        ClientInfo *ci = clientsMap[shortMAC(mac)];
        ci->confirmed = true;
        ci->client_ticker.detach();
        return true;
    }

    return false;
}

bool Clients::isConfirmed(uint16_t id)
{
    uint32_t shrt_mac;
    if (getShortMacById(shrt_mac, id) && clientsMap[shrt_mac]->confirmed)
        return true;
    return false;
}

void Clients::completeClientInfo(uint8_t *mac, AsyncWebSocketClient *client, uint16_t id, RGB color)
{
    ClientInfo *ci = clientsMap[shortMAC(mac)];
    ci->client = client;
    ci->id = id;
    ci->mac = mac;

    //TODO: run ping/pong
}

void Clients::updateClients()
{ //TODO: PEREPISAT' ETO GOVNO
    // for (int16_t i = 0; i < clientsList.size(); ++i)
    // {
    //     if (clientsList.get(i).client->status() != WS_CONNECTED || !clientsList.get(i).client->client()->connected())
    //         clientsList.remove(i);
    // }
}

void Clients::deleteClient(uint16_t id)
{
    for (int16_t i = 0; i < clientsMap.size(); ++i)
    {
        if (clientsMap.valueAt(i)->id == id)
            clientsMap.remove(clientsMap.keyAt(i));
    }
}

void Clients::deleteClient(uint8_t *mac)
{
    clientsMap.remove(shortMAC(mac));
}

void Clients::deleteClientAfterSeconds(uint8_t *mac, uint16_t seconds)
{
}

bool Clients::contains(uint8_t *mac)
{
    return clientsMap.contains(shortMAC(mac));
}

bool Clients::contains(uint32_t shortMac)
{
    return clientsMap.contains(shortMac);
}

bool Clients::getShortMacById(uint32_t &shortMac, uint16_t id)
{
    for (int16_t i = 0; i < clientsMap.size(); ++i)
    {
        if (clientsMap.valueAt(i)->id == id)
        {
            shortMac = clientsMap.keyAt(i);
            return true;
        }
    }
    return false;
}

void Clients::printClients()
{
    if (clientsMap.size() != 0)
    {
        Serial.println("Clients:");
        for (int16_t i = 0; i < clientsMap.size(); ++i)
        {
            Serial.print(clientsMap.valueAt(i)->id);
            Serial.print(":\t");

            for (int j = 0; j < 5; ++j)
            {
                Serial.printf("%X", clientsMap.valueAt(i)->mac[j]);
                Serial.print(":");
            }
            Serial.printf("%X", clientsMap.valueAt(i)->mac[5]);
        }
        Serial.println();
    }
}

void Clients::timeout(uint8_t *mac)
{
    clientsMap.remove(shortMAC(mac));
}