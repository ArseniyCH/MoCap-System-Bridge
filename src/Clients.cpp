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

ClientInfo *Clients::addEmptyClient(uint8_t *mac)
{
    //Distruct if not confirmed
    Serial.println("here3");
    ClientInfo *eci = new ClientInfo();
    Serial.println("here3");
    memcpy(eci->mac, mac, 6);
    Serial.println("here3");
    clientsMap[shortMAC(mac)] = eci;
    Serial.println("here3");
    return eci;
}

bool Clients::confirmClient(uint8_t *mac)
{
    ClientInfo *ci = clientsMap[shortMAC(mac)];
    if (ci)
    {
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
    uint32_t key = shortMAC(mac);
    ClientInfo *ci = clientsMap[key];
    ci->client = client;
    ci->id = id;

    if (_onnewclient)
        _onnewclient(ci->mac);
}

void Clients::deleteClient(uint16_t id)
{
    Serial.println("Delete by id");
    ClientInfo *cci;
    for (int16_t i = 0; i < clientsMap.size(); ++i)
    {
        Serial.println("1111");
        cci = clientsMap.valueAt(i);
        if (cci && cci->id == id)
        {
            if (_ondeleteclient)
                _ondeleteclient(cci->mac);
            Serial.println("1112");
            delete cci;
            clientsMap.remove(clientsMap.keyAt(i));
        }
        Serial.println("1113");
    }
}

void Clients::deleteClient(uint8_t *mac)
{
    Serial.println("Delete by mac");
    ClientInfo *ci = clientsMap[shortMAC(mac)];
    Serial.println("a1");
    if (ci)
    {
        if (_ondeleteclient)
            _ondeleteclient(ci->mac);
        Serial.println("a2");
        delete ci;
        Serial.println("a3");
        clientsMap.remove(shortMAC(mac));
        Serial.println("a4");
    }
    Serial.println("a5");
}

void Clients::deleteAllClients()
{
    Serial.println("Delete by id all");
    ClientInfo *cci;
    for (int16_t i = 0; i < clientsMap.size(); ++i)
        deleteClient(clientsMap.valueAt(i)->mac);
}

void Clients::addOnDeleteHandler(MacEvent funcEvent)
{
    _ondeleteclient = funcEvent;
}
void Clients::addOnAddHandler(MacEvent funcEvent)
{
    _onnewclient = funcEvent;
}

bool Clients::getMac(uint8_t *&mac, uint16_t id)
{
    ClientInfo *cci;
    for (uint16_t i = 0; i < clientsMap.size(); ++i)
    {
        cci = clientsMap.valueAt(i);
        if (cci->id == id)
        {
            mac = cci->mac;
            return true;
        }
    }

    return false;
}

int16_t Clients::getId(uint32_t shortMAC)
{
    ClientInfo *ci = clientsMap[shortMAC];
    if (ci)
        return ci->id;
    return -1;
}

String Clients::getActiveClients()
{
    String clients = "";
    uint8_t *cur_mac;
    for (int i = 0; i < clientsMap.size(); ++i)
    {
        cur_mac = clientsMap.valueAt(i)->mac;

        for (size_t j = 0; j < 6; ++j)
        {
            clients += cur_mac[j];
        }
    }

    return clients;
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

uint16_t Clients::size()
{
    return clientsMap.size();
}
