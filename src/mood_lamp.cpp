#include "Arduino.h"
#include "compiler_defs.h"
#include "helpers.h"
#include "pattern_parms.h"
#include "strategy_utils.h"
#include "color_utils.h"
#include "pattern_parms_utils.h"
#include <FastLED.h>
#include "config.h"


CRGB leds[NUM_LEDS], leds1[NUM_LEDS], leds2[NUM_LEDS];

patternParms p1_parms, p2_parms;

// void resetPatternGlobals(patternParms *p_parms);

void setBrightnessFromKnob()
{
	uint8_t val = map(analogRead(A0), 0, 1023, 30, 255);
	FastLED.setBrightness(val);
	// EVERY_N_SECONDS(1) { Serial.println(val); }
}

bool checkPowerSwitch(patternParms *p_parms)
{
	if (POWER_SWITCH_ENABLED == 1 && digitalRead(POWER_SWITCH_PIN))
	{
		fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
		FastLED.show();
		if (!p_parms->g_patternsReset)
		{
			resetPatternGlobals(p_parms);
		}
		FastLED.delay(250);
		return false;
	}

	return true;
}

void loop()
{
	// g_patternIndex = 1;
	// g_bifurcatePatterns = true;
	// g_colorStrategy = 3;

	if (!checkPowerSwitch(&p1_parms))
	{
		return;
	}

	// Call the current pattern function once, updating the 'leds' array
	// patterns[g_patternIndex]();
	if (p1_parms.g_transitionUntil > millis())
	{
		fadeToBlackBy(leds, NUM_LEDS, 65);
	}
	else
	{
		p1_parms.g_transitionUntil = 0;
		fullThreeWaveStrategy(leds, NUM_LEDS, NUM_COLUMNS, NUM_ROWS, &p1_parms);
	}

	// send the 'leds' array out to the actual LED strip
	FastLED.show();
	// insert a delay to keep the framerate modest
	FastLED.delay(1000 / FRAMES_PER_SECOND);

	// do some periodic updates
	EVERY_N_SECONDS(SECONDS_TO_SHOW) 
	{
		p1_parms.g_transitionUntil = millis() + 500;
		resetPatternGlobals(&p1_parms);
	} // change patterns periodically
	EVERY_N_MILLIS(25) { setBrightnessFromKnob(); }
}

void setup()
{
	pinMode(POWER_SWITCH_PIN, INPUT);
	digitalWrite(POWER_SWITCH_PIN, HIGH);
	delay(1000);
	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	// FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

	// set master brightness control
	FastLED.setBrightness(BRIGHTNESS);
	Serial.begin(9600);
	random16_set_seed(analogRead(3));
	random16_add_entropy(random());

	resetPatternGlobals(&p1_parms);
}