/**
 * @brief 
 * 
 * @file StateMachine.hpp
 * @author Arseniy Churin
 * @date 2018-05-20
 */

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <Arduino.h>
#include <WString.h>

#include "WebServer.h"
#include "SerialManager.h"

typedef enum
{
    Normal = 0,
    Bind = 1
} State;

static State _state = Normal;

WebServer *server;
SerialManager serial = SerialManager(115200, true);

static void setState(State state)
{
    switch (state)
    {
    case Normal:
        break;
    case Bind:
        break;
    }
    _state = state;
}

static void stateNormal()
{
    setState(Normal);
}

static void stateBind()
{
    setState(Bind);
}

void RecieveFromSerial(uint8_t *buf, uint16_t len)
{
    Serial.println("Recieved from serial:");
    Serial.print("length: ");
    Serial.println(len);
    Serial.print("content: ");
    for (size_t i = 0; i < len; ++i)
    {
        Serial.printf("%X ", buf[i]);
    }
    Serial.println();

    if (len < 1)
        return;

    char command = buf[0];

    switch (command)
    {
    case 0x19:
    {
        String nodes = command + server->getActiveNodes();
        Serial.println("!command == 0x19");
        Serial.print("content: ");
        for (size_t i = 0; i < nodes.length(); ++i)
        {
            Serial.printf("%X ", (uint8_t)nodes[i]);
        }
        Serial.println();

        serial.sendMessage((uint8_t *)nodes.c_str(), nodes.length());
        return;
    }
    case 0x20:
        memcpy(buf + 1, &server->bridge_id, 4);
        serial.sendMessage(buf, 5);
        return;
    case 0x32:
        if (_state != Bind)
        {
            stateBind();
            server->BindMode();
        }
        return;
    case 0x33:
        if (_state != Normal)
        {
            stateNormal();
            server->NormalMode();
        }
        return;
    case 0x37:
        uint8_t st = _state;
        Serial.print("Current state: ");
        Serial.print((uint8_t)st);
        Serial.println();
        serial.sendMessage(&st, 1);
        return;
    }

    uint8_t count = buf[1];
    uint8_t *data;

    if (len >= 2 && count == 0)
    {
        memcpy(buf + 1, buf + 2, len - 2);

        Serial.println("!msg.length() >= 2 && count == 0");
        Serial.print("content: ");
        for (size_t i = 0; i < len - 1; ++i)
        {
            Serial.printf("%X ", (uint8_t)buf[i]);
        }
        Serial.println();

        server->broadcastBin(buf, len - 1);
        return;
    }

    if (len >= 2 + count * 6)
    {
        uint8_t *mac;

        uint16_t data_len = len - (1 + count * 6);
        uint8_t *data;
        if (data_len > 1)
        {
            data = new uint8_t[data_len];
            data[0] = command;
            memcpy(data + 1, buf + 2 + count * 6, len - (2 + count * 6));
        }
        else
            data = buf;

        // String data = command + msg.substring(2 + count * 6);
        uint16_t index = 0;

        Serial.println("!msg.length() >= 2 + count * 6");
        Serial.print("content: ");

        for (int i = 0; i < count; ++i)
        {
            index = 2 + i * 6;
            mac = buf + index;
            for (size_t i = 0; i < 6; ++i)
            {
                Serial.printf("%X ", mac[i]);
            }
            Serial.print("; \t");
            for (size_t i = 0; i < data_len; ++i)
            {
                Serial.printf("%d ", data[i]);
            }
            Serial.println();

            server->sendBin(mac, data, data_len);
        }

        if (data_len > 1)
            delete[] data;
    }
}

void RecieveFromNode(uint8_t *buf, uint16_t len)
{
    // Serial.println("Recieved from node:");
    // Serial.print("length: ");
    // Serial.println(len);

    // Serial.print("content: ");
    // Serial.printf("%X", buf[0]);
    // for (size_t i = 1; i < len; ++i)
    // {
    //     Serial.print('\t');
    //     Serial.print(buf[i]);
    // }
    // Serial.println();
    serial.sendMessage(buf, len);
}

void state_setup()
{
    server = new WebServer();
    serial.onRecieve(RecieveFromSerial);
    server->msg_handler(RecieveFromNode);
}

void state_loop()
{
    serial.loop();
};

#endif STATEMACHINE_H