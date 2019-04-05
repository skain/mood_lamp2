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

const int NUM_PATTERNS = 12;
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
  patterns[3] = colorSinTest;
  patterns[4] = colorSinPosTest;
//  patterns[5] = squareTest;
//  patterns[6] = fmSinTest;
//  patterns[7] = fmSin3WaveTest;
//  patterns[8] = fmSin3WavePosSigTest;
  patterns[5] = rowTest;
  patterns[6] = colTest;
  patterns[7] = oddEvenRGBWaveTest;
  patterns[8] = rowRGBWaveTest;
  patterns[9] = colRGBWaveTest;
  patterns[10] = squarePosSigTest;
  patterns[10] = squareRGBPosSigTest;
}

void setupTransitions() {
  transitions[0] = wipeToBlack;
  transitions[1] = wipeToRandom;
}













void loop() {
//  test();
//  squareTest(10);
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
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

float haveSecsElapsed(float secs, float startTime) {
  float now = millis();
  if ((now - startTime) > (secs * 1000)) {
    return true;
  }

  return false;
}

float timeFromMillis(float t){
  return t / 1000.0f;
}

float computeSinWave(float frequency, float phase, float t, float amplitude) {
  return amplitude * sin(TWO_PI * frequency * timeFromMillis(t) + phase);
}

float computeSquareWave(float frequency, float phase, float t, float amplitude, float dutyCycle){
  float threshold = 1 - (dutyCycle * 2);
  float sinVal = computeSinWave(frequency, phase, t, amplitude);
  if (sinVal > threshold) {
    return 1.0f;
  } 
  
  return 0.0f;
}

float interpolate(float val, float inMin, float inMax, float outMin, float outMax){
  return outMin + (val - inMin) * ((outMax - outMin)/(inMax - inMin));
}

float sinTo255(float frequency, float phase, float t, float amplitude){
  float sinVal = computeSinWave(frequency, phase, t, amplitude);
  return interpolate(sinVal, -1, 1, 0, 255);
}

float squareTo255(float frequency, float phase, float t, float amplitude, float dutyCycle){
  float squareVal = computeSquareWave(frequency, phase, t, amplitude, dutyCycle);
  return interpolate(squareVal, 0, 1, 0, 255);
}

float phaseFromPixelIndex_old(float pixelIndex, float numPixels, float scale){
    float phase = interpolate(pixelIndex, 0, numPixels * scale, 0, PI);
    return phase;
}

float phaseFromPixelIndex(float pixelIndex, float numPixels, float scale) {
  int phase = interpolate(pixelIndex, 0, numPixels * scale, 0, 255);
  return phase;
}


float computeSinFmSinWave(float modFreq, float modPhase, float t, float modAmp, float modMin, float modMax, float sinPhase, float sinAmp) {
  float modSin = computeSinWave(modFreq, modPhase, t, modAmp);
  float sinFreq = interpolate(modSin, -1.0f, 1.0f, modMin, modMax);
  return computeSinWave(sinFreq, sinPhase, t, sinAmp);
}

float sinFmSinTo255(float modFreq, float modPhase, float t, float modAmp, float modMin, float modMax, float sinPhase, float sinAmp){
  float sinVal = computeSinFmSinWave(modFreq, modPhase, t, modAmp, modMin, modMax, sinPhase, sinAmp);
  return interpolate(sinVal, -1.0f, 1.0f, 0, 255);
}

float phaseFromOddEvenIndex_old(uint16_t pixelIndex) {
  if (pixelIndex % 2 == 0) {
    return 0.0f;
  } else {
    return PI;
  }
}

int phaseFromOddEvenIndex(uint16_t pixelIndex) {
  if (pixelIndex % 2 == 0) {
    return 0;
  } else {
    return 255 / 2;
  }
}

float phaseFromRowIndex_old(float pixelIndex, float pixelsPerRow, float numRows, float scale){
  if (pixelIndex  == 0) {
    return 0;
  }
  uint16_t rowIndex = pixelIndex / pixelsPerRow;
  return interpolate(rowIndex, 0.0f, (numRows - 1.0f) * scale, 0.0f, PI);
}

float phaseFromRowIndex(float pixelIndex, float pixelsPerRow, float numRows, float scale){
  if (pixelIndex  == 0) {
    return 0;
  }
  uint16_t rowIndex = pixelIndex / pixelsPerRow;
  return interpolate(rowIndex, 0.0f, (numRows - 1.0f) * scale, 0.0f, 255);
}

float phaseFromColumnIndex_old(uint16_t pixelIndex, uint16_t numCols, float scale){
  if (pixelIndex  == 0) {
    return 0;
  }
  uint16_t colIndex = pixelIndex % numCols;
  return interpolate(colIndex, 0, (numCols - 1) * scale, 0.0f, PI);
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
  float startTime = millis();
  
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
  float startTime = millis();
  
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
  float startTime = millis();
  
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
  float startTime = millis();
  
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
  float startTime = millis();
  
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
  float startTime = millis();
  
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
  float t, phase, redVal;
  uint16_t numPixels = strip.numPixels();
//  float freq = 0.7f;
  float freq = getRandomFloat(0.7f, 1.5f);
  float dutyCycle = 0.25f;
  float startTime = millis();
  while(true) {
    t = millis() -  startTime;
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromOddEvenIndex_old(i);
      redVal = squareTo255(freq, phase, t, 1.0f, dutyCycle);
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
  float freq = getRandomFloat(0.1f, 3.0f);
//  freq = 1.0f;
  uint16_t numPixels = strip.numPixels();
  float dutyCycle = getRandomFloat(1 / numPixels, 0.85);
  float startTime, t, redVal, curSin, phase;
  float redScale = 1.0f;

  startTime = millis();
  while(true){
    t = millis() -  startTime;
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromPixelIndex_old(i, numPixels, redScale);
      curSin = squareTo255(freq, phase, t, 1.0f, dutyCycle);
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
  float redFreq = getRandomFloat(0.1f, 3.0f);
  float greenFreq = getRandomFloat(0.1f, 3.0f);
  float blueFreq = getRandomFloat(0.1f, 3.0f);
  float minDutyCycle = 1.0f / (float)strip.numPixels();
//  freq = 1.0f;
  uint16_t numPixels = strip.numPixels();
  float redDutyCycle = getRandomFloat(minDutyCycle, 0.85);
  float greenDutyCycle = getRandomFloat(minDutyCycle, 0.85);
  float blueDutyCycle = getRandomFloat(minDutyCycle, 0.85);
  float startTime, t, redVal, greenVal, blueVal, redSin, greenSin, blueSin, redPhase, greenPhase, bluePhase;
  float redScale = 1.0f;
  float greenScale = 1.0f;
  float blueScale = 1.0f;

  startTime = millis();
  while(true){
    t = millis() -  startTime;
    for(uint16_t i=0; i<numPixels; i++) {
      redPhase = phaseFromPixelIndex_old(i, numPixels, redScale);
      greenPhase = phaseFromPixelIndex_old(i, numPixels, greenScale);
      bluePhase = phaseFromPixelIndex_old(i, numPixels, blueScale);
      redSin = squareTo255(redFreq, redPhase, t, 1.0f, redDutyCycle);
      greenSin = squareTo255(greenFreq, greenPhase, t, 1.0f, greenDutyCycle);
      blueSin = squareTo255(blueFreq, bluePhase, t, 1.0f, blueDutyCycle);
      strip.setPixelColor(i, strip.Color(redSin, greenSin, blueSin));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}





















void colorSinTest(float runTime){
  Serial.println("colorSinTest");
  float freq = 0.2f;
  float t, startTime;
  startTime = millis();
  float color_wheelIndex;
  
  while(true){
    t = millis() -  startTime;
    color_wheelIndex = sinTo255(freq, 0.0f, t, 1.0f);
    strip.fill(Wheel(color_wheelIndex));

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void colorSinPosTest(float runTime){
  Serial.println("colorSinPosTest");
  float amp = 1.0f;
  float freq = 0.09f;
  
  uint16_t numPixels = strip.numPixels();
  float t, startTime, phase, color_wheelIndex;

  startTime = millis();
  while(true){
    t = millis() -  startTime;
    for(uint16_t i=0; i<numPixels; i++) {
      phase = phaseFromPixelIndex_old(i, numPixels, 1.0f);
      color_wheelIndex = sinTo255(freq, phase, t, 1.0f);
      strip.setPixelColor(i, Wheel(color_wheelIndex));
    }

    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield();
  }
}

void squareTest(float runTime){
  Serial.println("squareTest");
  int freq = random(10,100);
  int pulseWidth = 255 * 0.5f;
  float startTime, redVal;

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


void fmSinTest(float runTime){
  Serial.println("fmSinTest");
  float modFreq = 0.1f;
  float t;
  float modSin, redSin;
  float startTime = millis();
  
  while(true) {
    t = millis() -  startTime;
    redSin = sinFmSinTo255(modFreq, 0.0f, t, 1.0f, .001f, .02f, 0.0f, 1.0f);
    strip.fill(strip.Color(redSin, 0, 0));
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void fmSin3WaveTest(float runTime){
  Serial.println("fmSin3WaveTest");
  float redModFreq = 0.1f;
  float greenModFreq = 0.2f;
  float blueModFreq = 0.3f;
  float modAmp = 1.0f;
  float greenPhase = PI/2.0f;
  float bluePhase = greenPhase / 3;
  float modMin = 0.001f;
  float modMax = 0.02f;
  float t;
  float modSin, redSin, greenSin, blueSin;
  float startTime = millis();
  
  while(true) {
    t = millis() -  startTime;
    redSin = sinFmSinTo255(redModFreq, 0.0f, t, modAmp, modMin, modMax, 0.0f, 1.0f);
    greenSin = sinFmSinTo255(greenModFreq, greenPhase, t, modAmp, modMin, modMax, greenPhase, 1.0f); 
    blueSin = sinFmSinTo255(blueModFreq, bluePhase, t, modAmp, modMin, modMax, bluePhase, 1.0f);
    strip.fill(strip.Color(redSin, greenSin, blueSin));
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void fmSin3WavePosSigTest(float runTime){
  Serial.println("fmSin3WavePosSigTest");
  float redModFreq = 0.1f;
  float greenModFreq = 0.2f;
  float blueModFreq = 0.3f;
  float modAmp = 1.0f;
  float greenPhase = PI/2.0f;
  float bluePhase = greenPhase / 3;
  float pixelPhase;  
  float modMin = 0.001f;
  float modMax = 0.02f;
  float t;
  float modSin, redSin, greenSin, blueSin;
  uint16_t numPixels = strip.numPixels();
  float startTime = millis();
  
  while(true) {
    t = millis() -  startTime;
    for(uint16_t i=0; i<numPixels; i++) {
      pixelPhase = phaseFromPixelIndex_old(i, numPixels, 1.0f);
      redSin = sinFmSinTo255(redModFreq, 0.0f, t, modAmp, modMin, modMax, pixelPhase, 1.0f);
      greenSin = sinFmSinTo255(greenModFreq, greenPhase, t, modAmp, modMin, modMax, pixelPhase + greenPhase, 1.0f); 
      blueSin = sinFmSinTo255(blueModFreq, bluePhase, t, modAmp, modMin, modMax, pixelPhase + bluePhase, 1.0f);
      strip.setPixelColor(i, strip.Color(redSin, greenSin, blueSin));
    }
    strip.show();
    if (haveSecsElapsed(runTime, startTime)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}
