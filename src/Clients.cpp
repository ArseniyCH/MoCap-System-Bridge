#include "Clients.h"

Clients::Clients()
{
    clientsList = Linkedlist<ClientInfo>();
}

void Clients::loop()
{
    // Serial.println("MAC:");
    // printClients();
    // Serial.println("end");
    updateClients();
}

void Clients::addClient(String mac, AsyncWebSocketClient *client, uint16_t id, RGB color)
{
    ClientInfo s;
    s.mac = mac;
    s.color = color;
    s.client = client;
    s.id = id;
    if (!isContains(mac))
        clientsList.add(s);
    else
    {
        int16_t i = getClient(mac);
        if (client->id() != clientsList.get(i).client->id())
        {
            clientsList.remove(i);
            clientsList.add(s);
        }
    }
}

int16_t Clients::getClient(String mac)
{
    for (int16_t i = 0; i < clientsList.size(); ++i)
    {
        if (clientsList.get(i).mac.equals(mac))
            return i;
    }
}

void Clients::updateClients()
{
    for (int16_t i = 0; i < clientsList.size(); ++i)
    {
        if (clientsList.get(i).client->status() != WS_CONNECTED)
            clientsList.remove(i);
    }
}

void Clients::deleteClient(uint16_t id)
{
    for (int16_t i = 0; i < clientsList.size(); ++i)
    {
        if (clientsList.get(i).id == id)
            clientsList.remove(i);
    }
}

bool Clients::isContains(String val)
{
    for (int16_t i = 0; i < clientsList.size(); ++i)
    {
        if (clientsList.get(i).mac.equals(val))
            return true;
    }
    return false;
}

void Clients::printClients()
{
    for (int16_t i = 0; i < clientsList.size(); ++i)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println(clientsList.get(i).mac);
    }
}
