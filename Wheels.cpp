#include "Wheels.h"

#define BEEPER 13
#define SET_MOVEMENT(side,f,b) digitalWrite(side[0], f);\
                               digitalWrite(side[1], b)

Wheels* Wheels::GetInstance() {
    static Wheels instance;
    return &instance;
}

Wheels::Wheels() {
    pinMode(BEEPER, OUTPUT);
    // Timer1.initialize();
    // this->instance = this;
}

Wheels::~Wheels() {
    if (this->lcd) {
        delete this->lcd;
    }
    // if (this->sonar) {
        // delete this->sonar;
    // }
}

void Wheels::setFrameRate(const unsigned long fr) {
    // 1000 / fps = fr
    // 1000 / fr  = fps
    this->frameRate = max(1000 / 30, fr);
}

LiquidCrystal_I2C* Wheels::attachLCD(byte lcd_Addr, byte lcd_cols = 16, byte lcd_rows = 2) {
    this->lcd = new LiquidCrystal_I2C(lcd_Addr, lcd_cols, lcd_rows);
    this->lcd->init();
    this->lcd->backlight();
    fetchAssets(this->lcd);
    this->lcd->clear();
    this->lcd->setCursor(0, 0);
    return this->lcd;
}

// void Wheels::attachSonar(byte trig_addr, byte echo_addr, byte servo_addr) {
    // this->sonar = new Sonar(trig_addr, echo_addr, servo_addr, Wheels::updateSonarDisplay);
    // Serial.println("sonar attached");
// }

void Wheels::attachRight(byte pF, byte pB, byte pS) {
    pinMode(pF, OUTPUT);
    pinMode(pB, OUTPUT);
    pinMode(pS, OUTPUT);
    this->pinsRight[0] = pF;
    this->pinsRight[1] = pB;
    this->pinsRight[2] = pS;
}

void Wheels::attachLeft(byte pF, byte pB, byte pS) {
    pinMode(pF, OUTPUT);
    pinMode(pB, OUTPUT);
    pinMode(pS, OUTPUT);
    this->pinsLeft[0] = pF;
    this->pinsLeft[1] = pB;
    this->pinsLeft[2] = pS;
}

void Wheels::attachWheels(byte pRF, byte pRB, byte pRS, byte pLF, byte pLB, byte pLS) {
    this->attachRight(pRF, pRB, pRS);
    this->attachLeft(pLF, pLB, pLS);
}

void Wheels::setSpeedRight(uint32_t s) {
    this->speedRight = s;
    analogWrite(this->pinsRight[2], s);
    this->handleMovementSideeffects();
}

void Wheels::setSpeedLeft(uint32_t s) {
    this->speedLeft = s;
    analogWrite(this->pinsLeft[2], s);
    this->handleMovementSideeffects();
}

void Wheels::setSpeed(uint32_t s) {
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

uint32_t Wheels::getBraceCountersSum(){
    return leftBraceCounter + rightBraceCounter;
}

uint8_t Wheels::getCurrentMaxSpeed(){
    return (this->speedLeft > this->speedRight) ? this->speedLeft : this->speedRight;
}

void Wheels::resetBraceCounters(){
    leftBraceCounter = 0;
    rightBraceCounter = 0;
}

// result codes:
// 0 : succeeded via the usual flow
// 1 : was interrupted and finished prematurely 
int Wheels::goGeneral(
    const int64_t cm, 
    const uint8_t speed, 
    void (*onStart)() = [](){},
    int (*onRepeat)() = [](){ return 0; },
    void (*onFinish)() = [](){}
) {
    if (speed == 0) {
        return;
    }

    leftBraceCounter = 0;
    rightBraceCounter = 0;
    unsigned long distanceToTravel = abs(cm) * BRACE_PER_DISTANCE;
    unsigned long distanceTraveled = 0;

    this->stop();
    this->setSpeed(speed);

    if (cm > 0) {
        this->forward();
    } else if (cm < 0) {
        this->back();
    } else {
        return;
    }
    
    // unsigned long previousStart = 0;
    // unsigned long elapsedTime = 0;
    // unsigned long currentTime = millis();

    (*onStart)();

    while (distanceTraveled < 2 * distanceToTravel) {
        // this->runAnimationFrame(); // TODO set a timeout
        auto resultCode = (*onRepeat)();
        if (resultCode != 0) {
            return resultCode;
        } // do some task in background if provided/necessary
        distanceTraveled = leftBraceCounter + rightBraceCounter;
    }

    (*onFinish)();

    this->stop();
    return 0;
}


// TODO: make it a wrapper of the goGeneral(...)
void Wheels::animationGoGeneral(const int64_t cm, const uint8_t speed, void (*callback)() = [](){}) {
    if (speed == 0) {
        return;
    }

    leftBraceCounter = 0;
    rightBraceCounter = 0;
    unsigned long distanceToTravel = abs(cm) * BRACE_PER_DISTANCE; // TODO debug!
    unsigned long distanceTraveled = 0;

    this->stop();
    this->setSpeed(speed);
    if (cm > 0) {
        this->forward();
    } else if (cm < 0) {
        this->back();
    } else {
        return;
    }
    
    unsigned long previousStart = 0;
    unsigned long elapsedTime = 0;
    unsigned long currentTime = millis();

    int cursorStartPos = 0;
    int arrow_max_length = 16; // entire screen width

    this->lcd->setCursor(cursorStartPos, 0);
    for (int i = 0; i < arrow_max_length - 2; i++) { // TODO debug!!!
          this->lcd->print("=");
    }

    this->lcd->print(">");
    while (distanceTraveled < 2 * distanceToTravel) {
        // busy wait until next frame
        
        currentTime = millis();
        elapsedTime = currentTime - previousStart;
        
        // in case of busy waiting, we check if we should further continue to drive
        distanceTraveled = leftBraceCounter + rightBraceCounter;
        if (elapsedTime < this->frameRate) {
            (*callback)(); // do some task in background if provided/necessary
            // the callback ideally should abstract away everything below in a generel case
            // but since this project is small we stick to the monolith architecture

            // even though the framerate is the same here and for this animation we envoke here, 
            // we are busy waiting either way so we could utilize spare time to occasionally play a frame
            this->runAnimationFrame(); 

            continue;
        }
        previousStart = currentTime;

        
        // setup loop logic
        int arrow_length = arrow_max_length - (float(distanceTraveled) / float(2 * distanceToTravel)) * float(arrow_max_length);
        
        // DEBUG INFORMATION
        Serial.println(arrow_length);
        Serial.println("#");
    

        // perform state based action
        this->lcd->setCursor(cursorStartPos + arrow_length, 0);
        this->lcd->print(">");

        for (int i = 1; i < arrow_max_length - arrow_length; i++) {
            this->lcd->setCursor(cursorStartPos + arrow_length + i, 0);
            this->lcd->print(" ");
        } // TODO: replace with just print("> ");
    }
    
    // cleanup the animation debris
    this->lcd->setCursor(cursorStartPos,0);
    for (int i = 0; i < arrow_max_length; i++) {
        this->lcd->setCursor(cursorStartPos + i, 0);
        this->lcd->print(" ");
    }

    this->stop();
}

void Wheels::goForward(const uint64_t cm) {
    const uint8_t speed = speedLeft > speedRight ? speedLeft : speedRight;
    this->goGeneral(cm, speed);
}

void Wheels::goBack(const uint64_t cm) {
    const uint8_t speed = speedLeft > speedRight ? speedLeft : speedRight;
    this->goGeneral(-cm, speed);
}

int Wheels::turnGeneral(const int signed_angle) {
    const uint8_t angle = abs(signed_angle);
    static const uint8_t speed = 250;
    static const double magicConstant = 0.1;
    const uint32_t distanceToTravel = angle * magicConstant * BRACE_PER_DISTANCE;
    this->stop();
    this->setSpeed(speed);

    leftBraceCounter = 0;
    rightBraceCounter = 0;
    unsigned long distanceTraveled = 0;
    
    if (signed_angle > 0) { // turn Left
        this->backLeft();
        this->forwardRight();
    }
    else if (signed_angle < 0) { // turn right
        this->backRight();
        this->forwardLeft();
    }

    while (distanceTraveled < 2 * distanceToTravel) {
        distanceTraveled = leftBraceCounter + rightBraceCounter;
    }

    // stop
    this->stop();
    return 0;
}

void Wheels::turnRight(const uint8_t angle) {
    const int signed_angle = -angle;
    this->turnGeneral(signed_angle);
}

void Wheels::turnLeft(const uint8_t angle) {
    this->turnGeneral(angle);
}

// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// BEEPER

//DEPRECATED
// void Wheels::updateBeeper() {
//     return;

//     static const uint32_t second = 1000000;
//     Timer1.detachInterrupt();
//     if (this->beeperState == 1) {
//         digitalWrite(BEEPER, 0);
//     }

//     if (this->directionLeft == -1 && this->directionRight == -1) {
//         if (this->speedLeft + this->speedRight > 0) {
//             Serial.println("#");
//             Serial.println(this->speedLeft);
//             Serial.println(this->speedRight);
            
//             this->beeperRate = min(max(second - second * (this->speedLeft + this->speedRight) / 400, second / 10), second);
//             // this->beeperRate = second;
//             Timer1.attachInterrupt(Wheels::InterruptHandler, this->beeperRate);
//         }
//     }
// }

// void Wheels::toggleBeeper() {
//     // the callback of backing beeper
//     this->beeperState = this->beeperState ^ 1;
//     digitalWrite(BEEPER, this->beeperState);
// }


// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// ISR interrupts

void Wheels::saveBraceCounters() {
    this->leftBraceCounterCache = this->leftBraceCounter;
    this->rightBraceCounterCache = this->rightBraceCounter;
}

void Wheels::restoreBraceCounters() {
    this->leftBraceCounter = this->leftBraceCounterCache;
    this->rightBraceCounter = this->rightBraceCounterCache;
}

void Wheels::handleISR(){
    // brace counting:
    const byte LPS = (PINC & (1 << PC0));
    const byte RPS = (PINC & (1 << PC1));
    if( (this->leftBracePinState != LPS) ) {
        this->leftBracePinState = LPS;
        this->leftBraceCounter++;
    }
    if( this->rightBracePinState != RPS ) {
        this->rightBracePinState = RPS;
        this->rightBraceCounter++;
    }
}
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// LCD ANIMATIONS

void Wheels::handleMovementSideeffects() {
    this->updateSpeedDisplay();
    // this->updateBeeper(); //deprecated
}

static void Wheels::updateSonarDisplay(const uint16_t distance, const uint8_t angle) {
    auto instance = Wheels::GetInstance();
    // TODO debug

    // Serial.println("updating sonar display");
    // Serial.println(distance);
    // Serial.println(angle);

    if (distance > 0) {
        instance->lcd->setCursor(0,0);
        instance->lcd->print("    ");
        instance->lcd->setCursor(0,0);
        instance->lcd->print(distance);
    } else {
        // instance->lcd->print("???");
    }

    instance->lcd->setCursor(13,0);
    instance->lcd->print("  ");
    if (angle < 10) {
        instance->lcd->setCursor(15, 0);
    } else if (angle < 100) {
        instance->lcd->setCursor(14, 0);
    } else {
        instance->lcd->setCursor(13, 0);
    }
    instance->lcd->print(angle);
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
    this->lcd->print("     ");
    this->lcd->setCursor(this->speedDisplayPos[0], this->speedDisplayPos[1]);
    this->lcd->print(displaySpeed);
}

void Wheels::runAnimationFrame() {
    static Timeout frameTimer;
    static int frame = 0;
    // static unsigned long previousStart = 0;
    // const unsigned long currentTime = millis();
    // const unsigned long elapsedTime = currentTime - previousStart;
    if (!frameTimer.periodic(this->frameRate)){
        return;
    }
    // previousStart = millis();
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
    this->lcd->setCursor(this->animationPos[0] + 1, this->animationPos[1]);
    if (this->directionRight != 0) {
        switch (this->directionRight) {
            case -1: offset = backwardAnimationOffset; break;
            case 1: offset = forwardAnimationOffset; break;
            case 0: offset = standbyAnimationOffset; break;
        }
        this->lcd->write((currentFrame/animationSpeedFrame) % numberOfFrames + offset);
    } // else { this->lcd->print(" "); } 
    
    // Serial.print("offset: ");
    // Serial.println(offset);
    // Serial.print("freame: ");
    // Serial.println((frame/animationSpeedFrame) % numberOfFrames + offset);
}
