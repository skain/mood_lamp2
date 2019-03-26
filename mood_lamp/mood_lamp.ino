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

const int NUM_PATTERNS = 10;

void (*patterns[NUM_PATTERNS])(float);

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
  patterns[0] = red_green_blue_sin;
  patterns[1] = possig_test;
  patterns[2] = possig_3_wave_test;
  patterns[3] = color_sin_test;
  patterns[4] = color_sin_pos_test;
//  patterns[5] = square_test;
//  patterns[6] = fm_sin_test;
//  patterns[7] = fm_sin_3_wav_test;
//  patterns[8] = fm_sin_3_wav_possig_test;
  patterns[5] = row_test;
  patterns[6] = col_test;
  patterns[7] = odd_even_rgb_wave_test;
  patterns[8] = row_rgb_wave_test;
  patterns[9] = col_rgb_wave_test;
}















void loop() {
//  col_rgb_wave_test(10);
  patterns[random(NUM_PATTERNS)](60);
  colorWipe(strip.Color(0,0,0), 75);
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

void red_green_blue_sin(float run_time){
  Serial.println("red_green_blue_sin");
  float red_phase = get_random_phase();
  float green_phase = get_random_phase();
  float blue_phase = get_random_phase();
  float red_f = get_random_float(0.01f, 0.75f);
  float green_f = get_random_float(0.01f, 0.75f);
  float blue_f = get_random_float(0.01f, 0.75f);
  float t;
  float red_sin;
  float green_sin;
  float blue_sin;
  float start_time = millis();
  
  while(true) {
    t = millis() -  start_time;
    red_sin = sin_to_255(red_f, red_phase, t, 1.0f);
    green_sin = sin_to_255(green_f, green_phase, t, 1.0f);
    blue_sin = sin_to_255(blue_f, blue_phase, t, 1.0f);
    strip.fill(strip.Color(red_sin, green_sin, blue_sin));
//    strip.fill(strip.Color(red_sin, 0, 0));
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void possig_test(float run_time){
  Serial.println("possig_test");
  float freq = get_random_float(0.2f, 2.0f);
  float red_scale = get_random_float(0.25f, 4.0f);
//  float red_scale = 4.0f;
  uint16_t num_pix = strip.numPixels();
  float phase;
  float cur_sin;
  float start_time = millis();
  float t;
  while(true){
    t = millis() -  start_time;
    for(uint16_t i=0; i<num_pix; i++) {
      phase = phase_from_pixel_index(i, num_pix, red_scale);
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
  Serial.println("possig_3_wave_test");
  float red_freq = get_random_float(0.2f, 2.0f);
  float green_freq = get_random_float(0.2f, 2.0f);
  float blue_freq = get_random_float(0.2f, 2.0f);
  float red_scale = get_random_float(0.2f, 2.0f);
  float green_scale = get_random_float(0.2f, 2.0f);
  float blue_scale = get_random_float(0.2f, 2.0f);
  uint16_t num_pix = strip.numPixels();
  float t, red_phase, green_phase, blue_phase;
  int red_val, green_val, blue_val;
  float start_time = millis();
  
  while(true){
    for(uint16_t i=0; i<num_pix; i++) {
      t = millis() -  start_time;
      red_phase = phase_from_pixel_index(i, num_pix, red_scale);
      green_phase = phase_from_pixel_index(i, num_pix, green_scale);
      blue_phase = phase_from_pixel_index(i, num_pix, blue_scale);
      red_val = sin_to_255(red_freq, red_phase, t, 1.0f);
      green_val = sin_to_255(green_freq, green_phase, t, 1.0f);
      blue_val = sin_to_255(blue_freq, blue_phase, t, 1.0f);
      strip.setPixelColor(i, strip.Color(red_val, green_val, blue_val));
    }

    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}



void odd_even_square_test(float run_time) {
  Serial.println("odd_even_test");
  float t, phase, red_val;
  uint16_t num_pix = strip.numPixels();
//  float freq = 0.7f;
  float freq = get_random_float(0.7f, 1.5f);
  float duty_cycle = 0.25f;
  float start_time = millis();
  while(true) {
    t = millis() -  start_time;
    for(uint16_t i=0; i<num_pix; i++) {
      phase = phase_from_odd_even_index(i);
      red_val = square_to_255(freq, phase, t, 1.0f, duty_cycle);
      strip.setPixelColor(i, strip.Color(red_val, 0, 0));
    }
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void row_test(float run_time){
  Serial.println("row_test");
//  float red_f = 1.0f;
//  float red_scale = 0.25f;
  float red_scale = get_random_float(0.5f, 5.0f);
  float red_f = get_random_float(0.5f, 3.0f);
  float t, red_sin, phase;
  uint16_t num_pix = strip.numPixels();
  float start_time = millis();
  
  while(true) {
    t = millis() -  start_time;
    for(uint16_t i=0; i<num_pix; i++) {
      phase = phase_from_row_index(i, 7, 7, red_scale);
      red_sin = sin_to_255(red_f, phase, t, 1.0f);
      strip.setPixelColor(i, strip.Color(red_sin, 0, 0));
    }
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void col_test(float run_time){
  Serial.println("col_test");
//  float red_f = 0.3f;
  float red_scale = get_random_float(0.5f, 5.0f);
  float red_f = get_random_float(0.3f, 3.0f);
//  float red_f = get_random_float(0.3f, 4.0f);
  float t, red_sin, phase;
  uint16_t num_pix = strip.numPixels();
  float start_time = millis();
  
  while(true) {
    t = millis() -  start_time;
    for(uint16_t i=0; i<num_pix; i++) {
      phase = phase_from_column_index(i, 7, red_scale);
      red_sin = sin_to_255(red_f, phase, t, 1.0f);
      strip.setPixelColor(i, strip.Color(red_sin, 0, 0));
    }
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void odd_even_rgb_wave_test(float run_time){
  Serial.println("odd_even_rgb_wave_test");
  float red_freq = get_random_float(0.1f, 1.0f);
  float green_freq = get_random_float(0.1f, 1.0f);
  float blue_freq = get_random_float(0.1f, 1.0f);
  uint16_t num_pix = strip.numPixels();
  float t, phase;
  int red_val, green_val, blue_val;
  float start_time = millis();
  
  while(true){
    for(uint16_t i=0; i<num_pix; i++) {
      t = millis() -  start_time;
      phase = phase_from_odd_even_index(i);
      red_val = sin_to_255(red_freq, phase, t, 1.0f);
      green_val = sin_to_255(green_freq, phase, t, 1.0f);
      blue_val = sin_to_255(blue_freq, phase, t, 1.0f);
//      strip.setPixelColor(i, strip.Color(red_val, 0, 0));
      strip.setPixelColor(i, strip.Color(red_val, green_val, blue_val));
    }

    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}


void row_rgb_wave_test(float run_time){
  Serial.println("row_rgb_wave_test");
  float red_freq = get_random_float(0.1f, 1.0f);
  float green_freq = get_random_float(0.1f, 1.0f);
  float blue_freq = get_random_float(0.1f, 1.0f);
  float red_scale = get_random_float(0.2f, 2.0f);
  float green_scale = get_random_float(0.2f, 2.0f);
  float blue_scale = get_random_float(0.2f, 2.0f);
  uint16_t num_pix = strip.numPixels();
  float t, red_phase, green_phase, blue_phase;
  int red_val, green_val, blue_val;
  float start_time = millis();
  
  while(true){
    for(uint16_t i=0; i<num_pix; i++) {
      t = millis() -  start_time;
      red_phase = phase_from_row_index(i, 7, 7, red_scale);
      green_phase = phase_from_row_index(i, 7, 7, green_scale);
      blue_phase = phase_from_row_index(i, 7, 7, blue_scale);
      red_val = sin_to_255(red_freq, red_phase, t, 1.0f);
      green_val = sin_to_255(green_freq, green_phase, t, 1.0f);
      blue_val = sin_to_255(blue_freq, blue_phase, t, 1.0f);
      strip.setPixelColor(i, strip.Color(red_val, green_val, blue_val));
    }

    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}

void col_rgb_wave_test(float run_time){
  Serial.println("column_rgb_wave_test");
  float red_freq = get_random_float(0.1f, 1.0f);
  float green_freq = get_random_float(0.1f, 1.0f);
  float blue_freq = get_random_float(0.1f, 1.0f);
  float red_scale = get_random_float(0.2f, 2.0f);
  float green_scale = get_random_float(0.2f, 2.0f);
  float blue_scale = get_random_float(0.2f, 2.0f);
  uint16_t num_pix = strip.numPixels();
  float t, red_phase, green_phase, blue_phase;
  int red_val, green_val, blue_val;
  float start_time = millis();
  
  while(true){
    for(uint16_t i=0; i<num_pix; i++) {
      t = millis() -  start_time;
      red_phase = phase_from_column_index(i, 7, red_scale);
      green_phase = phase_from_column_index(i, 7, green_scale);
      blue_phase = phase_from_column_index(i, 7, blue_scale);
      red_val = sin_to_255(red_freq, red_phase, t, 1.0f);
      green_val = sin_to_255(green_freq, green_phase, t, 1.0f);
      blue_val = sin_to_255(blue_freq, blue_phase, t, 1.0f);
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
  Serial.println("color_sin_test");
  float freq = 0.2f;
  float t, start_time;
  start_time = millis();
  float color_wheel_index;
  
  while(true){
    t = millis() -  start_time;
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
  Serial.println("color_sin_pos_test");
  float amp = 1.0f;
  float freq = 0.09f;
  
  uint16_t num_pix = strip.numPixels();
  float t, start_time, phase, color_wheel_index;

  start_time = millis();
  while(true){
    t = millis() -  start_time;
    for(uint16_t i=0; i<num_pix; i++) {
      phase = phase_from_pixel_index(i, num_pix, 1.0f);
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
  Serial.println("square_test");
  float freq = 1.0f;
  float duty_cycle = 0.25f;
  float start_time, t, red_val;

  start_time = millis();
  while(true){
    t = millis() -  start_time;
    red_val = square_to_255(freq, 0.0f, t, 1.0f, duty_cycle);
    strip.fill(strip.Color(red_val, 0, 0));
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield();
  }
}


void fm_sin_test(float run_time){
  Serial.println("fm_sin_test");
  float mod_freq = 0.1f;
  float t;
  float mod_sin, red_sin;
  float start_time = millis();
  
  while(true) {
    t = millis() -  start_time;
    red_sin = sin_fm_sin_to_255(mod_freq, 0.0f, t, 1.0f, .001f, .02f, 0.0f, 1.0f);
    strip.fill(strip.Color(red_sin, 0, 0));
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void fm_sin_3_wav_test(float run_time){
  Serial.println("fm_sin_3_wav_test");
  float red_mod_freq = 0.1f;
  float green_mod_freq = 0.2f;
  float blue_mod_freq = 0.3f;
  float mod_amp = 1.0f;
  float green_phase = PI/2.0f;
  float blue_phase = green_phase / 3;
  float mod_min = 0.001f;
  float mod_max = 0.02f;
  float t;
  float mod_sin, red_sin, green_sin, blue_sin;
  float start_time = millis();
  
  while(true) {
    t = millis() -  start_time;
    red_sin = sin_fm_sin_to_255(red_mod_freq, 0.0f, t, mod_amp, mod_min, mod_max, 0.0f, 1.0f);
    green_sin = sin_fm_sin_to_255(green_mod_freq, green_phase, t, mod_amp, mod_min, mod_max, green_phase, 1.0f); 
    blue_sin = sin_fm_sin_to_255(blue_mod_freq, blue_phase, t, mod_amp, mod_min, mod_max, blue_phase, 1.0f);
    strip.fill(strip.Color(red_sin, green_sin, blue_sin));
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}

void fm_sin_3_wav_possig_test(float run_time){
  Serial.println("fm_sin_3_wav_possig_test");
  float red_mod_freq = 0.1f;
  float green_mod_freq = 0.2f;
  float blue_mod_freq = 0.3f;
  float mod_amp = 1.0f;
  float green_phase = PI/2.0f;
  float blue_phase = green_phase / 3;
  float pixel_phase;  
  float mod_min = 0.001f;
  float mod_max = 0.02f;
  float t;
  float mod_sin, red_sin, green_sin, blue_sin;
  uint16_t num_pix = strip.numPixels();
  float start_time = millis();
  
  while(true) {
    t = millis() -  start_time;
    for(uint16_t i=0; i<num_pix; i++) {
      pixel_phase = phase_from_pixel_index(i, num_pix, 1.0f);
      red_sin = sin_fm_sin_to_255(red_mod_freq, 0.0f, t, mod_amp, mod_min, mod_max, pixel_phase, 1.0f);
      green_sin = sin_fm_sin_to_255(green_mod_freq, green_phase, t, mod_amp, mod_min, mod_max, pixel_phase + green_phase, 1.0f); 
      blue_sin = sin_fm_sin_to_255(blue_mod_freq, blue_phase, t, mod_amp, mod_min, mod_max, pixel_phase + blue_phase, 1.0f);
      strip.setPixelColor(i, strip.Color(red_sin, green_sin, blue_sin));
    }
    strip.show();
    if (have_secs_elapsed(run_time, start_time)) {
      break;
    }
    yield(); // this is here for the Feather HUZZAH's watchdog
  }
}







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

float phase_from_pixel_index(float pixel_index, float num_pixels, float scale){
    float phase = interpolate(pixel_index, 0, num_pixels * scale, 0, PI);
    return phase;
}


float compute_sin_fm_sin_wave(float mod_freq, float mod_phase, float t, float mod_amp, float mod_min, float mod_max, float sin_phase, float sin_amp) {
  float mod_sin = compute_sin_wave(mod_freq, mod_phase, t, mod_amp);
  float sin_freq = interpolate(mod_sin, -1.0f, 1.0f, mod_min, mod_max);
  return compute_sin_wave(sin_freq, sin_phase, t, sin_amp);
}

float sin_fm_sin_to_255(float mod_freq, float mod_phase, float t, float mod_amp, float mod_min, float mod_max, float sin_phase, float sin_amp){
  float sin_val = compute_sin_fm_sin_wave(mod_freq, mod_phase, t, mod_amp, mod_min, mod_max, sin_phase, sin_amp);
  return interpolate(sin_val, -1.0f, 1.0f, 0, 255);
}

float phase_from_odd_even_index(uint16_t pixel_index) {
  if (pixel_index % 2 == 0) {
    return 0.0f;
  } else {
    return PI;
  }
}

float phase_from_row_index(float pixel_index, float pixels_per_row, float num_rows, float scale){
  if (pixel_index  == 0) {
    return 0;
  }
  uint16_t row_index = pixel_index / pixels_per_row;
  return interpolate(row_index, 0.0f, (num_rows - 1.0f) * scale, 0.0f, PI);
}

float phase_from_column_index(uint16_t pixel_index, uint16_t num_cols, float scale){
  if (pixel_index  == 0) {
    return 0;
  }
  uint16_t col_index = pixel_index % num_cols;
  return interpolate(col_index, 0, (num_cols - 1) * scale, 0.0f, PI);
}

float get_random_float(float min, float max){
  // this is not the greatest implementation but works in the situations I've got right now
  int norm_min = min * 1000;
  int norm_max = max * 1000;
  long r = random(norm_min, norm_max);
  float freq = r / 1000.0f;
  return freq;
}

float get_random_phase(){
  float r = random(0,4);
  if (r == 0) {
    return 0.0f;
  }
  return PI / r;
}
