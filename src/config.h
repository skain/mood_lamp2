#ifndef confg_h
#define config_h
#define FRAMES_PER_SECOND 120

#define LED_TYPE WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS 150

// consts
#define SECONDS_TO_SHOW 60

// choose the correct data pin for your layout
// #define DATA_PIN 5
#define DATA_PIN 4
// #define DATA_PIN 2

#define POWER_SWITCH_ENABLED 0
// #define POWER_SWITCH_ENABLED 1
#define POWER_SWITCH_PIN 8

// choose the matrix layout
// #define NUM_ROWS 8
// #define NUM_COLUMNS 6
// #define NUM_ROWS 7
// #define NUM_COLUMNS 7
#define NUM_ROWS 10
#define NUM_COLUMNS 5
// #define NUM_ROWS 5
// #define NUM_COLUMNS 5
#define NUM_LEDS NUM_ROWS *NUM_COLUMNS
#endif