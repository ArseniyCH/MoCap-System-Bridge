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
    clients.addOnAddHandler(std::bind(&WebServer::OnAddClient, this, std::placeholders::_1));
    clients.addOnDeleteHandler(std::bind(&WebServer::OnDeleteClient, this, std::placeholders::_1));
    establishConnection(hostName);
}

void WebServer::onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
    {
        client->keepAlivePeriod(1);
        char cmd = 0x15; //get MAC command
        client->binary(&cmd, 1);
        os_printf("ws[%s][%u] connect\n", server->url(), client->id());
        break;
    }
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
                // Serial.print("Binary data:\nlength: ");
                // Serial.println((uint32_t)info->len);
                // Serial.print("content: ");
                // for (size_t i = 0; i < info->len; i++)
                // {
                //     Serial.printf("%X", data[i]);
                //     Serial.print('\t');
                //     msg += (char)data[i];
                // }
                // Serial.println();

                bool confirm = clients.isConfirmed(client->id());

                if (data[0] == 0x15 && !confirm)
                {
                    if (clients.confirmClient(data + sizeof(uint8_t)))
                    {
                        clients.completeClientInfo(data + sizeof(uint8_t), client, client->id());
                    }
                    else
                        client->close(4000);
                    break;
                }

                if (!confirm)
                    break;

                uint8_t *b = new uint8_t[info->len + 6];

                uint8_t *mac;

                if (!clients.getMac(mac, client->id()))
                    break;

                // Serial.print("mac: ");
                // if (mac)
                //     for (size_t i = 0; i < 6; i++)
                //     {
                //         Serial.printf("%X ", mac[i]);
                //     }
                // Serial.println();

                memcpy(b, data, 1);
                memcpy(b + 1, mac, 6);
                memcpy(b + 1 + 6, data + 1, info->len - 1);

                _msghandler(b, info->len + 6);
                delete[] b;
            }
            break;
        }
    }
}

void WebServer::establishConnection(const char *hostname)
{
    connectHandler = WiFi.onSoftAPModeStationConnected([&](const WiFiEventSoftAPModeStationConnected &e) {
        Serial.println("WiFi client connected");
        ClientInfo *client = clients.addEmptyClient((uint8_t *)&e.mac);
        client->callback->timeout_func = std::bind(static_cast<void (Clients::*)(uint8_t *)>(&Clients::deleteClient), clients, std::placeholders::_1);

        memcpy(client->callback->mac_param, &e.mac, 6);

        client->client_ticker.once_ms<TimeOutCallback *>(2000, [](TimeOutCallback *tc) {
            Serial.println("haha");
            tc->timeout_func(tc->mac_param);
        },
                                                         client->callback);
    });

    disconnectHandler = WiFi.onSoftAPModeStationDisconnected([&](const WiFiEventSoftAPModeStationDisconnected &e) {
        Serial.println("WiFi client disconnected");
        clients.deleteClient((uint8_t *)&e.mac);
    });

    Serial.print("hostname: ");
    Serial.println(hostname);

    WiFi.disconnect();
    WiFi.hostname(hostname);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostname, NULL, 1, 0, 8);
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

    ping_ticker.attach<AsyncWebSocket *>(3, [](AsyncWebSocket *ws) { ws->pingAll(); }, &this->ws);
}

void WebServer::broadcastBin(uint8_t *buf, uint16_t len)
{
    if (clients.size())
        ws.binaryAll(buf, len);
}

void WebServer::sendBin(uint8_t *mac, uint8_t *buf, uint16_t len)
{
    Serial.println("sendBin");
    Serial.print("length: ");
    Serial.println(len);
    Serial.print("content: ");
    if (buf)
        for (size_t i = 0; i < len; ++i)
        {
            printf("%X ", buf[i]);
        }
    Serial.println();

    uint32_t short_mac = Clients::shortMAC(mac);
    int16_t id = clients.getId(short_mac);
    Serial.print("id: ");
    Serial.println(id);
    if (id > 0)
        ws.binary(id, buf, len);
}

void WebServer::msg_handler(WsBinaryEvent event)
{
    _msghandler = event;
}

String WebServer::getActiveNodes()
{
    return clients.getActiveClients();
}

void WebServer::onClientChange(uint8_t cmd, uint8_t *mac)
{
    uint8_t *buf = new uint8_t[7];
    buf[0] = cmd;
    memcpy(buf + 1, mac, 6);
    if (_msghandler)
        _msghandler(mac, 6);

    delete[] buf;
}

void WebServer::OnAddClient(uint8_t *mac)
{
    Serial.println("!!!Add new client");
    onClientChange(0x17, mac);
}

void WebServer::OnDeleteClient(uint8_t *mac)
{
    Serial.println("!!!Delete client");
    onClientChange(0x18, mac);
}

void WebServer::BindMode()
{
    Serial.println("1");
    uint8_t cmd[2] = {0x96, 1};
    Serial.println("2");
    broadcastBin(cmd, 2);
    delay(50);
    Serial.println("3");
    WiFi.softAPdisconnect();
    Serial.println("4");
    establishConnection(bindHostName);
}

void WebServer::NormalMode()
{
    Serial.println("111");
    WiFi.softAPdisconnect();
    Serial.println("222");
    establishConnection(hostName);
}