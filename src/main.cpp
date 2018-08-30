/**
 * @brief Bridge main
 * 
 * @file main.cpp
 * @author Arseniy Churin
 * @date 2018-05-20
 */

#include <Arduino.h>

#include "StateMachine.hpp"

/**
 * @brief Main setup method
 * 
 */
void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  state_setup();
}

String inString;

uint32_t lastMillis = 0;

/**
 * @brief Main loop method
 * 
 */
void loop()
{
  state_loop();
}