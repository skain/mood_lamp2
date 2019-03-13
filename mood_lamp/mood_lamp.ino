#include <Adafruit_NeoPixel.h>
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

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
  strip.setBrightness(100);
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop() {
//  red_green_blue_sin(5);
  square_test(15);
  // Some example procedures showing how to display to the pixels:
//  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
//  colorWipe(strip.Color(0, 0, 255), 50); // Blue
////colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
//  // Send a theater pixel chase in...
//  theaterChase(strip.Color(127, 127, 127), 50); // White
//  theaterChase(strip.Color(127, 0, 0), 50); // Red
//  theaterChase(strip.Color(0, 0, 127), 50); // Blue
//
//  rainbow(20);
//  rainbowCycle(20);
//  theaterChaseRainbow(50);
}

// helpers
float have_secs_elapsed(float secs, float start_time) {
  float now = millis();
  if ((now - start_time) > (secs * 1000)) {
    return true;
  }

  return false;
}

float time_from_millis(float t){
  return t / 1000.0f;
}

float compute_sin_wave(float frequency, float phase, float t, float amplitude) {
  return amplitude * sin(TWO_PI * frequency * time_from_millis(t) + phase);
}

float compute_square_wave(float frequency, float phase, float t, float amplitude, float duty_cycle){
  float threshold = 1 - (duty_cycle * 2);
  float sin_val = compute_sin_wave(frequency, phase, t, amplitude);
  if (sin_val > threshold) {
    return 1.0f;
  } 
  
  return 0.0f;
}

float interpolate(float val, float in_min, float in_max, float out_min, float out_max){
  return out_min + (val-in_min) * ((out_max-out_min)/(in_max-in_min));
}

float sin_to_255(float frequency, float phase, float t, float amplitude){
  float sin_val = compute_sin_wave(frequency, phase, t, amplitude);
  return interpolate(sin_val, -1, 1, 0, 255);
}

float square_to_255(float frequency, float phase, float t, float amplitude, float duty_cycle){
  float square_val = compute_square_wave(frequency, phase, t, amplitude, duty_cycle);
  return interpolate(square_val, 0, 1, 0, 255);
}

float phase_from_pixel_index(int pixel_index, int num_pixels){
    float phase = interpolate(pixel_index, 0, num_pixels, 0, PI);
    return phase;
}


//sequences
void red_green_blue_sin(float run_time){
  float green_phase = PI;
  float blue_phase = PI / 4.0f;
  float red_f = 0.01f;
  float green_f = 0.08f;
  float blue_f = 0.04f;
  float t;
  float red_sin;
  float green_sin;
  float blue_sin;
  float start_time = millis();
  
  while(true) {
    t = millis();
    red_sin = sin_to_255(red_f, 0.0f, t, 1.0f);
    green_sin = sin_to_255(green_f, green_phase, t, 1.0f);
    blue_sin = sin_to_255(blue_f, blue_phase, t, 1.0f);
    strip.fill(strip.Color(red_sin, green_sin, blue_sin));
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void possig_test(float run_time){
  float freq = 0.4f;
  uint16_t num_pix = strip.numPixels();
  float phase;
  float cur_sin;
  float start_time = millis();
  while(true){
    float t = millis();
    for(uint16_t i=0; i<num_pix; i++) {
      phase = phase_from_pixel_index(i, num_pix);
      cur_sin = sin_to_255(freq, phase, t, 1.0f);
      strip.setPixelColor(i, strip.Color(cur_sin, 0, 0));
    }

    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}

void possig_3_wave_test(float run_time){
  float red_freq = 0.2f;
  float green_freq = 0.1f;
  float blue_freq = 0.3f;
  uint16_t num_pix = strip.numPixels();
  float t, phase;
  int red_val, green_val, blue_val;
  float start_time = millis();
  
  while(true){
    for(uint16_t i=0; i<num_pix; i++) {
      t = millis();
      phase = phase_from_pixel_index(i, num_pix);
      red_val = sin_to_255(red_freq, phase, t, 1.0f);
      green_val = sin_to_255(green_freq, phase, t, 1.0f);
      blue_val = sin_to_255(blue_freq, phase, t, 1.0f);
      strip.setPixelColor(i, strip.Color(red_val, green_val, blue_val));
    }

    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}

void color_sin_test(float run_time){
  float freq = 0.2f;
  float t, start_time;
  start_time = millis();
  float color_wheel_index;
  
  while(true){
    t = millis();
    color_wheel_index = sin_to_255(freq, 0.0f, t, 1.0f);
    strip.fill(Wheel(color_wheel_index));

    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}

void color_sin_pos_test(float run_time){
  float amp = 1.0f;
  float freq = 0.04f;
  
  uint16_t num_pix = strip.numPixels();
  float t, start_time, phase, color_wheel_index;

  start_time = millis();
  while(true){
    t = millis();
    for(uint16_t i=0; i<num_pix; i++) {
      phase = phase_from_pixel_index(i, num_pix);
      color_wheel_index = sin_to_255(freq, phase, t, 1.0f);
      strip.setPixelColor(i, Wheel(color_wheel_index));
    }

    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}

void square_test(float run_time){
  float freq = 1.0f;
  float duty_cycle = 0.25f;
  float start_time, t, red_val;

  start_time = millis();
  while(true){
    t = millis();
    red_val = square_to_255(freq, 0.0f, t, 1.0f, duty_cycle);
    strip.fill(strip.Color(red_val, 0, 0));
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}






















// adafruit sample code
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

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
