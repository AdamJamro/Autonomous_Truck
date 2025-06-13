#include "Sonar.h"


void Sonar::attach(byte trig_addr, byte echo_addr, byte servo_addr, void (*updateCallback)(uint16_t, uint8_t) = [](){}) {
  this->TRIG = trig_addr;
  this->ECHO = echo_addr;
  pinMode(TRIG, OUTPUT);    // TRIG startuje sonar
  pinMode(ECHO, INPUT);     // ECHO odbiera powracający impuls

  this->SERVADDR = servo_addr;
  this->sonarUpdateCallback = updateCallback;
  this->servo.attach(servo_addr);
  setDirection(90);
}

uint64_t Sonar::getDistance() {
  unsigned long tot;      // time-of-travel
  unsigned int distance;

  // Serial.print("Patrzę w kącie ");
  // Serial.print(this->direction);
  
/* uruchamia sonar (puls 10 ms na `TRIGGER')
 * oczekuje na powrotny sygnał i aktualizuje
 */
  digitalWrite(TRIG, HIGH);
  delay(10);
  digitalWrite(TRIG, LOW);
  tot = pulseIn(ECHO, HIGH);

/* prędkość dźwięku = 340m/s => 1 cm w 29 mikrosekund
 * droga tam i z powrotem, zatem:
 */
  distance = tot/58;
  // Serial.print("DEBUG distance:");
  // Serial.println(distance);

  this->sonarUpdateCallback(distance, this->direction);
  return distance;
}

void Sonar::setDirection(uint8_t angle){
  this->servo.write(angle);
  // analogWrite(this->SERVADDR, angle * 255 / 180);
  // Serial.print("write angle:");
  // Serial.print(angle);
  this->direction = angle;
  this->sonarUpdateCallback(0, this->direction);
}

uint8_t Sonar::getDirection() {
  return this->direction;
}
