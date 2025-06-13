#ifndef AnimationAssets_h
#define AnimationAssets_h

#include <Arduino.h>
#include "LiquidCrystal_I2C.h"

// const int maxFrames = max(standbyAnimationFrames, max(forwardAnimationFrames, backwardAnimationFrames));
const int numberOfFrames = 4;
const int animationSpeedFrame = 10; // number of frames that display single animation asset

// const int standbyAnimationFrames = 4;
// const int forwardAnimationFrames = 4;
// const int backwardAnimationFrames = 4;

const int standbyAnimationOffset = 0;
const int forwardAnimationOffset = standbyAnimationOffset + numberOfFrames;
const int backwardAnimationOffset = forwardAnimationOffset + numberOfFrames;

extern byte standbyAnimationAsset1[8];
extern byte standbyAnimationAsset2[8];
extern byte standbyAnimationAsset3[8];
extern byte standbyAnimationAsset4[8];
extern byte forwardAnimationAsset1[8];
extern byte forwardAnimationAsset2[8];
extern byte forwardAnimationAsset3[8];
extern byte forwardAnimationAsset4[8];
extern byte backwardAnimationAsset1[8];
extern byte backwardAnimationAsset2[8];
extern byte backwardAnimationAsset3[8];
extern byte backwardAnimationAsset4[8];

void fetchAssets(const LiquidCrystal_I2C* lcd);

#endif