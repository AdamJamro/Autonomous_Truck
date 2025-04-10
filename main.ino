#include "Wheels.h"
#include "Ticker.h"


const int EnB = 5;
const int In3 = 8;
const int In4 = 7;
const int EnA = 10;
const int In1 = 12;
const int In2 = 11;

Wheels* w;
volatile char cmd;

// byte LCDAddress = 0x27;
byte LCDAddress = 0x3F;

void setup() {
  // put your setup code here, to run once:
  // w.attach(7,8,5,12,11,10);
  // attach(r-fwd,r-bwd,r-analog,l-fwd,l-bwd,l-analog)
  w = Wheels::GetInstance();
  w->attach(In4, In3, EnB, In2, In1, EnA);
  w->attachLCD(LCDAddress, 16, 2);

  Serial.begin(9600);
  Serial.setTimeout(2000);


  Serial.println("Forward: WAD");
  Serial.println("Back: ZXC");
  Serial.println("Stop: S");



}

void loop() {
  while(Serial.available())
  {
    cmd = Serial.read();
    switch(cmd)
    {
      case 'w': w->forward(); break;
      case 'x': w->back(); break;
      case 'a': w->forwardLeft(); break;
      case 'd': w->forwardRight(); break;
      case 'z': w->backLeft(); break;
      case 'c': w->backRight(); break;
      case 's': w->stop(); break;
      case '1': w->setSpeedLeft(75); break;
      case '2': w->setSpeedLeft(200); break;
      case '9': w->setSpeedRight(75); break;
      case '0': w->setSpeedRight(200); break;
      case '5': w->setSpeed(100); break;
      case 'p': w->goForward(200); break;
      case 'l': w->goBack(200); break;
    }
  }

    w->runAnimationFrame();
}
