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

typedef enum
{
    Undef,
    Bind,
    Detach,
    Calibration,
    Standby,
    Active,
    Search
} State;

static State _state = Undef;

WebServer server = WebServer();

static void setState(State state)
{
    switch (state)
    {
    case Undef:
        // state Undef exit logic
    case Bind:
        break;
    case Detach:
        break;
    case Calibration:
        // state Calibration exit logic
        break;
    case Standby:
        // state Standby exit logic
        break;
    case Active:
        break;
    case Search:
        // state Searching exit logic
        break;
    }
    _state = state;
}

static void stateUndef()
{
    setState(Undef);
    // State Undef enter logic
}

static void stateSearch();

static void stateBind()
{
    setState(Bind);
}

static void stateDetach()
{
    setState(Detach);
}

static void stateCalibration()
{
    setState(Calibration);
    // State Calibration enter logic
}

static void stateStandby()
{
    Serial.println("Standby Enter");
    setState(Standby);
    // State Standby enter logic
}

static void stateActive()
{
    Serial.println("Active Enter");
    setState(Active);

    // State Active enter logic
}

static void stateSearch()
{
    Serial.println("Search Enter");
    setState(Search);
}

void state_setup()
{
}

void state_loop()
{
    String inString = "";
    while (Serial.available() > 0)
    {
        int inChar = Serial.read();
        if (inChar != '\n')
            inString += (char)inChar;
    }
    char in[100];
    strcpy(in, inString.c_str());

    if (inString.compareTo(""))
    {
        char buf[inString.length()];
        char *p = buf;
        char *str;
        String out = "";
        uint8_t i = 0;
        str = strtok_r(in, " ", &p);
        while (str != NULL)
        {
            out += (char)atoi(str);
            i++;
            str = strtok_r(NULL, " ", &p);
        }

        server.broadcastBin((uint8_t *)out.c_str(), out.length());

        Serial.print("Send binary; length: ");
        Serial.println(out.length());
        Serial.print("content :");
        for (int i = 0; i < out.length(); ++i)
        {
            Serial.print((uint8_t)out[i]);
            Serial.print(" ");
        }
        Serial.println();

        inString = "";
    }
}

#endif STATEMACHINE_H