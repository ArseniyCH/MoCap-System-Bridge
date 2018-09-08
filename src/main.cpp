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
  state_setup();
}

/**
 * @brief Main loop method
 * 
 */
void loop()
{
  state_loop();
}