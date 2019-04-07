#include <FastLED.h>
#define FRAMES_PER_SECOND  120

#define DATA_PIN    2
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    49
#define BRIGHTNESS          96
CRGB leds[NUM_LEDS];

//uint32_t RED = strip.Color(255, 0, 0);
//uint32_t YELLOW = strip.Color(255, 150, 0);
//uint32_t GREEN = strip.Color(0, 255, 0);
//uint32_t CYAN = strip.Color(0, 255, 255);
//uint32_t BLUE = strip.Color(0, 0, 255);
//uint32_t PURPLE = strip.Color(180, 0, 255);
//uint32_t WHITE = strip.Color(255, 255, 255);
//uint32_t BLACK = strip.Color(0, 0, 0);

const int NUM_PATTERNS = 10;
void (*patterns[NUM_PATTERNS])(float);

const int NUM_TRANSITIONS = 2;
void (*transitions[NUM_TRANSITIONS])();
unsigned long patternStartTime;

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
  randomSeed(analogRead(0));
  patternStartTime = millis();
  
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













void loop() {
//  test();
//  squareTest(60);
  patterns[random(NUM_PATTERNS)](60);
  transitions[random(NUM_TRANSITIONS)]();

  // Call the current pattern function once, updating the 'leds' array
//  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
//  FastLED.show();  
  // insert a delay to keep the framerate modest
//  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
//  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
//  EVERY_N_SECONDS( 10 ) { patternStartTime = millis(); patterns[random(NUM_PATTERNS)](60); transitions[random(NUM_TRANSITIONS)](); } // change patterns periodically

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
  randomTimeWipe(CRGB(0,0,0));
}

void wipeToRandom() {
  randomTimeWipe(CRGB(random8(), random8(), random8()));
}

void randomTimeWipe(uint32_t color) {  
  int d = random8(25, 100);
  colorWipe(color, d);
}















//sequences
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
    leds[i] = c;
    FastLED.show();
    delay(wait);
  }
}

void redGreenBlueSin(float runTime){
  Serial.println("redGreenBlueSin");
  int redPhase = random8();
  int greenPhase = random8();
  int bluePhase = random8();
  int redFreq = random8(10,100);
  int greenFreq = random8(10,100);
  int blueFreq = random8(10,100);
  int redSin, greenSin, blueSin;
  unsigned long startTime = millis();
  
  while(true) {
    redSin = beatsin8(redFreq, 0, 255, startTime, redPhase);
    greenSin = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
    blueSin = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
//    Serial.println(redSin);
    fill_solid(leds, NUM_LEDS, CRGB(redSin, greenSin, blueSin));
    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void posSigTest(float runTime){
  Serial.println("posSigTest");
  int freq = random8(10,100);
  float redScale = getRandomFloat(0.25f, 4.0f);
  int phase;
  int curSin;
  unsigned long startTime = millis();
  while(true){
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      phase = phaseFromPixelIndex(i, NUM_LEDS, redScale);
      curSin = beatsin8(freq, 0, 255, startTime, phase);
      leds[i] = CRGB(curSin, 0, 0);
    }

    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void posSig3WaveTest(float runTime){
  Serial.println("posSig3WaveTest");
  int redFreq = random8(10,100);
  int greenFreq = random8(10,100);
  int blueFreq = random8(10,100);
  float redScale = getRandomFloat(0.2f, 2.0f);
  float greenScale = getRandomFloat(0.2f, 2.0f);
  float blueScale = getRandomFloat(0.2f, 2.0f);
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      redPhase = phaseFromPixelIndex(i, NUM_LEDS, redScale);
      greenPhase = phaseFromPixelIndex(i, NUM_LEDS, greenScale);
      bluePhase = phaseFromPixelIndex(i, NUM_LEDS, blueScale);
      redVal = beatsin8(redFreq, 0, 255, startTime, redPhase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
      leds[i] = CRGB(redVal, greenVal, blueVal);
    }

    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void rowTest(float runTime){
  Serial.println("rowTest");
  float redScale = getRandomFloat(0.5f, 5.0f);
  int redFreq = random8(10,100);
  float redSin, phase;
  unsigned long startTime = millis();
  
  while(true) {
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      phase = phaseFromRowIndex(i, 7, 7, redScale);
      redSin = beatsin8(redFreq, 0, 255, startTime, phase);
      leds[i] = CRGB(redSin, 0, 0);
    }
    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void colTest(float runTime){
  Serial.println("colTest");
  float redScale = getRandomFloat(0.5f, 5.0f);
  float redFreq = random8(10,100);
  float redSin, phase;
  unsigned long startTime = millis();
  
  while(true) {
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      phase = phaseFromColumnIndex(i, 7, redScale);
      redSin = beatsin8(redFreq, 0, 255, startTime, phase);
      leds[i] = CRGB(redSin, 0, 0);
    }
    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}


void rowRGBWaveTest(float runTime){
  Serial.println("rowRGBWaveTest");
  int redFreq = random8(10,100);
  int greenFreq = random8(10,100);
  int blueFreq = random8(10,100);
  float redScale = getRandomFloat(0.2f, 2.0f);
  float greenScale = getRandomFloat(0.2f, 2.0f);
  float blueScale = getRandomFloat(0.2f, 2.0f);
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      redPhase = phaseFromRowIndex(i, 7, 7, redScale);
      greenPhase = phaseFromRowIndex(i, 7, 7, greenScale);
      bluePhase = phaseFromRowIndex(i, 7, 7, blueScale);
      redVal = beatsin8(redFreq, 0, 255, startTime, redPhase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
      leds[i] = CRGB(redVal, greenVal, blueVal);
    }

    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void colRGBWaveTest(float runTime){
  Serial.println("column_rgb_wave_test");
  int redFreq = random8(10,100);
  int greenFreq = random8(10,100);
  int blueFreq = random8(10,100);
  float redScale = getRandomFloat(0.2f, 2.0f);
  float greenScale = getRandomFloat(0.2f, 2.0f);
  float blueScale = getRandomFloat(0.2f, 2.0f);
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      redPhase = phaseFromColumnIndex(i, 7, redScale);
      greenPhase = phaseFromColumnIndex(i, 7, greenScale);
      bluePhase = phaseFromColumnIndex(i, 7, blueScale);
      redVal = beatsin8(redFreq, 0, 255, startTime, redPhase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
      leds[i] = CRGB(redVal, greenVal, blueVal);
    }

    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void oddEvenRGBWaveTest(float runTime){
  Serial.println("oddEvenRGBWaveTest");
  int redFreq = random8(10,100);
  int greenFreq = random8(10,100);
  int blueFreq = random8(10,100);
  float phase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      phase = phaseFromOddEvenIndex(i);   
      redVal = beatsin8(redFreq, 0, 255, startTime, phase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, phase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, phase);
      leds[i] = CRGB(redVal, greenVal, blueVal);
    }

    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}



void squarePosSigTest(float runTime){
  Serial.println("squareTest");
  int freq = random8(10,100);
  int pulseWidth = 255 / random(2,NUM_LEDS);
  unsigned long startTime;
  int redVal, curSin, phase;
  float redScale = 1.0f;

  startTime = millis();
  while(true){
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      phase = phaseFromPixelIndex(i, NUM_LEDS, redScale);
      curSin = beatsquare8(freq, 0, 255, startTime, phase, pulseWidth);
      leds[i] = CRGB(curSin, 0, 0);
    }

    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void squareRGBPosSigTest(float runTime){
  Serial.println("squareTest");
  int redFreq = random8(10,100);
  int greenFreq = random8(10,100);
  int blueFreq = random8(10,100);
  float redPulseWidth = 255 / random8(2,NUM_LEDS);
  float greenPulseWidth = 255 / random8(2,NUM_LEDS);
  float bluePulseWidth = 255 / random8(2,NUM_LEDS);
  unsigned long startTime;
  int redVal, greenVal, blueVal, redSin, greenSin, blueSin, redPhase, greenPhase, bluePhase;
  float redScale = getRandomFloat(0.1f, 4.0f);
  float greenScale = getRandomFloat(0.1f, 4.0f);
  float blueScale = getRandomFloat(0.1f, 4.0f);

  startTime = millis();
  while(true){
    for(uint16_t i=0; i<NUM_LEDS; i++) {
      redPhase = phaseFromPixelIndex(i, NUM_LEDS, redScale);
      greenPhase = phaseFromPixelIndex(i, NUM_LEDS, greenScale);
      bluePhase = phaseFromPixelIndex(i, NUM_LEDS, blueScale);
      redSin = beatsquare8(redFreq, 0, 255, startTime, redPhase, redPulseWidth);
      greenSin = beatsquare8(greenFreq, 0, 255, startTime, greenPhase, greenPulseWidth);
      blueSin = beatsquare8(blueFreq, 0, 255, startTime, bluePhase, bluePulseWidth);
      leds[i] = CRGB(redSin, greenSin, blueSin);
    }

    FastLED.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}


















//unused sequences
void squareTest(float runTime){
  Serial.println("squareTest");
  int freq = random8(10,100);
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
  int freq = random8(10,100);
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
