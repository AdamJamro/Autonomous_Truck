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
    goHookSpring();
    // w->setSpeed(200);
    // w->forward();
    // delay(1000);
    // w->setSpeed(255);
    // delay(1000);
    // w->setSpeed(270);
    // delay(5000);
    // w->stop();
}

void loop() {
  // TODO delete:
    // if (sonarTimer.periodic(400)) {
    //     sonar.getDistance();
    // }

    // if (irTimer.periodic(350)) {
    //     handleRemoteControl();  // has its own timer
    // }
    // w->runAnimationFrame(); // has its own timer for some reason
}


ISR(PCINT1_vect){
 if (w) {
  w->handleISR();
 }
}


int goHookSpring() {
    static const Timeout sonarTimer;
    static const uint8_t equilibrium = 30;
    static const uint8_t error = 3; 
    static const uint8_t sonarUpdateRate = 1000;//w->sonarUpdateRate;
    static const uint8_t speedThreshold = 100;
    static const uint8_t springConstant = 30;
    int32_t previousSpeed = 0;
    int32_t speed = 0;

    w->stop();
    sonar.setDirection(90); // face forward
    w->setSpeed(speed);
    
    Serial.println("debug start");
    while (true) {
        if (!sonarTimer.periodic(sonarUpdateRate)) {
            continue;
        }
        uint32_t currentDistance = sonar.getDistance();
        
        if (abs(currentDistance - int32_t(equilibrium)) <= int32_t(error)) {
            speed = 0;
            w->setSpeed(speed);
            continue; // do nothing
        }
        Serial.print("springConstant:");
        Serial.println(springConstant);
        Serial.print("currentDistance");
        Serial.println(currentDistance);
        Serial.print("equilibrium");
        Serial.println(equilibrium);
        previousSpeed = speed;
        speed = max(-255, min(255, 255 * ((double(springConstant) * (double(currentDistance) - double(equilibrium)))/600))); 
        Serial.print("speed:");
        Serial.println(speed);
              
        if (speed > 0) {
            w->forward();
        } else {
            w->back();
        }
        w->setSpeed(abs(speed));


        if (abs(speed) < speedThreshold && abs(previousSpeed) < speedThreshold && abs(previousSpeed) < abs(speed)) {
            // boost the movement
            // const auto boostedSpeed = -log(speed);
            const auto boostedSpeed = 2 * (speedThreshold - abs(speed));
            w->setSpeed(boostedSpeed);
            delay(100);
            w->setSpeed(speed);
            previousSpeed = speed;
        }
    }
    Serial.println("debug end");

    w->stop();
    return 0;
}

// int obviate(const int64_t minDistance) {
//     w->saveBraceCounters();

//     Serial.print("Initialize obviation\n");

//     // go around the obstacle
//     const static uint8_t maxAngle = 70;

//     sonar.setDirection(90 - maxAngle); // look on rightside
//     delay(1000);
//     auto distanceLeft = sonar.getDistance();

//     sonar.setDirection(90 + maxAngle); // look on leftside
//     delay(1000);
//     auto distanceRight = sonar.getDistance();
    
//     sonar.setDirection(90);

//     // FOR NOW;
//     if (distanceLeft < minDistance && distanceRight < minDistance && distanceLeft * distanceRight != 0) {
//         w->goBack(10);
//         w->turnLeft(45); 
//     } else if (distanceLeft < distanceRight || distanceLeft == 0) {
//         w->turnLeft(90);
//     } else {
//         w->turnRight(90);
//     }
//     Serial.print("finalize obviation\n");
//     w->restoreBraceCounters();
//     return 0;
//     // temporary flow.

//     // TODO implement vector logic:
//     // while (destinationVector.x + destinationVector.y > 10) {
//         // if (distanceLeft < minDistance && distanceRight < minDistance) {
//             // turnLeft(180);
//             // return 2;
//         // }

//         // TODO: try to get closer to the destination
//     // }
//     // destinationVector = ;

//     w->restoreBraceCounters();
//     return 0;
// }

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
            case BTN1:      goHookSpring(); break;
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
