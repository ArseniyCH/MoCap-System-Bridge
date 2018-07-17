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

typedef enum {
    Undef,
    Bind,
    Detach,
    Calibration,
    Standby,
    Active,
    Search
} State;

static State _state = Undef;

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

void state_loop()
{
}

void parseInData(String input)
{
    if (input.equals("bndmode"))
        stateBind();
    if (input.equals("detachmode"))
        stateDetach();
    if (input.equals("777"))
        stateCalibration();
    if (input.equals("101"))
        stateActive();
    if (input.equals("010"))
        stateStandby();
}

#endif STATEMACHINE_H