#include "Arduino.h"
#include <FastLED.h>

float haveSecsElapsed(uint16_t secs, unsigned long startTime);

float interpolate(float val, float inMin, float inMax, float outMin, float outMax);

float phaseFromPixelIndex(uint16_t pixelIndex, uint16_t numPixels, float scale, bool reverse_ptn);

int phaseFromOddEvenIndex(uint16_t pixelIndex);

float phaseFromRowIndex(uint16_t pixelIndex, uint16_t pixelsPerRow, uint16_t numRows, float scale, bool reverse_ptn);

float phaseFromColumnIndex(uint16_t pixelIndex, uint16_t numCols, float scale, bool reverse_ptn);

float getRandomFloat(float min, float max);

float getRandomPhase();

uint16_t beatsquare8(accum88 beatsPerMinute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timeBase = 0, uint8_t phaseOffset = 0, uint8_t pulseWidth=128);

uint8_t beatsaw8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0);

bool pctToBool(fract8 chance);

CHSV getRandomColor();