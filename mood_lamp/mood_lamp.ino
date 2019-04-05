#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(49, PIN, NEO_GRB + NEO_KHZ800);

uint32_t RED = strip.Color(255, 0, 0);
uint32_t YELLOW = strip.Color(255, 150, 0);
uint32_t GREEN = strip.Color(0, 255, 0);
uint32_t CYAN = strip.Color(0, 255, 255);
uint32_t BLUE = strip.Color(0, 0, 255);
uint32_t PURPLE = strip.Color(180, 0, 255);
uint32_t WHITE = strip.Color(255, 255, 255);
uint32_t BLACK = strip.Color(0, 0, 0);

const int NUM_PATTERNS = 10;
void (*patterns[NUM_PATTERNS])(float);

const int NUM_TRANSITIONS = 2;
void (*transitions[NUM_TRANSITIONS])();

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
  strip.setBrightness(100);
  strip.show(); // Initialize all pixels to 'off'
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













void loop() {
//  test();
//  squareRGBPosSigTest(10);
  patterns[random(NUM_PATTERNS)](60);
  transitions[random(NUM_TRANSITIONS)]();
}

//void test() {
//  float val, t;
//  float start_time = millis();
//  while(true){
//    t = millis() - start_time;
//    //computeSinWave(float frequency, float phase, float t, float amplitude)
//    val = sinTo255(1.0f, 0.0f, t, 0.5f);
//    Serial.println(val);
//    
//    yield();
//  }
//}








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
  float r = random(0,4);
  if (r == 0) {
    return 0.0f;
  }
  return PI / r;
}

uint32_t getRandomColor(){
  return strip.Color(random(0,256), random(0,256), random(0,256));
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
  randomTimeWipe(strip.Color(0,0,0));
}

void wipeToRandom() {
  uint32_t c = getRandomColor();
  randomTimeWipe(c);
}

void randomTimeWipe(uint32_t color) {  
  int d = random(25, 100);
  colorWipe(color, d);
}















//sequences
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void redGreenBlueSin(float runTime){
  Serial.println("redGreenBlueSin");
  int redPhase = random(0,255);
  int greenPhase = random(0,255);
  int bluePhase = random(0,255);
  int redFreq = random(10,100);
  int greenFreq = random(10,100);
  int blueFreq = random(10,100);
  int redSin, greenSin, blueSin;
  unsigned long startTime = millis();
  
  while(true) {
    redSin = beatsin8(redFreq, 0, 255, startTime, redPhase);
    greenSin = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
    blueSin = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
    strip.fill(strip.Color(redSin, greenSin, blueSin));
//    Serial.println(redSin);
//    strip.fill(strip.Color(redSin, 0, 0));
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void posSigTest(float runTime){
  Serial.println("posSigTest");
  int freq = random(10,100);
  float redScale = getRandomFloat(0.25f, 4.0f);  
  uint16_t numPixels = strip.numPixels();
  int phase;
  int curSin;
  unsigned long startTime = millis();
  while(true){
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromPixelIndex(i, numPixels, redScale);
      curSin = beatsin8(freq, 0, 255, startTime, phase);
      strip.setPixelColor(i, strip.Color(curSin, 0, 0));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void posSig3WaveTest(float runTime){
  Serial.println("posSig3WaveTest");
  int redFreq = random(10,100);
  int greenFreq = random(10,100);
  int blueFreq = random(10,100);
  float redScale = getRandomFloat(0.2f, 2.0f);
  float greenScale = getRandomFloat(0.2f, 2.0f);
  float blueScale = getRandomFloat(0.2f, 2.0f);
  uint16_t numPixels = strip.numPixels();
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<numPixels; i++) {
      redPhase = phaseFromPixelIndex(i, numPixels, redScale);
      greenPhase = phaseFromPixelIndex(i, numPixels, greenScale);
      bluePhase = phaseFromPixelIndex(i, numPixels, blueScale);
      redVal = beatsin8(redFreq, 0, 255, startTime, redPhase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
      strip.setPixelColor(i, strip.Color(redVal, greenVal, blueVal));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}




void rowTest(float runTime){
  Serial.println("rowTest");
  float redScale = getRandomFloat(0.5f, 5.0f);
  int redFreq = random(10,100);
  float redSin, phase;
  uint16_t numPixels = strip.numPixels();
  unsigned long startTime = millis();
  
  while(true) {
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromRowIndex(i, 7, 7, redScale);
      redSin = beatsin8(redFreq, 0, 255, startTime, phase);
      strip.setPixelColor(i, strip.Color(redSin, 0, 0));
    }
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void colTest(float runTime){
  Serial.println("colTest");
  float redScale = getRandomFloat(0.5f, 5.0f);
  float redFreq = random(10,100);
  float redSin, phase;
  uint16_t numPixels = strip.numPixels();
  unsigned long startTime = millis();
  
  while(true) {
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromColumnIndex(i, 7, redScale);
      redSin = beatsin8(redFreq, 0, 255, startTime, phase);
      strip.setPixelColor(i, strip.Color(redSin, 0, 0));
    }
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}


void rowRGBWaveTest(float runTime){
  Serial.println("rowRGBWaveTest");
  int redFreq = random(10,100);
  int greenFreq = random(10,100);
  int blueFreq = random(10,100);
  float redScale = getRandomFloat(0.2f, 2.0f);
  float greenScale = getRandomFloat(0.2f, 2.0f);
  float blueScale = getRandomFloat(0.2f, 2.0f);
  uint16_t numPixels = strip.numPixels();
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<numPixels; i++) {
      redPhase = phaseFromRowIndex(i, 7, 7, redScale);
      greenPhase = phaseFromRowIndex(i, 7, 7, greenScale);
      bluePhase = phaseFromRowIndex(i, 7, 7, blueScale);
      redVal = beatsin8(redFreq, 0, 255, startTime, redPhase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
      strip.setPixelColor(i, strip.Color(redVal, greenVal, blueVal));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void colRGBWaveTest(float runTime){
  Serial.println("column_rgb_wave_test");
  int redFreq = random(10,100);
  int greenFreq = random(10,100);
  int blueFreq = random(10,100);
  float redScale = getRandomFloat(0.2f, 2.0f);
  float greenScale = getRandomFloat(0.2f, 2.0f);
  float blueScale = getRandomFloat(0.2f, 2.0f);
  uint16_t numPixels = strip.numPixels();
  float redPhase, greenPhase, bluePhase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<numPixels; i++) {
      redPhase = phaseFromColumnIndex(i, 7, redScale);
      greenPhase = phaseFromColumnIndex(i, 7, greenScale);
      bluePhase = phaseFromColumnIndex(i, 7, blueScale);
      redVal = beatsin8(redFreq, 0, 255, startTime, redPhase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, greenPhase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, bluePhase);
      strip.setPixelColor(i, strip.Color(redVal, greenVal, blueVal));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void oddEvenRGBWaveTest(float runTime){
  Serial.println("oddEvenRGBWaveTest");
  int redFreq = random(10,100);
  int greenFreq = random(10,100);
  int blueFreq = random(10,100);
  uint16_t numPixels = strip.numPixels();
  float phase;
  int redVal, greenVal, blueVal;
  unsigned long startTime = millis();
  
  while(true){
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromOddEvenIndex(i);   
      redVal = beatsin8(redFreq, 0, 255, startTime, phase);
      greenVal = beatsin8(greenFreq, 0, 255, startTime, phase);
      blueVal = beatsin8(blueFreq, 0, 255, startTime, phase);
      strip.setPixelColor(i, strip.Color(redVal, greenVal, blueVal));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void odd_even_squareTest(float runTime) {
  Serial.println("odd_even_test");
  int phase, redVal;
  uint16_t numPixels = strip.numPixels();
  int freq = random(10,100);
  int pulseWidth = 255 * 0.25f;
  unsigned long startTime = millis();
  while(true) {
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromOddEvenIndex(i);
      redVal = beatsquare8(freq, 0, 255, startTime, phase, pulseWidth);
      strip.setPixelColor(i, strip.Color(redVal, 0, 0));
    }
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void squarePosSigTest(float runTime){
  Serial.println("squareTest");
  int freq = random(10,100);
  uint16_t numPixels = strip.numPixels();
  int pulseWidth = 255 / random(2,numPixels);
  unsigned long startTime;
  int redVal, curSin, phase;
  float redScale = 1.0f;

  startTime = millis();
  while(true){
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromPixelIndex(i, numPixels, redScale);
      curSin = beatsquare8(freq, 0, 255, startTime, phase, pulseWidth);
      strip.setPixelColor(i, strip.Color(curSin, 0, 0));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void squareRGBPosSigTest(float runTime){
  Serial.println("squareTest");
  int redFreq = random(10,100);
  int greenFreq = random(10,100);
  int blueFreq = random(10,100);
  uint16_t numPixels = strip.numPixels();
  float redPulseWidth = 255 / random(2,numPixels);
  float greenPulseWidth = 255 / random(2,numPixels);
  float bluePulseWidth = 255 / random(2,numPixels);
  unsigned long startTime;
  int redVal, greenVal, blueVal, redSin, greenSin, blueSin, redPhase, greenPhase, bluePhase;
  float redScale = getRandomFloat(0.1f, 4.0f);
  float greenScale = getRandomFloat(0.1f, 4.0f);
  float blueScale = getRandomFloat(0.1f, 4.0f);

  startTime = millis();
  while(true){
    for(uint16_t i=0; i<numPixels; i++) {
      redPhase = phaseFromPixelIndex(i, numPixels, redScale);
      greenPhase = phaseFromPixelIndex(i, numPixels, greenScale);
      bluePhase = phaseFromPixelIndex(i, numPixels, blueScale);
      redSin = beatsquare8(redFreq, 0, 255, startTime, redPhase, redPulseWidth);
      greenSin = beatsquare8(greenFreq, 0, 255, startTime, greenPhase, greenPulseWidth);
      blueSin = beatsquare8(blueFreq, 0, 255, startTime, bluePhase, bluePulseWidth);
      strip.setPixelColor(i, strip.Color(redSin, greenSin, blueSin));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}


















//unused sequences
void squareTest(float runTime){
  Serial.println("squareTest");
  int freq = random(10,100);
  int pulseWidth = 255 * 0.5f;
  unsigned long startTime, redVal;

  startTime = millis();
  while(true){
//    redVal = squareTo255(freq, 0.0f, t, 1.0f, dutyCycle);
    redVal = beatsquare8(freq, 0, 255, startTime, 0, pulseWidth);
//    Serial.println(redVal);
    strip.fill(strip.Color(redVal, 0, 0));
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}
