#include "Assets.h"

byte standbyAnimationAsset1[8] = {
  B00100,
  B01110,
  B11011,
  B00000,
  B00000,
  B11011,
  B01110,
  B00100
};

byte standbyAnimationAsset2[8] = {
  B00100,
  B01110,
  B11011,
  B00000,
  B00000,
  B11011,
  B01110,
  B00100
};

byte standbyAnimationAsset3[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};

byte standbyAnimationAsset4[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000  
};

byte forwardAnimationAsset1[8] {
  B10001,
  B00100,
  B01110,
  B11011,
  B10001,
  B00100,
  B01110,
  B11011
};

byte forwardAnimationAsset2[8] = {
  B00100,
  B01110,
  B11011,
  B10001,
  B00100,
  B01110,
  B11011,
  B10001
};

byte forwardAnimationAsset3[8] = {
  B01110,
  B11011,
  B10001,
  B00100,
  B01110,
  B11011,
  B10001,
  B00100
};

byte forwardAnimationAsset4[8] = {
  B11011,
  B10001,
  B00100,
  B01110,
  B11011,
  B10001,
  B00100,
  B01110
};



byte backwardAnimationAsset1[8] {
  B10001,
  B11011,
  B01110,
  B00100,
  B10001,
  B11011,
  B01110,
  B00100
};

byte backwardAnimationAsset2[8] = {
  B00100,
  B10001,
  B11011,
  B01110,
  B00100,
  B10001,
  B11011,
  B01110  
};

byte backwardAnimationAsset3[8] = {
  B01110,
  B00100,
  B10001,
  B11011,
  B01110,
  B00100,
  B10001,
  B11011
};

byte backwardAnimationAsset4[8] = {
  B11011,
  B01110,
  B00100,
  B10001,
  B11011,
  B01110,
  B00100,
  B10001
};

static void fetchAssets(const LiquidCrystal_I2C* lcd) {
    // fetch assets
    lcd->createChar(0, standbyAnimationAsset1);
    lcd->createChar(1, standbyAnimationAsset1);
    lcd->createChar(2, standbyAnimationAsset3);
    lcd->createChar(3, standbyAnimationAsset3);
    
    lcd->createChar(4, forwardAnimationAsset1);
    lcd->createChar(5, forwardAnimationAsset2);
    lcd->createChar(6, forwardAnimationAsset3);
    lcd->createChar(7, forwardAnimationAsset4);

    lcd->createChar(8, backwardAnimationAsset1);
    lcd->createChar(9, backwardAnimationAsset2);
    lcd->createChar(10, backwardAnimationAsset3);
    lcd->createChar(11, backwardAnimationAsset4);
}