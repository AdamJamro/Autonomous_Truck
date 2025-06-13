#ifndef InterruptManager_h
#define InterruptManager_h

#include <Arduino.h>

class InterruptManager {
public:
  InterruptManager() = delete;
  static void enablePCINT1();
  static void disablePCINT1();
};

#endif