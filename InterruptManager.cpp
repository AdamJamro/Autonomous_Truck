#include "InterruptManager.h"

static void InterruptManager::enablePCINT1() {
  PCICR  = 0x02;  // turn on listening to pin change interrupt for group 1 (A0..A5) (PCINT8..14)
  // PCICR |= (1 << PCIE1);

  PCMSK1 = 0x03;  // turn on the interrupts for pin A0, A1
  // PCMSK1 = 0;
  // PCMSK1 |= (1 << PCINT8); // A0
  // PCMSK1 |= (1 << PCINT9); // A1
}

static void InterruptManager::disablePCINT1() {
  PCICR &= -(1 << PCIE1);
}