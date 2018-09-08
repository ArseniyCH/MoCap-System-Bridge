/**
 * @brief 
 * 
 * @file SerialManager.h
 * @author Arseniy Churin
 * @date 2018-09-03
 */

#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#include <HardwareSerial.h>
#include <functional>

typedef std::function<void(String)> SerialStringEvent;
typedef std::function<void(uint8_t *, uint16_t)> SerialBytesEvent;

class SerialManager
{
public:
  SerialManager(uint64_t baudrate = 115200, bool debug = false, char endline = '\n');

  void onRecieve(SerialBytesEvent event);
  void sendMessage(uint8_t *buf, uint16_t len);

  void loop();

private:
  SerialBytesEvent _onrecieve;

  String input = "";
  char _endline = '\n';
  uint8_t _byte;
  uint8_t ndx = 0;

  static const uint16_t num = 150;
  uint8_t receivedChars[num];

  bool newData = false;
};

#endif