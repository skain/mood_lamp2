#include <FastLED.h>
#define FRAMES_PER_SECOND  120

#define DATA_PIN 2
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 49
#define BRIGHTNESS 100
CRGB leds[NUM_LEDS];

const int NUM_PATTERNS = 10;
void (*patterns[NUM_PATTERNS])();

const int NUM_TRANSITIONS = 2;
void (*transitions[NUM_TRANSITIONS])();

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {  
  delay(2000);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  setupPatterns();
  setupTransitions();
}

void setupPatterns() {
  patterns[0] = redGreenBlueSin;
  patterns[1] = posSigTest;
  patterns[2] = posSig3WaveTest;
  patterns[3] = rowTest;
  patterns[4] = colTest;
  patterns[5] = oddEvenRGBWaveTest;
  patterns[6] = rowRGBWaveTest;
  patterns[7] = colRGBWaveTest;
  patterns[8] = squarePosSigTest;
  patterns[9] = squareRGBPosSigTest;
}

void setupTransitions() {
  transitions[0] = wipeToBlack;
  transitions[1] = wipeToRandom;
}




// Global Variables
bool g_patternsReset = true;
unsigned long g_startTime;
int g_phase1, g_phase2, g_phase3;
int g_bpm1, g_bpm2, g_bpm3;
int g_sin1, g_sin2, g_sin3;
int g_scale1, g_scale2, g_scale3;
int g_pulseWidth1, g_pulseWidth2, g_pulseWidth3;
int g_patternIndex = random(NUM_PATTERNS);
CRGB g_CRGB1;
int g_hue1;
int g_sat1;









void loop() {
//  squarePosSigTest();

// Call the current pattern function once, updating the 'leds' array
  patterns[g_patternIndex]();
 
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_SECONDS( 20 ) { g_patternsReset = true;  transitions[random(NUM_TRANSITIONS)](); g_patternIndex = random(NUM_PATTERNS); } // change patterns periodically
}




// helpers
float haveSecsElapsed(float secs, unsigned long startTime) {
  float now = millis();
  if ((now - startTime) > (secs * 1000)) {
    return true;
  }

  return false;
}

float timeFromMillis(float t){
  return t / 1000.0f;
}

float interpolate(float val, float inMin, float inMax, float outMin, float outMax){
  return outMin + (val - inMin) * ((outMax - outMin)/(inMax - inMin));
}

float phaseFromPixelIndex(float pixelIndex, float numPixels, float scale) {
  int phase = interpolate(pixelIndex, 0, numPixels * scale, 0, 255);
  return phase;
}

int phaseFromOddEvenIndex(uint16_t pixelIndex) {
  if (pixelIndex % 2 == 0) {
    return 0;
  } else {
    return 255 / 2;
  }
}

float phaseFromRowIndex(float pixelIndex, float pixelsPerRow, float numRows, float scale){
  if (pixelIndex  == 0) {
    return 0;
  }
  uint16_t rowIndex = pixelIndex / pixelsPerRow;
  return interpolate(rowIndex, 0.0f, (numRows - 1.0f) * scale, 0.0f, 255);
}

float phaseFromColumnIndex(uint16_t pixelIndex, uint16_t numCols, float scale){
  if (pixelIndex  == 0) {
    return 0;
  }
  uint16_t colIndex = pixelIndex % numCols;
  return interpolate(colIndex, 0, (numCols - 1) * scale, 0.0f, 255);
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













//transitions
void wipeToBlack() {
  randomRGBTimeWipe(0,0,0);
}

void wipeToRandom() {
  int r = random8();
  int g = random8();
  int b = random8();
  randomRGBTimeWipe(r, g, b);
}

void randomRGBTimeWipe(byte r, byte g, byte b) {  
  int d = random8(25, 100);
  RGBWipe(r, g, b, d);
}















//sequences
// Fill the dots one after the other with a color
void RGBWipe(byte r, byte g, byte b, uint8_t wait) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    leds[i] = CRGB(r, g, b);
    FastLED.show();
    delay(wait);
  }
}

void redGreenBlueSin(){
  if (g_patternsReset) {
    Serial.println("redGreenBlueSin");
    g_phase1 = random8();
    g_phase2 = random8();
    g_phase3 = random8();
    g_bpm1 = random8(2,80);
    g_bpm2 = random8(2,80);
    g_bpm3 = random8(2,80);
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  int redSin = beatsin8(g_bpm1, 0, 255, g_startTime, g_phase1);
  int greenSin = beatsin8(g_bpm2, 0, 255, g_startTime, g_phase2);
  int blueSin = beatsin8(g_bpm3, 0, 255, g_startTime, g_phase3);
  fill_solid(leds, NUM_LEDS, CRGB(redSin, greenSin, blueSin));
  FastLED.show();
}

void posSigTest(){
  if (g_patternsReset) {
    Serial.println("posSigTest");
    g_bpm1 = random8(2,80); //value sin
    g_bpm2 = random(2,80); //hue sin
    g_scale1 = getRandomFloat(0.25f, 4.0f);
    g_sat1 = 255;
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  int phase, sinVal, hue;

  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1);
    sinVal = beatsin8(g_bpm1, 0, 255, g_startTime, phase);
    leds[i] = CHSV(hue, g_sat1, sinVal);
  }

  FastLED.show();
}

void posSig3WaveTest(){
  if (g_patternsReset) {
    Serial.println("posSig3WaveTest");
    g_bpm1 = random8(2,80);
    g_bpm2 = random8(2,80);
    g_bpm3 = random8(2,80);
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3 = getRandomFloat(0.2f, 2.0f);
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1);
    greenPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale2);
    bluePhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale3);
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, redPhase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, greenPhase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  FastLED.show();
}

void rowTest(){
  if (g_patternsReset) {
    Serial.println("rowTest");
    g_scale1 = getRandomFloat(0.5f, 5.0f);
    g_bpm1 = random8(2,80); //value sin
    g_bpm2 = random(2,80); //hue sin
    g_sat1 = 255;
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  float sinVal, phase, hue;
  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromRowIndex(i, 7, 7, g_scale1);
    sinVal = beatsin8(g_bpm1, 0, 255, g_startTime, phase);
    leds[i] = CHSV(hue, g_sat1, sinVal);
  }
    FastLED.show();
}

void colTest(){
  if (g_patternsReset) {
    Serial.println("colTest");
    g_scale1 = getRandomFloat(0.5f, 5.0f);
    g_bpm1 = random8(2,80); //value sin
    g_bpm2 = random(2,80); //hue sin
    g_sat1 = 255;
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  float sinVal, phase, hue;
  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromColumnIndex(i, 7, g_scale1);
    sinVal = beatsin8(g_bpm1, 0, 255, g_startTime, phase);
    leds[i] = CHSV(hue, g_sat1, sinVal);
  }
  FastLED.show();
}


void rowRGBWaveTest(){
  if (g_patternsReset) {
    Serial.println("rowRGBWaveTest");
    g_bpm1 = random8(2,80);
    g_bpm2 = random8(2,80);
    g_bpm3 = random8(2,80);
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3= getRandomFloat(0.2f, 2.0f);
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromRowIndex(i, 7, 7, g_scale1);
    greenPhase = phaseFromRowIndex(i, 7, 7, g_scale2);
    bluePhase = phaseFromRowIndex(i, 7, 7, g_scale3);
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, redPhase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, greenPhase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  FastLED.show();
}

void colRGBWaveTest(){
  if (g_patternsReset) {
    Serial.println("column_rgb_wave_test");
    g_bpm1 = random8(2,80);
    g_bpm2 = random8(2,80);
    g_bpm3 = random8(2,80);
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3= getRandomFloat(0.2f, 2.0f);
    g_startTime = millis();
    g_patternsReset = false;
  }

  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromColumnIndex(i, 7, g_scale1);
    greenPhase = phaseFromColumnIndex(i, 7, g_scale2);
    bluePhase = phaseFromColumnIndex(i, 7, g_scale3);
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, redPhase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, greenPhase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  FastLED.show();
}

void oddEvenRGBWaveTest(){
  if (g_patternsReset) {
    Serial.println("oddEvenRGBWaveTest");
    g_bpm1 = random8(2,80);
    g_bpm2 = random8(2,80);
    g_bpm3 = random8(2,80);
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  float phase;
  int redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromOddEvenIndex(i);   
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, phase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, phase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, phase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  FastLED.show();
}



void squarePosSigTest(){
  if (g_patternsReset) {
    Serial.println("squareTest");
    g_bpm1 = random8(2,80); //value sin
    g_bpm2 = random8(2,40); //hue sin
    g_scale1 = getRandomFloat(0.1f, 4.0f);
    g_pulseWidth1 = 255 / random8(2,NUM_LEDS);
    g_sat1 = 255;
    g_startTime = millis();
    g_patternsReset = false;
  }
  
  int curSin, phase, hue;
  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1);
    curSin = beatsquare8(g_bpm1, 0, 255, g_startTime, phase, g_pulseWidth1);
    leds[i] = CHSV(hue, g_sat1, curSin);
  }

  FastLED.show();
}

void squareRGBPosSigTest(){
  if (g_patternsReset) {
    Serial.println("squareTest");
    g_bpm1 = random8(2,80);
    g_bpm2 = random8(2,80);
    g_bpm3 = random8(2,80);
    g_pulseWidth1 = 255 / random8(2,NUM_LEDS);
    g_pulseWidth2 = 255 / random8(2,NUM_LEDS);
    g_pulseWidth3 = 255 / random8(2,NUM_LEDS);
    g_scale1 = getRandomFloat(0.1f, 4.0f);
    g_scale2 = getRandomFloat(0.1f, 4.0f);
    g_scale3 = getRandomFloat(0.1f, 4.0f);
    g_startTime = millis();
    g_patternsReset = false;
  }

  
  int redVal, greenVal, blueVal, redSin, greenSin, blueSin, redPhase, greenPhase, bluePhase;

  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1);
    greenPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale2);
    bluePhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale3);
    redSin = beatsquare8(g_bpm1, 0, 255, g_startTime, redPhase, g_pulseWidth1);
    greenSin = beatsquare8(g_bpm2, 0, 255, g_startTime, greenPhase, g_pulseWidth2);
    blueSin = beatsquare8(g_bpm3, 0, 255, g_startTime, bluePhase, g_pulseWidth3);
    leds[i] = CRGB(redSin, greenSin, blueSin);
  }

  FastLED.show();
}


















//unused sequences
void squareTest(float runTime){
  Serial.println("squareTest");
  int freq = random8(2,80);
  int pulseWidth = 255 * 0.5f;
  unsigned long startTime, redVal;

  startTime = millis();
  while(true){
    redVal = beatsquare8(freq, 0, 255, startTime, 0, pulseWidth);
    fill_solid(leds, NUM_LEDS, CRGB(redVal, 0, 0));
    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void odd_even_squareTest(float runTime) {
  Serial.println("odd_even_test");
  int phase, redVal;
  int freq = random8(2,80);
  int pulseWidth = 255 * 0.25f;
  unsigned long startTime = millis();
  while(true) {
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      phase = phaseFromOddEvenIndex(i);
      redVal = beatsquare8(freq, 0, 255, startTime, phase, pulseWidth);
      leds[i] = CRGB(redVal, 0, 0);
    }
    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}
