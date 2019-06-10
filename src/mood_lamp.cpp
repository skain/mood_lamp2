#include "Arduino.h"
#include "helpers.h"
#include <FastLED.h>
#define FRAMES_PER_SECOND  120

#define DATA_PIN 2
#define LED_TYPE WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS 150

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.



// consts
#define NUM_PATTERNS 5
#define NUM_ROWS 5
#define NUM_COLUMNS 5
// #define NUM_ROWS 7
// #define NUM_COLUMNS 7
#define NUM_LEDS NUM_ROWS * NUM_COLUMNS

// the strategy consts are just here to try and make the code more legible
#define COLOR_STRATEGY_HSV_HUE_AND_BRIGHTNESS 0 // HSV with hue for hue and postion for value
#define COLOR_STRATEGY_PALETTE_HUE_AND_BRIGHTNESS 1 // Random palette with hue for wheel BRIGHTNESS and BRIGHTNESS for brightness
#define COLOR_STRATEGY_HSV_BRIGHTNESS_FOR_HUE 2 // HSV with BRIGHTNESS for hue, 255 for value (hue is ignored)
#define COLOR_STRATEGY_PALETTE_BRIGHTNESS_FOR_HUE 3 // Palette with BRIGHTNESS for wheel BRIGHTNESS, 255 for brightness (hue is ignored)

#define PHASE_STRATEGY_ROWS 0 // Phase calculated by row
#define PHASE_STRATEGY_COLUMNS 1 // Phase calculated by column
#define PHASE_STRATEGY_STRIP_INDEX 2 // Phase calculated directly by pixel location on strip
// #define PHASE_STRATEGY_SOLID 3 // All pixels assigned a phase of 0
// #define PHASE_STRATEGY_ODD_EVEN 4 // Phase calculated by pixelIndex % 2

#define WAVE_STRATEGY_SIN 0 // basic sin
#define WAVE_STRATEGY_SAW 1 // basic sawtooth
#define WAVE_STRATEGY_TRIANGLE 2 // basic triangle (linear slopes)
#define WAVE_STRATEGY_CUBIC 3 // basic cubic (spends more time at limits than sine)
#define WAVE_STRATEGY_SQUARE 4 // basic square (on or off)

#define OFFSET_FILL_STRATEGY_RAINBOW 0 //use fill_rainbow
#define OFFSET_FILL_STRATEGY_PALETTE 1 //use current random palette




CRGB leds[NUM_LEDS];
void (*patterns[NUM_PATTERNS])();


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
uint16_t g_everyNMillis1, g_everyNMillis2;
CRGBPalette16 g_palette1;
TBlendType g_paletteBlending1;
static uint8_t g_colorIndex;
bool g_reverse1, g_reverse2, g_reverse3;
uint8_t g_colorStrategy;
uint8_t g_phaseStrategy1, g_phaseStrategy2, g_phaseStrategy3;
uint8_t g_waveStrategy1, g_waveStrategy2, g_waveStrategy3;
uint8_t g_offsetFillStrategy;
uint8_t g_demoReelPatternIndex;
unsigned int g_rowGlitchFactor, g_columnGlitchFactor, g_pixelGlitchFactor;







void resetPatternGlobals();

void doPeriodicUpdates() {
  resetPatternGlobals();  
  uint8_t i = random(NUM_PATTERNS);
  g_patternIndex = i;
}

void setBrightnessFromKnob() {
  uint8_t val = interpolate(analogRead(A0), 0, 1023, 30, 255);
  FastLED.setBrightness(val);
  // EVERY_N_SECONDS(1) { Serial.println(val); }
}


// void fullRGBStrategy();
void loop() {
  // fullRGBStrategy();

  // Call the current pattern function once, updating the 'leds' array
  patterns[g_patternIndex]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show(); 
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_SECONDS(20) { doPeriodicUpdates(); } // change patterns periodically
  EVERY_N_MILLIS(25) { setBrightnessFromKnob(); }
}












// helpers that require access to globals TODO: fix this

// This function fills the palette with totally random colors.
void setupRandomPalette1()
{
  if (random8() > 128) {
    for (uint8_t i = 0; i < 16; i++) {
        g_palette1[i] = CHSV(random8(), 255, random8(10, 255));
    }
  } else {
    CRGB c1, c2, c3, c4;
    c1 = getRandomColor();
    c2 = getRandomColor();
    c3 = getRandomColor();
    c4 = getRandomColor();
    g_palette1 = CRGBPalette16(c1, c2, c3, c4);
  }
}

void addGlitter(uint8_t chanceOfGlitter) 
{
  if (pctToBool(chanceOfGlitter)) {
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
//   Serial.print("g_everyNMillis1: ");
//   Serial.println(g_everyNMillis1);
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

  g_bpm1 = random8(1,80);
  g_bpm2 = random8(1,80);
  g_bpm3 = random8(1,80);

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
  
  g_scale1 = getRandomFloat(0.1f, 4.0f);
  g_scale2 = getRandomFloat(0.1f, 4.0f);
  g_scale3 = getRandomFloat(0.1f, 4.0f);

  g_sat1 = 255;
    
  g_pulseWidth1 = 255 / random8(2,NUM_LEDS);    
  g_pulseWidth2 = 255 / random8(2,NUM_LEDS);
  g_pulseWidth3 = 255 / random8(2,NUM_LEDS);
  
  g_hue1 = random8();
  g_hue2 = random8();

  g_hueSteps1 = random8(1,32);
  
  g_everyNMillis1 = random16(100,1000);  
  g_everyNMillis2 = random(50,250);
  g_everyNSecs = random8(3,15);

  g_colorStrategy = random8(0,4);
  
  g_phaseStrategy1 = random8(0,3);
  g_phaseStrategy2 = random8(0,3);
  g_phaseStrategy3 = random8(0,3);
  
  g_waveStrategy1 = random8(0,4);
  g_waveStrategy2 = random8(0,4);
  g_waveStrategy3 = random8(0,4);

  g_offsetFillStrategy = random(0,2);
  setupRandomPalette1();

  g_rowGlitchFactor = g_columnGlitchFactor = g_pixelGlitchFactor = 0;

  if (pctToBool(30)) {
    g_rowGlitchFactor = random8(1, NUM_COLUMNS);
  }

  if (pctToBool(30)) {
    g_columnGlitchFactor = random8(1, NUM_ROWS);
  }

  if (pctToBool(30)) {
    g_pixelGlitchFactor = random8(1, NUM_LEDS);
  }
}

void disallowColorStrategyBrightnessForHueSwap() {
  //some patterns/waveforms don't work well when swapping BRIGHTNESS for hue, so call 
  //this in the global setup to disallow.
  switch(g_colorStrategy) { 
    case 2:
      g_colorStrategy = 0;
      break;
    case 3:
      g_colorStrategy = 1;
  }
}


CRGB executeColorStrategy(uint8_t hue, uint8_t brightness) {
  CRGB color;
  switch(g_colorStrategy) {
      case COLOR_STRATEGY_HSV_HUE_AND_BRIGHTNESS:      
        color = CHSV(hue, g_sat1, brightness);
        break;
      case COLOR_STRATEGY_PALETTE_HUE_AND_BRIGHTNESS:
        color = ColorFromPalette(g_palette1, hue, brightness, g_paletteBlending1);
        break;
      case COLOR_STRATEGY_HSV_BRIGHTNESS_FOR_HUE:      
        color = CHSV(brightness, g_sat1, 255);
        break;
      case COLOR_STRATEGY_PALETTE_BRIGHTNESS_FOR_HUE:      
        color = ColorFromPalette(g_palette1, brightness, 255, g_paletteBlending1);
        break;
  }

  return color;
}

uint8_t executePixelPhaseStrategy(uint16_t pixelIndex, uint8_t phaseStrategy, float scale, bool reversePattern) {
  uint8_t phase = 0;
  switch(phaseStrategy) {
    case PHASE_STRATEGY_ROWS:
      phase = phaseFromRowIndex(pixelIndex, NUM_COLUMNS - g_columnGlitchFactor, NUM_ROWS - g_rowGlitchFactor, scale, reversePattern);
      break;
    case PHASE_STRATEGY_COLUMNS:
      phase = phaseFromColumnIndex(pixelIndex, NUM_COLUMNS - g_columnGlitchFactor, scale, reversePattern);
      break;
    // case PHASE_STRATEGY_ODD_EVEN:
    //   phase = phaseFromOddEvenIndex(pixelIndex);
    //   break;
    case PHASE_STRATEGY_STRIP_INDEX:
      phase = phaseFromPixelIndex(pixelIndex, NUM_LEDS - g_pixelGlitchFactor, scale, reversePattern);
      break;
    // case PHASE_STRATEGY_SOLID:
    //   phase = 0;
    //   break;
  }
  return phase;
}

uint8_t executeWaveStrategy(uint8_t waveStrategy, uint8_t bpm, unsigned long startTime, uint8_t phase, uint8_t pulseWidth) {
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
      break;
    case WAVE_STRATEGY_SQUARE:
      waveValue = beatsquare8(bpm, 0, 255, startTime, phase, pulseWidth);
      break;
  }

  return waveValue;
}















//sequences
void fullHueStrategy() {
  if (g_patternsReset) {
    Serial.println("fullHueStrategy");
    g_bpm1 = random8(2,20);
    g_bpm2 = random(2,20); //hue sin
    if (g_waveStrategy2 == WAVE_STRATEGY_SQUARE) {
      disallowColorStrategyBrightnessForHueSwap();
    }
    g_patternsReset = false;
    // g_colorStrategy = COLOR_STRATEGY_HSV_HUE_AND_BRIGHTNESS;
    // g_phaseStrategy1 = PHASE_STRATEGY_ROWS;
    // g_waveStrategy1 = 0;
    // g_waveStrategy2 = 4;
    // g_addGlitter = false;
    // disallowColorStrategyBrightnessForHueSwap();
  }

  EVERY_N_SECONDS(g_everyNSecs) { g_reverse1 = !g_reverse1; }
  
  uint8_t phase, waveVal, hue;
  CRGB color;

  hue = executeWaveStrategy(g_waveStrategy1, g_bpm2, g_startTime, 0, g_pulseWidth1);
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    phase = executePixelPhaseStrategy(i, g_phaseStrategy1, g_scale1, g_reverse1);
    waveVal = executeWaveStrategy(g_waveStrategy2, g_bpm1, g_startTime, phase, g_pulseWidth1);
    color = executeColorStrategy(hue, waveVal);
    leds[i] = color;
  }
  
  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}



void fullRGBStrategy(){
  if (g_patternsReset) {
    Serial.println("strategyRGBWaveAndPhase");
    // g_scale1 = getRandomFloat(0.2f, 2.0f);
    // g_scale2 = getRandomFloat(0.2f, 2.0f);
    // g_scale3 = getRandomFloat(0.2f, 2.0f);
    g_patternsReset = false;
  }
  
  float redPhase, greenPhase, bluePhase;
  uint8_t redVal, greenVal, blueVal;
  
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    redPhase = executePixelPhaseStrategy(i, g_phaseStrategy1, g_scale1, g_reverse1);
    greenPhase = executePixelPhaseStrategy(i, g_phaseStrategy2, g_scale2, g_reverse1);
    bluePhase = executePixelPhaseStrategy(i, g_phaseStrategy3, g_scale3, g_reverse1);
    redVal = executeWaveStrategy(g_waveStrategy1, g_bpm1, g_startTime, redPhase, g_pulseWidth1);
    greenVal = executeWaveStrategy(g_waveStrategy2, g_bpm2, g_startTime, greenPhase, g_pulseWidth2);
    blueVal = executeWaveStrategy(g_waveStrategy3, g_bpm3, g_startTime, bluePhase, g_pulseWidth3);
    leds[i] = CRGB(redVal, greenVal, blueVal);
  }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
  
  EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

void offsetFill() 
{
  if (g_patternsReset) {
    Serial.println("offsetFill");
    g_hue2 = random8(1,100); //delta hue
    g_patternsReset = false;
  }

  switch(g_offsetFillStrategy) {
    case OFFSET_FILL_STRATEGY_RAINBOW:
      fill_rainbow( leds, NUM_LEDS, g_hue1, g_hue2);
      break;
    case OFFSET_FILL_STRATEGY_PALETTE:
      fill_palette(leds, NUM_LEDS, g_hue1, g_hue2, g_palette1, 255, g_paletteBlending1);
      break;
  }
  EVERY_N_MILLISECONDS( g_everyNMillis2 ) { g_hue1+= g_hueSteps1; }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}











//from demo reel example
void confetti() 
{
  if (g_patternsReset) {
    Serial.println("confetti");
    disallowColorStrategyBrightnessForHueSwap();
    g_patternsReset = false;
  }
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  uint8_t pos = random16(NUM_LEDS);
  leds[pos] += executeColorStrategy(g_hue1 + random8(64), 255);
  EVERY_N_MILLISECONDS( g_everyNMillis1 ) { g_hue1+=g_hueSteps1; }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

void sinelon()
{
  if (g_patternsReset) {
    Serial.println("sinelon");
    disallowColorStrategyBrightnessForHueSwap();
    g_bpm1 = random8(5, 80);
    g_patternsReset = false;
  }
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint8_t pos = beatsin16(g_bpm1, 0, NUM_LEDS-1);
  leds[pos] += executeColorStrategy(g_hue1, 255);
  EVERY_N_MILLISECONDS( g_everyNMillis1 ) { g_hue1+=g_hueSteps1; }
  
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
    disallowColorStrategyBrightnessForHueSwap();
    g_patternsReset = false;
  }
  // CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( g_bpm1, 64, 255);
  for( uint8_t i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = executeColorStrategy(g_hue1+(i*2), beat-g_hue1+(i*10));
  }
  
  EVERY_N_MILLISECONDS( g_everyNMillis1 ) { g_hue1+=g_hueSteps1; }

  if (g_addGlitter) {    
    addGlitter(g_glitterChance);
  }
}

// void juggle() {
//   if (g_patternsReset) {
//     Serial.println("juggle");
//     g_everyNMillis1 = random(500,1000);
//     g_hueSteps1 = random8(1,12);
//     disallowColorStrategyBrightnessForHueSwap();
//     g_patternsReset = false;
//   }
//   // eight colored dots, weaving in and out of sync with each other
//   fadeToBlackBy( leds, NUM_LEDS, 20);
//   byte dothue = 0;
//   for( uint8_t i = 0; i < 8; i++) {
//     leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= executeColorStrategy(dothue, 255);
//     dothue += 32;
//   }
  
//   EVERY_N_MILLISECONDS( g_everyNMillis1 ) { g_hue1+=g_hueSteps1; }

//   if (g_addGlitter) {    
//     addGlitter(g_glitterChance);
//   }
// }


void executeDemoReelPattern()
{
  if (g_patternsReset) {
    g_demoReelPatternIndex = random8(3);
  }

  switch(g_demoReelPatternIndex)
  {
    case 0:
      confetti();
      break;
    case 1:
      sinelon();
      break;
    case 2:
      bpm();
      break;
  }
}















//setup
void setupPatterns() {
  // patterns[0] = strategyPhaseWithRGBSquare;
  // patterns[1] = strategyRGBWaveAndPhase;
  patterns[0] = executeDemoReelPattern;
  patterns[1] = fullHueStrategy;
  patterns[2] = offsetFill;
  // patterns[4] = strategyHueWaveWithSquare;
  // patterns[5] = bpm;
  // patterns[6] = sinelon;
  // patterns[6] = juggle;
  // patterns[2] = confetti;
}

void setup() {  
  // delay(2000);
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  randomSeed(analogRead(3));
  random16_set_seed(8934);
  random16_add_entropy(analogRead(3));
  
  setupPatterns();
  // setupTransitions();
  resetPatternGlobals();
  g_patternIndex = random8(NUM_PATTERNS);
}