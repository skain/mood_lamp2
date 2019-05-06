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

// consts
const uint8_t NUM_PATTERNS = 11;
// const uint8_t NUM_TRANSITIONS = 3;
const uint8_t NUM_ROWS = 7;
const uint8_t NUM_COLUMNS = 7;
const uint8_t PIXELS_PER_ROW = NUM_LEDS / NUM_COLUMNS;

// the strategy consts are just here to try and make the code more legible
const uint8_t COLOR_STRATEGY_HSV_HUE_AND_POSITION = 0; // HSV with hue for hue and postion for value
const uint8_t COLOR_STRATEGY_PALETTE_HUE_AND_POSITION = 1; // Random palette with hue for wheel position and position for brightness
const uint8_t COLOR_STRATEGY_HSV_POSITION_FOR_HUE = 2; // HSV with position for hue, 255 for value (hue is ignored)
const uint8_t COLOR_STRATEGY_PALETTE_POSITION_FOR_HUE = 3; // Palette with position for wheel position, 255 for brightness (hue is ignored)

const uint8_t PHASE_STRATEGY_ROWS = 0; // Phase calculated by row
const uint8_t PHASE_STRATEGY_COLUMNS = 1; // Phase calculated by column
const uint8_t PHASE_STRATEGY_ODD_EVEN = 2; // Phase calculated by pixelIndex % 2
const uint8_t PHASE_STRATEGY_STRIP_INDEX = 3; // Phase calculated directly by pixel location on strip

const uint8_t WAVE_STRATEGY_SIN = 0; // basic sin
const uint8_t WAVE_STRATEGY_SAW = 1; // basic sawtooth
const uint8_t WAVE_STRATEGY_TRIANGLE = 2; // basic triangle (linear slopes)
const uint8_t WAVE_STRATEGY_CUBIC = 3; // basic cubic (spends more time at limits than sine)




void (*patterns[NUM_PATTERNS])();
// void (*transitions[NUM_TRANSITIONS])();


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
TBlendType g_paletteBlending1;
static uint8_t g_colorIndex;
bool g_reverse1, g_reverse2, g_reverse3;
const uint8_t NUM_COLOR_STRATEGIES = 4;
uint8_t g_colorStrategy;
uint8_t g_pixelIndexStrategy;
uint8_t g_waveStrategy1, g_waveStrategy2, g_waveStrategy3;







void resetPatternGlobals();

void doPeriodicUpdates() {
  resetPatternGlobals();  
  // uint8_t i = random(NUM_TRANSITIONS);
  // Serial.print("transition: ");
  // Serial.println(i);
  // transitions[i](); 
  uint8_t i = random(NUM_PATTERNS);
  g_patternIndex = i;
}

void setBrightnessFromKnob() {
  uint8_t val = interpolate(analogRead(A0), 0, 1023, 30, 255);
  FastLED.setBrightness(val);
}


// void sinelon();
void loop() {
  // sinelon();

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
  g_colorStrategy = random8(0,4);
  g_pixelIndexStrategy = random8(0,4);
  g_waveStrategy1 = random8(0,4);
  g_waveStrategy2 = random8(0,4);
  g_waveStrategy3 = random8(0,4);
  setupRandomPalette1();
}



CRGB executeColorStrategy(uint8_t hue, uint8_t positionalValue) {
  CRGB color;
  switch(g_colorStrategy) {
      case COLOR_STRATEGY_HSV_HUE_AND_POSITION:      
        color = CHSV(hue, g_sat1, positionalValue);
        break;
      case COLOR_STRATEGY_PALETTE_HUE_AND_POSITION:
        color = ColorFromPalette(g_palette1, hue, positionalValue, g_paletteBlending1);
        break;
      case COLOR_STRATEGY_HSV_POSITION_FOR_HUE:      
        color = CHSV(positionalValue, g_sat1, 255);
        break;
      case COLOR_STRATEGY_PALETTE_POSITION_FOR_HUE:      
        color = ColorFromPalette(g_palette1, positionalValue, 255, g_paletteBlending1);
        break;
  }

  return color;
}

uint8_t executePixelPhaseStrategy(uint16_t pixelIndex, float scale, bool reversePattern) {
  uint8_t phase = 0;
  switch(g_pixelIndexStrategy) {
    case PHASE_STRATEGY_ROWS:
      phase = phaseFromRowIndex(pixelIndex, PIXELS_PER_ROW, NUM_ROWS, scale, reversePattern);
      break;
    case PHASE_STRATEGY_COLUMNS:
      phase = phaseFromColumnIndex(pixelIndex, NUM_COLUMNS, scale, reversePattern);
      break;
    case PHASE_STRATEGY_ODD_EVEN:
      phase = phaseFromOddEvenIndex(pixelIndex);
      break;
    case PHASE_STRATEGY_STRIP_INDEX:
      phase = phaseFromPixelIndex(pixelIndex, NUM_LEDS, scale, reversePattern);
      break;
  }
  return phase;
}

uint8_t executeWaveStrategy(uint8_t waveStrategy, uint8_t bpm, unsigned long startTime, uint8_t phase) {
  uint8_t waveValue = 0;
  switch(waveStrategy) {
    case WAVE_STRATEGY_SIN:
      waveValue = beatsin8(bpm, 0, 255, startTime, phase);
      break;
    case WAVE_STRATEGY_SAW:
      waveValue = beatsaw8(bpm, 0, 255, startTime, phase);
      break;
    case WAVE_STRATEGY_TRIANGLE:
      waveValue = beattriwave8(bpm, 0, 255, startTime, phase);
      break;
    case WAVE_STRATEGY_CUBIC:
      waveValue = beatcubicwave8(bpm, 9, 255, startTime, phase);
  }

  return waveValue;
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
void strategySolidPaletteWave(){
  if (g_patternsReset) {
    Serial.println("strategySolidPaletteWave");
    g_bpm1 = random8(5,15);
    g_patternsReset = false;
  }

  uint8_t paletteSin = executeWaveStrategy(g_waveStrategy1, g_bpm1, g_startTime, 0);
  fill_solid(leds, NUM_LEDS, ColorFromPalette(g_palette1, paletteSin, 255, g_paletteBlending1));

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void strategySolidRGBWave(){
  if (g_patternsReset) {
    Serial.println("strategySolidRGBWave");
    g_patternsReset = false;
  }
  
  uint8_t redSin = executeWaveStrategy(g_waveStrategy1, g_bpm1, g_startTime, g_phase1);
  uint8_t greenSin = executeWaveStrategy(g_waveStrategy2, g_bpm2, g_startTime, g_phase2);
  uint8_t blueSin = executeWaveStrategy(g_waveStrategy3, g_bpm3, g_startTime, g_phase3);  
  fill_solid(leds, NUM_LEDS, CRGB(redSin, greenSin, blueSin));
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void strategyColorAndPositionWave() {
  if (g_patternsReset) {
    Serial.println("strategyColorAndPositionWave");
    g_bpm1 = random8(2,20);
    g_bpm2 = random(2,20); //hue sin
    g_patternsReset = false;
  }

  EVERY_N_SECONDS(g_everyNSecs) { g_reverse1 = !g_reverse1; }
  
  uint8_t phase, waveVal, hue;
  CRGB color;

  hue = executeWaveStrategy(g_waveStrategy1, g_bpm2, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = executePixelPhaseStrategy(i, g_scale1, g_reverse1);
    waveVal = executeWaveStrategy(g_waveStrategy2, g_bpm1, g_startTime, phase);
    color = executeColorStrategy(hue, waveVal);
    leds[i] = color;
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void strategyHueWaveWithSquare(){
  if (g_patternsReset) {
    Serial.println("strategyHueWaveWithSquare");
    g_bpm2 = random8(2,40); //hue sin
    g_scale1 = getRandomFloat(0.1f, 4.0f);
    switch(g_colorStrategy) { // strats 2 and 3 don't work with this pattern
      case 2:
        g_colorStrategy = 0;
        break;
      case 3:
        g_colorStrategy = 1;
    }
    g_patternsReset = false;
  }
  
  uint8_t curSquare, phase, hue;
  CRGB color;
  hue = executeWaveStrategy(g_waveStrategy2, g_bpm2, g_startTime, 0);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = executePixelPhaseStrategy(i, g_scale1, g_reverse1);
    curSquare = beatsquare8(g_bpm1, 0, 255, g_startTime, phase, g_pulseWidth1);
    color = executeColorStrategy(hue, curSquare);
    leds[i] = color;
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void strategyRGBWaveAndPhase(){
  if (g_patternsReset) {
    Serial.println("strategyRGBWaveAndPhase");
    g_scale1 = getRandomFloat(0.2f, 2.0f);
    g_scale2 = getRandomFloat(0.2f, 2.0f);
    g_scale3 = getRandomFloat(0.2f, 2.0f);
    g_patternsReset = false;
  }
  
  float redPhase, greenPhase, bluePhase;
  uint8_t redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = executePixelPhaseStrategy(i, g_scale1, g_reverse1);
    greenPhase = executePixelPhaseStrategy(i, g_scale2, g_reverse1);
    bluePhase = executePixelPhaseStrategy(i, g_scale3, g_reverse1);
    redVal = executeWaveStrategy(g_waveStrategy1, g_bpm1, g_startTime, redPhase);
    greenVal = executeWaveStrategy(g_waveStrategy2, g_bpm2, g_startTime, greenPhase);
    blueVal = executeWaveStrategy(g_waveStrategy3, g_bpm3, g_startTime, bluePhase);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void strategyPhaseWithRGBSquare(){
  if (g_patternsReset) {
    Serial.println("strategyPhaseWithRGBSquare");    
    g_scale1 = getRandomFloat(0.1f, 4.0f);
    g_scale2 = getRandomFloat(0.1f, 4.0f);
    g_scale3 = getRandomFloat(0.1f, 4.0f);
    g_patternsReset = false;
  }

  
  uint8_t redSin, greenSin, blueSin, redPhase, greenPhase, bluePhase;

  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = executePixelPhaseStrategy(i, g_scale1, g_reverse1);
    greenPhase = executePixelPhaseStrategy(i, g_scale2, g_reverse2);
    bluePhase = executePixelPhaseStrategy(i, g_scale3, g_reverse3);
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















//setup
void setupPatterns() {
  patterns[0] = strategySolidRGBWave;
  patterns[1] = strategyRGBWaveAndPhase;
  patterns[2] = confetti;
  patterns[3] = strategyColorAndPositionWave;
  patterns[4] = strategyHueWaveWithSquare;
  patterns[5] = bpm;
  patterns[6] = juggle;
  patterns[7] = strategySolidPaletteWave;
  patterns[8] = sinelon;
  patterns[9] = strategyPhaseWithRGBSquare;
  patterns[10] = rainbow;
}

// void setupTransitions() {
//   transitions[0] = wipeToBlack;
//   transitions[1] = wipeToRandom;
//   transitions[2] = fadeOut;
// }

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
  // setupTransitions();
  resetPatternGlobals();
  g_patternIndex = random8(NUM_PATTERNS);
}