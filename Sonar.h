#ifndef Sonar_h
#define Sonar_h

#include "Arduino.h"
#include <Servo.h>

class Sonar {
public:
  // Sonar()=delete;
  // Sonar(byte trig_addr, byte echo_addr, byte servo_addr, void (*updateCallback)(uint16_t, uint8_t));
  ~Sonar()=default;
  void attach(byte trig_addr, byte echo_addr, byte servo_addr, void (*updateCallback)(uint16_t, uint8_t));
  void setDirection(uint8_t angle);
  uint64_t getDistance();
  uint8_t getDirection();

private:
  byte TRIG = 0;
  byte ECHO = 0;
  byte SERVADDR = 0;
  void (*sonarUpdateCallback)(uint16_t distance, uint8_t angle) = [](uint16_t dist, uint8_t ang){};
  Servo servo;

  // state
  uint8_t direction;
};

#endif