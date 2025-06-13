#include <Arduino.h>
#include "Wheels.h"
#include "InterruptManager.h"
#include "Timeout.h"
#include <IRremote.hpp>
#include "keymap.hpp"

#define IR_RECEIVE_PIN 2
#define TRIG A2
#define ECHO A3
#define SERVO 11
#define EnA 6
#define In1 7
#define In2 8
#define In3 12
#define In4 4
#define EnB 5

Wheels* w;
LiquidCrystal_I2C* lcd;
Sonar sonar; 

static Timeout sonarTimer;
static Timeout animationTimer;
static Timeout irTimer;
// Sonar sonar;
volatile char cmd;

byte LCDAddress = 0x27;
// byte LCDAddress = 0x3F;


// MAIN METHODS
int goCollisionSafe(const int64_t cm);
int obviate(const int64_t minDistance, Wheels* w);
// void handleKeyboardControl();
void handleRemoteAuth();
void handleRemoteControl();


// SETUP
void setup() {
        
    Serial.begin(9600);
    while(!Serial); // TODO: DEBUG
  
  // INFRARED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    // Wheels: attach(r-fwd,r-bwd,r-analog,l-fwd,l-bwd,l-analog)
    w = Wheels::GetInstance();
    w->attachWheels(In4, In3, EnB, In2, In1, EnA);
    lcd = w->attachLCD(LCDAddress, 16, 2);
    sonar.attach(TRIG, ECHO, SERVO, Wheels::updateSonarDisplay);


    InterruptManager::enablePCINT1();

  // Display Menu
//   Serial.println("Forward: WAD");
//   Serial.println("Back: ZXC");
//   Serial.println("Stop: S");
    handleRemoteAuth();

}

void loop() {
//   deprecated
//   while(Serial.available()) {handleKeyboardControl(cmd);}
    

  // TODO delete:
    if (sonarTimer.periodic(400)) {
        sonar.getDistance();
    }

    if (irTimer.periodic(350)) {
        handleRemoteControl();  // has its own timer
    }
    w->runAnimationFrame(); // has its own timer for some reason
}


ISR(PCINT1_vect){
 if (w) {
  w->handleISR();
 }
}


int goCollisionSafe(const int64_t cm) {
    static const uint8_t minDistance = 70; 
    static const Timeout sensorTimer;
    const uint8_t speed = w->getCurrentMaxSpeed();

    if (speed == 0) {
        return;
    }

    w->resetBraceCounters(); 
    unsigned long distanceToTravel = abs(cm) * BRACE_PER_DISTANCE;
    unsigned long distanceTraveled = 0;

    w->stop();
    w->setSpeed(speed);

    if (cm > 0) {
        w->forward();
    } else if (cm < 0) {
        w->back();
    } else {
        return;
    }
    
    unsigned long previousStart = 0;
    unsigned long elapsedTime = 0;
    unsigned long currentTime = millis();

    
    sonar.setDirection(90); // face forward
    Serial.println("onStart callback!");

    while (distanceTraveled < 2 * distanceToTravel) {
        // this->runAnimationFrame(); // TODO set a timeout

        int returnCode = 0;
        if (sensorTimer.periodic(w->sonarUpdateRate)) {
            Serial.println("onrepeat TRYING TO GET DISTANCE:");
            const auto newDistance = sonar.getDistance();
            Serial.print("Got distance: ");
            Serial.println(newDistance);
            if (newDistance < minDistance && newDistance != 0) {
                Serial.print("FOUND OBSTACLE! distance: ");
                Serial.println(newDistance);
                w->stop();
                // return 1;
                returnCode = obviate(30); // blocking
                Serial.println("OBSTACLE DEFEATED. I PROCEED");
                w->forward();
            }
        }
        distanceTraveled = w->getBraceCountersSum();
    }

    Serial.println("I finished");
    w->stop();
    return 0;
}

// result codes:
// 0: succeeded via the default flow
// 1: 
// 2: observed a dead-end (left and right echo is less than minimal)
int obviate(const int64_t minDistance) {
    w->saveBraceCounters();

    Serial.print("Initialize obviation\n");

    // go around the obstacle
    const static uint8_t maxAngle = 70;

    sonar.setDirection(90 - maxAngle); // look on rightside
    delay(1000);
    auto distanceLeft = sonar.getDistance();

    sonar.setDirection(90 + maxAngle); // look on leftside
    delay(1000);
    auto distanceRight = sonar.getDistance();
    
    sonar.setDirection(90);

    // FOR NOW;
    if (distanceLeft < minDistance && distanceRight < minDistance && distanceLeft * distanceRight != 0) {
        w->goBack(10);
        w->turnLeft(45); 
    } else if (distanceLeft < distanceRight || distanceLeft == 0) {
        w->turnLeft(90);
    } else {
        w->turnRight(90);
    }
    Serial.print("finalize obviation\n");
    w->restoreBraceCounters();
    return 0;
    // temporary flow.

    // TODO implement vector logic:
    // while (destinationVector.x + destinationVector.y > 10) {
        // if (distanceLeft < minDistance && distanceRight < minDistance) {
            // turnLeft(180);
            // return 2;
        // }

        // TODO: try to get closer to the destination
    // }
    // destinationVector = ;

    w->restoreBraceCounters();
    return 0;
}

void handleRemoteAuth() {
    const int pinLength = 4;
    lcd->setCursor(0, 0);
    lcd->print("PIN:");
    // static Timeout bufferTimer;
    static int pinCode[pinLength] = {BTN1,BTN2,BTN3,BTN4};
    int inputCode[pinLength] = {0,0,0,0};
    
    bool isAuthed = false;
    while(isAuthed == false) {
        Serial.println("Press \'OK\' Followed by the \'PIN\'");
        lcd->setCursor(0, 0);
        lcd->print("PIN:");
        for (int i = 0; i < pinLength; i++) {
            lcd->print(" ");
        }
    
        uint16_t command = OKBTN + 1; 
        while(command != OKBTN) {
            while (!IrReceiver.decode()) {
            }
            if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
                command = IrReceiver.decodedIRData.command;
                Serial.print("DEbug: command: ");
                Serial.println(command);
                // delay(250);
            }
            delay(250);
            IrReceiver.resume();
        }
        lcd->setCursor(0, 0);
        lcd->print("PIN:");
        for (int i = 0; i < pinLength; i++) {
            lcd->print("_");
        }
        for (int i = 0; i < pinLength; i++) {
            while (!IrReceiver.decode()) {
                delay(300);
            }
            if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
                command = IrReceiver.decodedIRData.command;
                if (command == 0) {
                    i--;
                    delay(100);
                    IrReceiver.resume();
                    continue;
                }
                Serial.print("DEbug2: command: ");
                Serial.println(command);
                inputCode[i] = command;
                lcd->setCursor(4 + i,0);
                lcd->print("*");
            }
            if (command == OKBTN) {
                delay(350);
                break;
            }
            delay(250);
            IrReceiver.resume();
        }
        isAuthed = true;
        for (int i = 0; i < pinLength; i++) {
            if (inputCode[i] != pinCode[i]) {
                isAuthed = false; 
            }
        }
        if (isAuthed == false) {
            Serial.println("INPUT does not match the PIN!");
        }
    }
    // CLEAR THE SCREEN
    lcd->setCursor(0, 0);
    lcd->print("    ");
    for (int i = 0; i < pinLength; i++) {
        lcd->print(" ");
    }
    Serial.println("AUTHED SUCCESSFULLY");
}

void handleRemoteControl() {
    if (IrReceiver.decode() && !(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
        uint16_t command = IrReceiver.decodedIRData.command;
        Serial.println(command);
        switch (command) {
            case OKBTN:     w->stop(); break;
            case LEFTBTN:   w->turnLeft(90); break;
            case RIGHTBTN:  w->turnRight(90); break;
            case UPBTN:     w->goForward(20); break;
            case DOWNBTN:   w->goBack(20); break;
            case HASHBTN:   w->setSpeed(80); break;
            case ASTERBTN:  w->setSpeed(200); break;
            case BTN5:      w->forward(); break;
            case BTN8:      w->back(); break;
            case BTN4:      w->forwardLeft(); break;
            case BTN6:      w->forwardRight(); break;
            case BTN7:      w->backLeft(); break;
            case BTN9:      w->backRight(); break;
            case BTN1:      goCollisionSafe(1000); break;
            case BTN2:
                break;
            case BTN3:
                break;
            default:
                Serial.print("UNKNOWN REMOTE COMMAND");
                Serial.print(command);
                break;
        }
    }
    IrReceiver.resume();
}

// void handleKeyboardControl() {
//     cmd = Serial.read();
//     switch(cmd)
//     {
//       case 'w': w->forward(); break;
//       case 'x': w->back(); break;
//       case 'a': w->forwardLeft(); break;
//       case 'd': w->forwardRight(); break;
//       case 'z': w->backLeft(); break;
//       case 'c': w->backRight(); break;
//       case 's': w->stop(); break;
//       case '1': w->setSpeedLeft(75); break;
//       case '2': w->setSpeedLeft(200); break;
//       case '9': w->setSpeedRight(75); break;
//       case '0': w->setSpeedRight(200); break;
//       case '5': w->setSpeed(100); break;
//       case 'p': w->goForward(10); break;
//       case 'l': w->goBack(10); break;
//       case 'e': w->turnLeft(90); break;
//       case 'r': w->turnRight(90); break;
//       case 'o': goCollisionSafe(1000); break;
//     }
// }
