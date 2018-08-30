/**
 * @brief 
 * 
 * @file WebServer.cpp
 * @author Arseniy Churin
 * @date 2018-08-28
 */

#include "WebServer.h"
#include <Ticker.h>

WebServer::WebServer()
{
    connectHandler = WiFi.onSoftAPModeStationConnected([&](const WiFiEventSoftAPModeStationConnected &e) {
        Serial.println("WiFi client connected");
        Ticker to_ticker = clients.addEmptyClient((uint8_t *)&e.mac);
        TimeOutCallback *tc;
        tc->timeout_func = std::bind(&Clients::timeout, clients, std::placeholders::_1);
        tc->mac_param = (uint8_t *)&e.mac;
        to_ticker.once_ms<TimeOutCallback *>(2000, [](TimeOutCallback *tc) {
            tc->timeout_func(tc->mac_param);
        },
                                             tc);
    });

    disconnectHandler = WiFi.onSoftAPModeStationDisconnected([&](const WiFiEventSoftAPModeStationDisconnected &e) {
        Serial.println("WiFi client disconnected");
        clients.deleteClient((uint8_t *)&e.mac);
    });

    WiFi.hostname(hostName);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostName, NULL, 1, 1, 8);

    MDNS.addService("http", "tcp", 80);

    ws.onEvent(std::bind(&WebServer::onWsEvent, this,
                         std::placeholders::_1,
                         std::placeholders::_2,
                         std::placeholders::_3,
                         std::placeholders::_4,
                         std::placeholders::_5,
                         std::placeholders::_6));
    server.addHandler(&ws);
    server.begin();
}

void WebServer::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        //client->keepAlivePeriod(1);
        client->binary((char *)0x15, 1);
        os_printf("ws[%s][%u] connect\n", server->url(), client->id());
        break;
    case WS_EVT_DISCONNECT:
        Serial.print("Client #");
        Serial.print(client->id());
        Serial.println(" deleted.");
        clients.deleteClient(client->id());
        break;
    case WS_EVT_ERROR:
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
        break;
    case WS_EVT_PONG:
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
        break;
    case WS_EVT_DATA:
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len)
        {
            //the whole message is in a single frame and we got all of it's data
            //Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

            if (info->opcode == WS_BINARY)
            {
                bool confirm = clients.isConfirmed(client->id());

                if (data[0] == 0x15 && !confirm)
                {
                    //TODO: Check for already confirm
                    if (clients.confirmClient(data + sizeof(uint8_t)))
                        clients.completeClientInfo(data + sizeof(uint8_t), client, client->id());
                    else
                        client->close(4000);
                    break;
                }

                if (!confirm)
                    break;

                switch (data[0])
                {
                case 0x00: //TODO: Write all comand handling
                    if (clients.confirmClient(data + sizeof(uint8_t)))
                        clients.completeClientInfo(data + sizeof(uint8_t), client, client->id());
                    else
                        client->close(4000);
                    break;
                }
                for (size_t i = 0; i < info->len; i++)
                {

                    msg += (char)data[i];
                }
            }
            break;
        }
    }
}

void WebServer::broadcastBin(uint8_t *buf, uint16_t len)
{
    ws.binaryAll(buf, len);
}