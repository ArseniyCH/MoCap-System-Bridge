/**
 * @brief 
 * 
 * @file SerialManager.cpp
 * @author Arseniy Churin
 * @date 2018-09-03
 */

#include "SerialManager.h"
#include <Arduino.h>

SerialManager::SerialManager(uint64_t baudrate, bool debug, char endline)
{
    Serial.begin(baudrate);
    Serial.setDebugOutput(debug);
    this->_endline = endline;
}

void SerialManager::onRecieve(SerialBytesEvent event)
{
    _onrecieve = event;
}

void SerialManager::sendMessage(uint8_t *buf, uint16_t len)
{
    Serial.write(buf, len);
    Serial.write('\n');
}

void SerialManager::loop()
{

    // if (Serial.available() > 0)
    // {
    while (Serial.available() > 0)
    {
        _byte = Serial.read();

        if (_byte != _endline)
        {
            receivedChars[ndx] = _byte;
            ndx++;
            if (ndx >= num)
            {
                ndx = num - 1;
            }
        }
        else
        {
            receivedChars[ndx] = '\0'; // terminate the string
            _onrecieve(receivedChars, ndx);
            ndx = 0;
        }
    }
}

// void SerialManager::loop()
// {
//     if (Serial.available())
//     {
//         _char = uint8_t(Serial.read());
//         Serial.printf("%X ", _char);

//         if (_char == _endline)
//         {
//             _onrecieve(input);
//             input = "";
//             return;
//         }
//         else
//             input += _char;
//     }
// }

// void SerialManager::loop()
// {
//     if (millis() - last_millis < 10)
//         return;

//     while (Serial.available() > 0)
//     {
//         int inChar = Serial.read();
//         if (inChar != _endline)
//         {
//             input += (char)inChar;
//             break;
//         }
//     }
//     // char in[100];
//     // strcpy(in, input.c_str());

//     if (input.compareTo(""))
//     {
//         // char buf[input.length()];
//         // char *p = buf;
//         // char *str;
//         // String out = "";
//         // uint8_t i = 0;
//         // str = strtok_r(in, " ", &p);
//         // while (str != NULL)
//         // {
//         //     out += (char)atoi(str);
//         //     i++;
//         //     str = strtok_r(NULL, " ", &p);
//         // }

//         // //1server.broadcastBin((uint8_t *)out.c_str(), out.length());

//         // Serial.print("Send binary; length: ");
//         // Serial.println(out.length());
//         // Serial.print("content :");
//         // for (int i = 0; i < out.length(); ++i)
//         // {
//         //     Serial.print((uint8_t)out[i]);
//         //     Serial.print(" ");
//         // }
//         // Serial.println();

//         if (_onrecieve)
//             _onrecieve(input);

//         input = "";

//     }
//}