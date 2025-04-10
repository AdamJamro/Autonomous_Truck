/* 
 * prosta implementacja klasy obsługującej 
 * silniki pojazdu za pośrednictwem modułu L298
 *
 * Sterowanie odbywa się przez:
 * 1)  powiązanie odpowiednich pinów I/O Arduino metodą attach() 
 * 2)  ustalenie prędkości setSpeed*()
 * 3)  wywołanie funkcji ruchu
 *
 * TODO:
 *  - zabezpieczenie przed ruchem bez attach()
 *  - ustawienie domyślnej prędkości != 0
 */


#include <Arduino.h>

#ifndef Wheels_h
#define Wheels_h

#include "LiquidCrystal_I2C.h"


class Wheels {
    public: 
        static Wheels* GetInstance();
        static void DeleteInstance();
        ~Wheels();
        /*
         *  pinForward - wejście "naprzód" L298
         *  pinBack    - wejście "wstecz" L298
         *  pinSpeed   - wejście "enable/PWM" L298
         */
        void attachRight(int pinForward, int pinBack, int pinSpeed);
        void attachLeft(int pinForward, int pinBack, int pinSpeed);
        void attach(int pinRightForward, int pinRightBack, int pinRightSpeed,
                    int pinLeftForward, int pinLeftBack, int pinLeftSpeed);
        void attachLCD(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
        
        /*
         *  funkcje ruchu
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
         *  ustawienie prędkości obrotowej (przez PWM)
         *   - minimalna efektywna wartość 60
         *      może zależeć od stanu naładowania baterii
         */
        void setSpeed(uint8_t);
        void setSpeedRight(uint8_t);
        void setSpeedLeft(uint8_t);
        
        void goBack(int);
        void goForward(int);
        void goGeneral(int, uint8_t);

        void updateSpeedDisplay(); // handles dashboard speed display on/off logic
        void printSpeed(int); // clears and prints the intput into the dashboard onto speedDisplayPos[]
        void runAnimationFrame(); 
        void setFrameRate(unsigned long);
        static void InterruptHandler(const Wheels* instance) { 
            if (instance == nullptr) return;
            instance->toggleBeeper(); 
        }
        void toggleBeeper(); // alternating the beeper bit
        void updateBeeper(); // on/off logic

    private: 
        Wheels();
        Wheels(const Wheels&) = delete;
        Wheels& operator=(const Wheels&) = delete;
        int pinsRight[3];
        int pinsLeft[3];
        uint8_t speedLeft = 0;
        uint8_t speedRight = 0;
        int directionLeft = 0; // backwords -1, forwards +1, stop 0
        int directionRight = 0;  
        LiquidCrystal_I2C* lcd;
        int speedDisplayPos[2] = {7, 0};
        int animationPos[2] = {7, 1};
        int arrowAssetSize;
        unsigned long frameRate = 30; // 33fps
        static inline Wheels* instance = nullptr;
        unsigned long beeperRate = 1000000; // 1 second
        uint8_t beeperState = 0;
        void handleMovementSideeffects();
};



#endif
