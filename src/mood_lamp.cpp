#include "Arduino.h"
#include "helpers.h"
#include <FastLED.h>
#define FRAMES_PER_SECOND  120

#define DATA_PIN 2
#define LED_TYPE WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS 50
#define BRIGHTNESS 255

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


CRGB leds[NUM_LEDS];

const uint8_t NUM_PATTERNS = 18;
void (*patterns[NUM_PATTERNS])();

const uint8_t NUM_TRANSITIONS = 3;
void (*transitions[NUM_TRANSITIONS])();

const uint8_t NUM_ROWS = 7;
const uint8_t NUM_COLUMNS = 7;
const uint8_t PIXELS_PER_ROW = NUM_LEDS / NUM_COLUMNS;

// Shared Global Variables
bool g_patternsReset = true;
unsigned long g_startTime;
uint8_t g_phase1, g_phase2, g_phase3;
uint8_t g_bpm1, g_bpm2, g_bpm3;
float g_scale1, g_scale2, g_scale3;
uint8_t g_pulseWidth1, g_pulseWidth2, g_pulseWidth3;
uint8_t g_patternIndex;
uint8_t g_hue1, g_hue2;
uint8_t g_hueSteps1;
uint8_t g_sat1;
uint8_t g_glitterChance, g_glitterPercent;
bool g_addGlitter;
uint8_t g_everyNSecs;
uint16_t g_everyNMillis;
CRGBPalette16 g_palette1;
TBlendType    g_paletteBlending1;
static uint8_t g_colorIndex;
bool g_reverse1, g_reverse2, g_reverse3;
const uint8_t NUM_COLOR_STRATEGIES = 4;
uint8_t g_colorStrategy;
bool g_rowsInsteadOfCols;







void resetPatternGlobals();

void doPeriodicUpdates() {
  resetPatternGlobals();  
  uint8_t i = random(NUM_TRANSITIONS);
  // Serial.print("transition: ");
  // Serial.println(i);
  // transitions[i](); 
  i = random(NUM_PATTERNS);
  g_patternIndex = i;
}

void setBrightnessFromKnob() {
  uint8_t val = interpolate(analogRead(A0), 0, 1023, 30, 255);
  FastLED.setBrightness(val);
}


// void rowsOrColumnsTest();
void loop() {
  // rowsOrColumnsTest();

  // Call the current pattern function once, updating the 'leds' array
  patterns[g_patternIndex]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show(); 
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_SECONDS(20) { doPeriodicUpdates(); } // change patterns periodically
  EVERY_N_MILLIS(125) { setBrightnessFromKnob(); }
}












// helpers that require access to globals TODO: fix this

// This function fills the palette with totally random colors.
void setupRandomPalette1()
{
//    for( uint8_t i = 0; i < 16; i++) {
//      if (i == 0 || pctToBool(25)) {
//        g_palette1[i] = CHSV( random8(), 255, random8(10,255));
//      } else {
//        g_palette1[i] = g_palette1[i-1];
//      }
//    }
  CRGB c1, c2, c3, c4;
  c1 = getRandomColor();
  c2 = getRandomColor();
  c3 = getRandomColor();
  c4 = getRandomColor();
  g_palette1 = CRGBPalette16(c1, c2, c3, c4);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

// void printPatternGlobals() {
//   Serial.print("g_patternsReset: ");
//   Serial.println(g_patternsReset);
//   Serial.print("g_bpm1: ");
//   Serial.println(g_bpm1);
//   Serial.print("g_bpm2: ");
//   Serial.println(g_bpm2);
//   Serial.print("g_bpm3: ");
//   Serial.println(g_bpm3);
//   Serial.print("g_paletteBlending1: ");
//   Serial.println(g_paletteBlending1);
//   Serial.print("g_colorIndex: ");
//   Serial.println(g_colorIndex);
//   Serial.print("g_startTime: ");
//   Serial.println(g_startTime);
//   Serial.print("g_reverse1: ");
//   Serial.println(g_reverse1);
//   Serial.print("g_phase1: ");
//   Serial.println(g_phase1);
//   Serial.print("g_phase2: ");
//   Serial.println(g_phase2);
//   Serial.print("g_phase3: ");
//   Serial.println(g_phase3);  
//   Serial.print("g_addGlitter: ");
//   Serial.println(g_addGlitter);
//   Serial.print("g_glitterChance: ");
//   Serial.println(g_glitterChance);
//   Serial.print("g_glitterPercent: ");
//   Serial.println(g_glitterPercent);  
//   Serial.print("g_scale1: ");
//   Serial.println(g_scale1);
//   Serial.print("g_scale2: ");
//   Serial.println(g_scale2);
//   Serial.print("g_scale3: ");
//   Serial.println(g_scale3);
//   Serial.print("g_sat1: ");
//   Serial.println(g_sat1);    
//   Serial.print("g_pulseWidth1: ");
//   Serial.println(g_pulseWidth1);
//   Serial.print("g_pulseWidth2: ");
//   Serial.println(g_pulseWidth2);
//   Serial.print("g_pulseWidth3: ");
//   Serial.println(g_pulseWidth3);  
//   Serial.print("g_hue1: ");
//   Serial.println(g_hue1);
//   Serial.print("g_hue2: ");
//   Serial.println(g_hue2);
//   Serial.print("g_hueSteps1: ");
//   Serial.println(g_hueSteps1);  
//   Serial.print("g_everyNMillis: ");
//   Serial.println(g_everyNMillis);
//   Serial.print("g_everyNSecs: ");
//   Serial.println(g_everyNSecs);
// }

void randomizeReverses() {
  g_reverse1 = random8(2);
  g_reverse2 = random8(2);  
  g_reverse3 = random8(2);  
}

void resetPatternGlobals() {
  // set up our global variables with sane values. These values may be overridden by pattern functions as needed.
	g_patternsReset = true;

  g_bpm1 = random8(2,80);
  g_bpm2 = random8(2,80);
  g_bpm3 = random8(2,80);

  g_paletteBlending1 = LINEARBLEND;
  g_colorIndex = 0;

  g_startTime = millis();

  randomizeReverses();

  g_phase1 = random8();
  g_phase2 = random8();
  g_phase3 = random8();
  
  g_addGlitter = pctToBool(30);
  g_glitterChance = 80;
  g_glitterPercent = random8(40,80);
  
  g_scale1 = getRandomFloat(0.25f, 4.0f);
  g_scale2 = getRandomFloat(0.25f, 4.0f);
  g_scale3 = getRandomFloat(0.25f, 4.0f);

  g_sat1 = 255;
    
  g_pulseWidth1 = 255 / random8(2,NUM_LEDS);    
  g_pulseWidth2 = 255 / random8(2,NUM_LEDS);
  g_pulseWidth3 = 255 / random8(2,NUM_LEDS);
  
  g_hue1 = random8();
  g_hue2 = random8();

  g_hueSteps1 = random8(1,48);
  
  g_everyNMillis = random16(100,1000);  
  g_everyNSecs = random8(3,15);
  g_colorStrategy = (g_colorStrategy + 1) % NUM_COLOR_STRATEGIES;
  g_colorStrategy = 3;
  g_rowsInsteadOfCols = random8(0,2);
  setupRandomPalette1();
}



CRGB executeColorStrategy(uint8_t hue, uint8_t positionalValue) {
  CRGB color;
  switch(g_colorStrategy) {
      case 0:      
        color = CHSV(hue, g_sat1, positionalValue);
        break;
      case 1:
        color = ColorFromPalette(g_palette1, hue, positionalValue, g_paletteBlending1);
        break;
      case 2:      
        color = CHSV(positionalValue, g_sat1, 255);
        break;
      case 3:      
        color = ColorFromPalette(g_palette1, positionalValue, 255, g_paletteBlending1);
        break;
  }

  return color;
}

uint8_t phaseFromRowOrColumnIndex(uint16_t pixelIndex, float scale, bool reversePattern) {
  if (g_rowsInsteadOfCols) {
    return phaseFromRowIndex(pixelIndex, PIXELS_PER_ROW, NUM_ROWS, scale, reversePattern);
  } else {
    return phaseFromColumnIndex(pixelIndex, NUM_COLUMNS, scale, reversePattern);
  }
}







//transitions
void RGBWipe(byte r, byte g, byte b, uint8_t wait, bool reverse) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    leds[reverse ? NUM_LEDS - i : i] = CRGB(r, g, b);
    FastLED.show();
    delay(wait);
  }
}

void randomRGBTimeWipe(byte r, byte g, byte b) {  
  uint8_t d = random8(25, 100);
  bool reverse = pctToBool(50);
  // Serial.print("reverse: ");
  // Serial.println(reverse);
  RGBWipe(r, g, b, d, reverse);
}

void wipeToBlack() {
  Serial.println("wipeToBlack");
  randomRGBTimeWipe(0,0,0);
}

void wipeToRandom() {
  Serial.println("wipeToRandom");
  uint8_t r = random8();
  uint8_t g = random8();
  uint8_t b = random8();
  randomRGBTimeWipe(r, g, b);
}


void fadeOut() {
  Serial.println("fadeOut");
  for (uint8_t i=BRIGHTNESS; i > 0; i--) {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(20);
  }
  fill_solid(0,0,0);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
}















//sequences
void paletteTest(){
  if (g_patternsReset) {
    Serial.println("paletteTest");
    g_bpm1 = random8(2,20);
    g_patternsReset = false;
  }

  uint8_t paletteSin = beatsin8(g_bpm1, 0, 255, g_startTime, 0);
  fill_solid(leds, NUM_LEDS, ColorFromPalette(g_palette1, paletteSin, 255, g_paletteBlending1));
}

void redGreenBlueSin(){
  if (g_patternsReset) {
    Serial.println("redGreenBlueSin");
    g_patternsReset = false;
  }
  
  uint8_t redSin = beatsin8(g_bpm1, 0, 255, g_startTime, g_phase1);
  uint8_t greenSin = beatsin8(g_bpm2, 0, 255, g_startTime, g_phase2);
  uint8_t blueSin = beatsin8(g_bpm3, 0, 255, g_startTime, g_phase3);
  fill_solid(leds, NUM_LEDS, CRGB(redSin, greenSin, blueSin));
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void redGreenBlueSaw(){
  if (g_patternsReset) {
    Serial.println("redGreenBlueSaw");
    g_patternsReset = false;
  }
  
  uint8_t redSin = beatsaw8(g_bpm1, 0, 255, g_startTime, g_phase1);
  uint8_t greenSin = beatsaw8(g_bpm2, 0, 255, g_startTime, g_phase2);
  uint8_t blueSin = beatsaw8(g_bpm3, 0, 255, g_startTime, g_phase3);
  // fill_solid(leds, NUM_LEDS, CRGB(redSin, 0, 0));
  fill_solid(leds, NUM_LEDS, CRGB(redSin, greenSin, blueSin));
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void posSigTest(){
  if (g_patternsReset) {
    Serial.println("posSigTest");
    g_bpm1 = random8(2,20);
    g_bpm2 = random(2,20); //hue sin
    g_patternsReset = false;
  }

  EVERY_N_SECONDS(g_everyNSecs) { g_reverse1 = !g_reverse1; }
  
  uint8_t phase, sinVal, hue;
  CRGB color;

  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1, g_reverse1);
    sinVal = beatsin8(g_bpm1, 0, 255, g_startTime, phase);
    color = executeColorStrategy(hue, sinVal);
    leds[i] = color;
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void posSigSawTest(){
  if (g_patternsReset) {
    Serial.println("posSigSawTest");
    g_bpm2 = random(2,20); //hue sin
    g_patternsReset = false;
  }

  EVERY_N_SECONDS(g_everyNSecs) { g_reverse1 = !g_reverse1; }
  
  uint8_t phase, sinVal, hue;
  CRGB color;

  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1, g_reverse1);
    sinVal = beatsaw8(g_bpm1, 0, 255, g_startTime, phase);
    color = executeColorStrategy(hue, sinVal);
    leds[i] = color;
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void posSig3WaveTest(){
  if (g_patternsReset) {
    Serial.println("posSig3WaveTest");
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3 = getRandomFloat(0.2f, 2.0f);
    g_patternsReset = false;
  }
  
  float redPhase, greenPhase, bluePhase;
  uint8_t redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1, g_reverse1);
    greenPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale2, g_reverse2);
    bluePhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale3, g_reverse3);
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, redPhase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, greenPhase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void posSig3WaveSawTest(){
  if (g_patternsReset) {
    Serial.println("posSig3WaveSawTest");
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3 = getRandomFloat(0.2f, 2.0f);
    g_patternsReset = false;
  }
  
  float redPhase, greenPhase, bluePhase;
  uint8_t redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1, g_reverse1);
    greenPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale2, g_reverse2);
    bluePhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale3, g_reverse3);
    redVal = beatsaw8(g_bpm1, 0, 255, g_startTime, redPhase);
    greenVal = beatsaw8(g_bpm2, 0, 255, g_startTime, greenPhase);
    blueVal = beatsaw8(g_bpm3, 0, 255, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void rowsOrColumnsTest(){
  if (g_patternsReset) {
    Serial.println("rowsOrColumnsTest");
    g_scale1 = getRandomFloat(0.5f, 5.0f);
    g_bpm2 = random(2,20); //hue sin
    g_patternsReset = false;
  }
  
  float sinVal, phase, hue;
  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  CRGB color;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromRowOrColumnIndex(i, g_scale1, g_reverse1);
    sinVal = beatsin8(g_bpm1, 0, 255, g_startTime, phase);
    color = executeColorStrategy(hue, sinVal);
    leds[i] = color;
  }
    
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }

  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void rowRGBWaveTest(){
  if (g_patternsReset) {
    Serial.println("rowRGBWaveTest");
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3 = getRandomFloat(0.2f, 2.0f);
    g_patternsReset = false;
  }
  
  float redPhase, greenPhase, bluePhase;
  uint8_t redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromRowIndex(i, 7, 7, g_scale1, g_reverse1);
    greenPhase = phaseFromRowIndex(i, 7, 7, g_scale2, g_reverse2);
    bluePhase = phaseFromRowIndex(i, 7, 7, g_scale3, g_reverse3);
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, redPhase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, greenPhase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void colRGBWaveTest(){
  if (g_patternsReset) {
    Serial.println("colRGBWaveTest");
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3= getRandomFloat(0.2f, 2.0f);
    g_patternsReset = false;
  }

  float redPhase, greenPhase, bluePhase;
  uint8_t redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromColumnIndex(i, 7, g_scale1, g_reverse1);
    greenPhase = phaseFromColumnIndex(i, 7, g_scale2, g_reverse2);
    bluePhase = phaseFromColumnIndex(i, 7, g_scale3, g_reverse3);
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, redPhase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, greenPhase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void oddEvenRGBWaveTest(){
  if (g_patternsReset) {
    Serial.println("oddEvenRGBWaveTest");
    g_patternsReset = false;
  }
  
  float phase;
  uint8_t redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromOddEvenIndex(i);   
    redVal = beatsin8(g_bpm1, 0, 255, g_startTime, phase);
    greenVal = beatsin8(g_bpm2, 0, 255, g_startTime, phase);
    blueVal = beatsin8(g_bpm3, 0, 255, g_startTime, phase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  addGlitter(g_glitterChance);
}



void squarePosSigTest(){
  if (g_patternsReset) {
    Serial.println("squarePosSigTest");
    g_bpm2 = random8(2,40); //hue sin
    g_scale1 = getRandomFloat(0.1f, 4.0f);
    g_patternsReset = false;
  }
  
  uint8_t curSin, phase, hue;
  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1, g_reverse1);
    curSin = beatsquare8(g_bpm1, 0, 255, g_startTime, phase, g_pulseWidth1);
    leds[i] = CHSV(hue, g_sat1, curSin);
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void squareRGBPosSigTest(){
  if (g_patternsReset) {
    Serial.println("squareRGBPosSigTest");    
    g_scale1 = getRandomFloat(0.1f, 4.0f);
    g_scale2 = getRandomFloat(0.1f, 4.0f);
    g_scale3 = getRandomFloat(0.1f, 4.0f);
    g_patternsReset = false;
  }

  
  uint8_t redSin, greenSin, blueSin, redPhase, greenPhase, bluePhase;

  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1, g_reverse1);
    greenPhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale2, g_reverse2);
    bluePhase = phaseFromPixelIndex(i, NUM_LEDS, g_scale3, g_reverse3);
    redSin = beatsquare8(g_bpm1, 0, 255, g_startTime, redPhase, g_pulseWidth1);
    greenSin = beatsquare8(g_bpm2, 0, 255, g_startTime, greenPhase, g_pulseWidth2);
    blueSin = beatsquare8(g_bpm3, 0, 255, g_startTime, bluePhase, g_pulseWidth3);
    leds[i] = CRGB(redSin, greenSin, blueSin);
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

//from demo reel example
void rainbow() 
{
  if (g_patternsReset) {
    Serial.println("rainbow");
    g_hue2 = random8(5,255); //delta hue
    g_patternsReset = false;
  }
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, g_hue1, g_hue2);
  EVERY_N_MILLISECONDS( g_everyNMillis ) { g_hue1+= g_hueSteps1; }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void confetti() 
{
  if (g_patternsReset) {
    Serial.println("confetti");
    g_patternsReset = false;
  }
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  uint8_t pos = random16(NUM_LEDS);
  leds[pos] += CHSV( g_hue1 + random8(64), 200, 255);
  EVERY_N_MILLISECONDS( g_everyNMillis ) { g_hue1+=g_hueSteps1; }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void sinelon()
{
  if (g_patternsReset) {
    Serial.println("sinelon");
    g_bpm1 = random8(5, 100);
    g_patternsReset = false;
  }
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t pos = beatsin16( g_bpm1, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( g_hue1, 255, 192);
  EVERY_N_MILLISECONDS( g_everyNMillis ) { g_hue1+=g_hueSteps1; }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void bpm()
{
  if (g_patternsReset) {
    Serial.println("bpm");
    g_bpm1 = random8(10, 120);
    g_hueSteps1 = random8(1,16);
    g_patternsReset = false;
  }
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( g_bpm1, 64, 255);
  for( uint8_t i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, g_hue1+(i*2), beat-g_hue1+(i*10));
  }
  
  EVERY_N_MILLISECONDS( g_everyNMillis ) { g_hue1+=g_hueSteps1; }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void juggle() {
  if (g_patternsReset) {
    Serial.println("juggle");
    g_everyNMillis = random(500,1000);
    g_hueSteps1 = random8(1,12);
    g_patternsReset = false;
  }
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( uint8_t i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  
  EVERY_N_MILLISECONDS( g_everyNMillis ) { g_hue1+=g_hueSteps1; }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}
















//unused sequences
void squareTest(float runTime){
  Serial.println("squareTest");
  uint8_t freq = random8(2,80);
  uint8_t pulseWidth = 255 * 0.5f;
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
  uint8_t phase, redVal;
  uint8_t freq = random8(2,80);
  uint8_t pulseWidth = 255 * 0.25f;
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











//palette sequences
void posSigSquarePaletteTest(){
  if (g_patternsReset) {
    Serial.println("posSigSquarePaletteTest");
    g_bpm2 = random8(2,20); //hue sin
    g_scale1 = getRandomFloat(0.1f, 4.0f);
    g_patternsReset = false;
  }
  
  uint8_t squareVal, phase, hue;
  hue = beatsin8(g_bpm2, 0, 255, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = phaseFromPixelIndex(i, NUM_LEDS, g_scale1, g_reverse1);
    squareVal = beatsquare8(g_bpm1, 0, 255, g_startTime, phase, g_pulseWidth1);
    leds[i] = ColorFromPalette(g_palette1, hue, squareVal, g_paletteBlending1);
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}






//setup
void setupPatterns() {
  patterns[0] = redGreenBlueSin;
  patterns[1] = posSigTest;
  patterns[2] = posSig3WaveTest;
  patterns[3] = rowsOrColumnsTest;
  patterns[4] = posSigSquarePaletteTest;
  patterns[5] = oddEvenRGBWaveTest;
  patterns[6] = rowRGBWaveTest;
  patterns[7] = colRGBWaveTest;
  patterns[8] = squarePosSigTest;
  patterns[9] = squareRGBPosSigTest;
  patterns[10] = rainbow;
  patterns[11] = confetti;
  patterns[12] = sinelon;
  patterns[13] = bpm;
  patterns[14] = juggle;
  patterns[15] = paletteTest;
  patterns[16] = posSigSawTest;
  patterns[17] = posSig3WaveSawTest;
}

void setupTransitions() {
  transitions[0] = wipeToBlack;
  transitions[1] = wipeToRandom;
  transitions[2] = fadeOut;
}

void setup() {  
  delay(2000);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  randomSeed(analogRead(0));
  random16_set_seed(8934);
  random16_add_entropy(analogRead(3));
  
  setupPatterns();
  setupTransitions();
  resetPatternGlobals();
  g_patternIndex = random8(NUM_PATTERNS);
}