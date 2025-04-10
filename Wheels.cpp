#include <Arduino.h>

#include "Wheels.h"
#include "LiquidCrystal_I2C.h"
#include "Assets.h"
#include "TimerOne.h"


#define BEEPER 13
#define SET_MOVEMENT(side,f,b) digitalWrite(side[0], f);\
                               digitalWrite(side[1], b)

Wheels* Wheels::GetInstance() {
    if (Wheels::instance == nullptr) {
        instance = new Wheels();
    }
    return instance;
}

void Wheels::DeleteInstance() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

Wheels::Wheels() {
    pinMode(BEEPER, OUTPUT);
    this->instance = this;
}

void Wheels::attachLCD(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows) {
    this->lcd = new LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows);
    this->lcd->init();
    this->lcd->backlight();
    fetchAssets(this->lcd);
    this->lcd->clear();
    this->lcd->setCursor(0, 0);
}

Wheels::~Wheels() {
    if (this->lcd) {
        delete this->lcd;
    }
}

// 1000 / fps = fr
void Wheels::setFrameRate(const unsigned long fr) {
    this->frameRate = max(1000 / 30, fr);
}

void Wheels::attachRight(int pF, int pB, int pS) {
    pinMode(pF, OUTPUT);
    pinMode(pB, OUTPUT);
    pinMode(pS, OUTPUT);
    this->pinsRight[0] = pF;
    this->pinsRight[1] = pB;
    this->pinsRight[2] = pS;
}

void Wheels::attachLeft(int pF, int pB, int pS) {
    pinMode(pF, OUTPUT);
    pinMode(pB, OUTPUT);
    pinMode(pS, OUTPUT);
    this->pinsLeft[0] = pF;
    this->pinsLeft[1] = pB;
    this->pinsLeft[2] = pS;
}

void Wheels::attach(int pRF, int pRB, int pRS, int pLF, int pLB, int pLS) {
    this->attachRight(pRF, pRB, pRS);
    this->attachLeft(pLF, pLB, pLS);
}

void Wheels::setSpeedRight(uint8_t s) {
    this->speedRight = s;
    analogWrite(this->pinsRight[2], s);
    this->handleMovementSideeffects();
}

void Wheels::setSpeedLeft(uint8_t s) {
    this->speedLeft = s;
    analogWrite(this->pinsLeft[2], s);
    this->handleMovementSideeffects();
}

void Wheels::setSpeed(uint8_t s) {
    this->setSpeedLeft(s);
    this->setSpeedRight(s);
}

void Wheels::forwardLeft() {
    this->directionLeft = 1;
    SET_MOVEMENT(pinsLeft, HIGH, LOW);
    this->lcd->setCursor(0,1);
    this->lcd->print("F");
    this->handleMovementSideeffects();
}

void Wheels::forwardRight() {
    this->directionRight = 1;
    SET_MOVEMENT(pinsRight, HIGH, LOW);
    this->lcd->setCursor(15,1);
    this->lcd->print("F");
    this->handleMovementSideeffects();
}

void Wheels::forward() {
    this->forwardLeft();
    this->forwardRight();
}

void Wheels::backLeft() {
    this->directionLeft = -1;
    SET_MOVEMENT(pinsLeft, LOW, HIGH);
    this->lcd->setCursor(0,1);
    this->lcd->print("B");
    this->handleMovementSideeffects();
}

void Wheels::backRight() {
    this->directionRight = -1;
    SET_MOVEMENT(pinsRight, LOW, HIGH);
    this->lcd->setCursor(15,1);
    this->lcd->print("B");
    this->handleMovementSideeffects();
}

void Wheels::back() {
    this->backLeft();
    this->backRight();  
}

void Wheels::stopLeft() {
    this->directionLeft = 0;
    SET_MOVEMENT(pinsLeft, LOW, LOW);
    this->lcd->setCursor(0,1);
    this->lcd->print("S");
    this->handleMovementSideeffects();
}

void Wheels::stopRight() {
    this->directionRight = 0;
    SET_MOVEMENT(pinsRight, LOW, LOW);
    this->lcd->setCursor(15,1);
    this->lcd->print("S");
    this->handleMovementSideeffects();
}

void Wheels::stop() {
    this->stopLeft();
    this->stopRight();
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// CUSTOM MOVEMENT

void Wheels::goForward(const int cm) {
    const uint8_t speed = speedLeft > speedRight ? speedLeft : speedRight;
    this->goGeneral(cm, speed);
}

void Wheels::goBack(int cm) {
    const uint8_t speed = speedLeft > speedRight ? speedLeft : speedRight;
    this->goGeneral(-cm, speed);
}

void Wheels::goGeneral(const int cm, const uint8_t speed) {
    const unsigned long duration = abs((5000 * cm) / speed ) + 100;

    this->stop();
    this->setSpeed(speed);
    if (cm > 0) {
      this->forward();
    } else if (cm < 0) {
      this->back();
    }
    
    // const unsigned long deltaTime = max((duration) / abs(cm), 5);
    const unsigned long deltaTime = this->frameRate;
    long timeLeft = duration;
    int startPos = 0;
    int arrow_max_length = 16;
    this->lcd->setCursor(startPos, 0);
    for (int j = 0; j < arrow_max_length - 2; j++) {
          this->lcd->print("=");
    }
    this->lcd->print(">");

    for (unsigned long i = 0; i < duration; i += deltaTime) {
      int arrow_length = float(timeLeft) / float(duration) * float(arrow_max_length);
      Serial.println(arrow_length);
      Serial.println(timeLeft);
      Serial.println(duration);
      Serial.println("#");
      this->lcd->setCursor(startPos + arrow_length, 0);
      this->lcd->print(">");

      for (int j = 1; j < arrow_max_length - arrow_length; j++){
        this->lcd->setCursor(startPos + arrow_length + j, 0);
        this->lcd->print("  ");
      }
      timeLeft = long(max(timeLeft - long(deltaTime), 0));
      delay(deltaTime);
      this->runAnimationFrame(); // since this is pretty lengthy 
    }
    this->lcd->setCursor(startPos,0);
    this->lcd->print("    ");

    this->stop();
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// BEEPER

void Wheels::updateBeeper() {
    if (this->directionLeft == -1 && this->directionRight == -1) {
        if (this->speedLeft + this->speedRight > 0) {
            Timer1.attachInterrupt(Wheels::InterruptHandler, this->beeperRate);
        }
    } else {
        Timer1.detachInterrupt();
    }
}

void Wheels::toggleBeeper() {
    // the callback of backing beeper
    this->beeperState = this->beeperState ^ 1;
    digitalWrite(BEEPER, this->beeperState);
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// LCD ANIMATIONS

void Wheels::handleMovementSideeffects() {
    this->updateSpeedDisplay();
    this->updateBeeper();
}

void Wheels::updateSpeedDisplay() {
    int displaySpeed = max(this->speedLeft, this->speedRight);
 
    if (this->directionLeft == -1 && this->directionRight == -1) {
        displaySpeed = -displaySpeed;
    } else if (this->directionLeft == -1) {
        displaySpeed = this->speedRight;
    } else if (this->directionRight == -1) {
        displaySpeed = this->speedLeft;
    }

    if (this->directionLeft == 0 && this->directionRight == 0) {
        displaySpeed = 0;
    }
    printSpeed(displaySpeed);
}

void Wheels::printSpeed(const int displaySpeed) {
    this->lcd->setCursor(this->speedDisplayPos[0], this->speedDisplayPos[1]);
    this->lcd->print("                       ");
    this->lcd->setCursor(this->speedDisplayPos[0], this->speedDisplayPos[1]);
    this->lcd->print(displaySpeed);
}

void Wheels::runAnimationFrame() {
    static int frame = 0;
    static unsigned long previousStart = 0;
    const unsigned long currentTime = millis();
    const unsigned long elapsedTime = currentTime - previousStart;
    if (elapsedTime > this->frameRate){
        previousStart = millis();
        const int currentFrame = frame;
        frame = (frame + 1);
        
        int offset = 0;
        // run left side animation
        this->lcd->setCursor(this->animationPos[0], this->animationPos[1]);
        if (this->directionLeft != 0) {
            switch (this->directionLeft) {
                case -1: offset = backwardAnimationOffset; break;
                case 1: offset = forwardAnimationOffset; break;
                case 0: offset = standbyAnimationOffset; break;
            }
            this->lcd->write((currentFrame/animationSpeedFrame) % numberOfFrames + offset);
        } //else { this->lcd->print(" "); } do this to opt out of freezing behavior
        
        // run right side animation
        this->lcd->write((currentFrame/animationSpeedFrame) % numberOfFrames + offset);
        if (this->directionRight != 0) {
            switch (this->directionRight) {
                case -1: offset = backwardAnimationOffset; break;
                case 1: offset = forwardAnimationOffset; break;
                case 0: offset = standbyAnimationOffset; break;
            }
            this->lcd->setCursor(this->animationPos[0] + 1, this->animationPos[1]);
        } // else { this->lcd->print(" "); } 
        
        // Serial.print("offset: ");
        // Serial.println(offset);
        // Serial.print("freame: ");
        // Serial.println((frame/animationSpeedFrame) % numberOfFrames + offset);
    }
}
