#include "Arduino.h"
#include <FastLED.h>

float haveSecsElapsed(uint16_t secs, unsigned long startTime) {
  unsigned long now = millis();
  if ((now - startTime) > (secs * 1000)) {
    return true;
  }

  return false;
}

float interpolate(float val, float inMin, float inMax, float outMin, float outMax){
  return outMin + (val - inMin) * ((outMax - outMin)/(inMax - inMin));
}

float phaseFromPixelIndex(uint16_t pixelIndex, uint16_t numPixels, float scale, bool reverse_ptn) {
  uint16_t fromMin = 0;
  uint16_t fromMax = numPixels * scale;
  if (reverse_ptn) {
	  fromMin = fromMax;
	  fromMax = 0;
  }
  int phase = interpolate(pixelIndex, fromMin, fromMax, 0, 255);
  return phase;
}

int phaseFromOddEvenIndex(uint16_t pixelIndex) {
  if (pixelIndex % 2 == 0) {
    return 0;
  } else {
    return 255 / 2;
  }
}

float phaseFromRowIndex(uint16_t pixelIndex, uint16_t pixelsPerRow, uint16_t numRows, float scale, bool reverse_ptn){
  if (pixelIndex  == 0) {
    return 0;
  }
  uint16_t rowIndex = pixelIndex / pixelsPerRow;
  uint16_t fromMin = 0;
  uint16_t fromMax = (numRows - 1) * scale;
  if (reverse_ptn) {
	  fromMin = fromMax;
	  fromMax = 0;
  }
  return interpolate(rowIndex, fromMin, fromMax, 0, 255);
}

float phaseFromColumnIndex(uint16_t pixelIndex, uint16_t numCols, float scale, bool reverse_ptn){
  if (pixelIndex  == 0) {
    return 0;
  }
  uint16_t colIndex = pixelIndex % numCols;
  uint16_t fromMin = 0;
  uint16_t fromMax = (numCols - 1) * scale;
  if (reverse_ptn) {
	  fromMin = fromMax;
	  fromMax = 0;
  }
  return interpolate(colIndex, fromMin, fromMax, 0, 255);
}

float getRandomFloat(float min, float max){
  // this is not the greatest implementation but works in the situations I've got right now
  int normMin = min * 1000;
  int normMax = max * 1000;
  long r = random(normMin, normMax);
  float freq = r / 1000.0f;
  return freq;
}

float getRandomPhase(){
  float r = random8(0,4);
  if (r == 0) {
    return 0.0f;
  }
  return PI / r;
}

uint16_t beatsquare8(accum88 beatsPerMinute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timeBase = 0, uint8_t phaseOffset = 0, uint8_t pulseWidth=128){
    uint8_t beat = beat8(beatsPerMinute, timeBase);
    uint8_t beatSquare = squarewave8(beat + phaseOffset);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatSquare, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

uint8_t beatsaw8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0) {
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatsaw = beat + phase_offset;
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8( beatsaw, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

bool pctToBool(fract8 chance) {
  //rolls a yes/no dice with the specified integer percent of being yes
  return random8() < chance;
}

CHSV getRandomColor() {
  return CHSV(random8(), random8(240, 255), random8());
}