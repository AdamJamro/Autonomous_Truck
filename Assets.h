#ifndef Assets_h
#define Assets_h

// const int maxFrames = max(standbyAnimationFrames, max(forwardAnimationFrames, backwardAnimationFrames));
const int numberOfFrames = 4;
const int animationSpeedFrame = 10; // number of frames that display single animation asset

// const int standbyAnimationFrames = 4;
// const int forwardAnimationFrames = 4;
// const int backwardAnimationFrames = 4;

const int standbyAnimationOffset = 0;
const int forwardAnimationOffset = standbyAnimationOffset + numberOfFrames;
const int backwardAnimationOffset = forwardAnimationOffset + numberOfFrames;

// forward
void fetchAssets(const LiquidCrystal_I2C* lcd);

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

void fetchAssets(const LiquidCrystal_I2C* lcd) {
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


#endif