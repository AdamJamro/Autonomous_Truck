/* 
 * Allows steering of the truck including engines work, lcd display, accepting signal commands via a remote, and echo sonar support
 * (since we create a monolith that is not designed to do anything else, 
 * the fact of making it a separate class is pretty much needless but it's something that happened
 * and I'm not about to refactor this)
 * 
 * TODO: maybe encapsulate work of the engines' L298 module to a separate class component
 *
 * USAGE:
 * 1)  bind adequate I/O PINs with attach_____() where blank stands for Wheels, LCD, and Sonar 
 * 2)  in order to make custom movement menthods to work properly you need to hook ISR handler inside the ISR(){...} routine
 * 3)  apart from that just look up the public methods this header declares
 */


#ifndef Wheels_h
#define Wheels_h

#include "LiquidCrystal_I2C.h"
#include "Sonar.h"
#include "Assets.h"
#include "Timeout.h"


#define WHEEL_CIRCUMFERENCE 10  // cm
#define NUMBER_OF_BRACES 20
// #define addr uint8_t
#define BRACE_PER_DISTANCE (float(NUMBER_OF_BRACES) / WHEEL_CIRCUMFERENCE)
// LCD operates inside 2x16 grid
#define LCD_COLS 16
#define LCD_ROWS 2


class Wheels {
public:
    static Wheels* GetInstance();
    ~Wheels();

    /*
        *  pinForward - PIN "forward" L298
        *  pinBack    - PIN "backward" L298
        *  pinSpeed   - PIN "enable/PWM" L298
        */
    void attachRight(byte pinForward, byte pinBack, byte pinSpeed);
    void attachLeft(byte pinForward, byte pinBack, byte pinSpeed);
    void attachWheels(byte pinRightForward, byte pinRightBack, byte pinRightSpeed,
                byte pinLeftForward, byte pinLeftBack, byte pinLeftSpeed);
    LiquidCrystal_I2C* attachLCD(byte lcd_Addr, byte lcd_cols, byte lcd_rows);
    // void attachSonar(byte thig_addr, byte echo_addr, byte servo_addr);

    /*
        *  motion
        */
    void forward();
    void forwardLeft();
    void forwardRight();
    void back();
    void backLeft();
    void backRight();
    void stop();
    void stopLeft();
    void stopRight();
    /*
        *  handling PWM pins
        *   - minimal effective value is 60
        */
    void setSpeed(uint32_t);
    void setSpeedRight(uint32_t);
    void setSpeedLeft(uint32_t);

    // custom movement
    uint8_t getCurrentMaxSpeed();
    void resetBraceCounters();
    void saveBraceCounters();
    void restoreBraceCounters();
    int goGeneral(int64_t, uint8_t, void (*onStart)(), int (*onRepeat)(), void (*onFinish)());
    void goForward(uint64_t);
    void goBack(uint64_t);
    void animationGoGeneral(int64_t, uint8_t, void(*f)()); // TODO: maybe deprecate
    int goCollisionSafe(int64_t);
    int obviate(const int64_t = 50);
// custom movement: rotation
    uint32_t getBraceCountersSum();
    int turnGeneral(int);
    void turnRight(uint8_t);
    void turnLeft(uint8_t);

    // LCD display
    void setFrameRate(unsigned long);
    void updateSpeedDisplay(); // handles dashboard speed display on/off logic
    static void updateSonarDisplay(uint16_t, uint8_t); // handles dashboard sonar data
    void printSpeed(int); // clears and prints the intput into the dashboard onto speedDisplayPos[]
    void handleMovementSideeffects();
    void runAnimationFrame(); // the arrowy animation of the motion (from the assets)

    // TimerOne: 
    // DO NOT USE IT it interferes with servo aswell
    // NOTE: interferes with arduino 9 and 10 PINS!
    // void toggleBeeper(); // simply switches the beeper bit (BEEPER =xor 1)
    // void updateBeeper(); // calls Timer1 on/off logic 
    // static void InterruptHandler() {
        // Wheels::GetInstance()->toggleBeeper(); 
    // }

    // ISR - Interrupt Service Routine
    void handleISR(); // needs to be called inside ISR routine
    volatile uint32_t leftBraceCounter = 0;
    volatile uint32_t rightBraceCounter = 0;
    uint32_t leftBraceCounterCache = 0, rightBraceCounterCache = 0;
    

    uint32_t frameRate = 30; // 33fps
    uint32_t sonarUpdateRate = 200; // miliseconds 
    uint32_t beeperRate = 1000000; // 1 second

private: 
    Wheels();
    Wheels(const Wheels&) = delete;
    Wheels& operator=(const Wheels&) = delete;
    int pinsRight[3];
    int pinsLeft[3];
    int speedDisplayPos[2] = {7, 0};
    int animationPos[2] = {7, 1};
    int arrowAssetSize;
    LiquidCrystal_I2C const * lcd;

    //state
    uint32_t speedLeft = 0;
    uint32_t speedRight = 0;
    int directionLeft = 0; // backwords -1, forwards +1, stop 0
    int directionRight = 0;
    uint8_t beeperState = 0;
    uint8_t leftBracePinState = 0;
    uint8_t rightBracePinState = 0;

};

#endif