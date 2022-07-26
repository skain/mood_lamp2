#include "Arduino.h"
#include "compiler_defs.h"
#include "helpers.h"
#include "pattern_parms.h"
#include "strategy_utils.h"
#include "color_utils.h"
#include <FastLED.h>
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
#define NUM_LEDS NUM_ROWS * NUM_COLUMNS

CRGB leds[NUM_LEDS];

patternParms p1_parms;

void resetPatternGlobals(patternParms *p_parms);

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

void addGlitter(uint8_t chanceOfGlitter)
{
	if (pctToBool(chanceOfGlitter))
	{
		leds[random16(NUM_LEDS)] += CRGB::White;
	}
}

void randomizeReverses(patternParms *p_parms)
{
	p_parms->g_reverse1 = random8(2);
	// Serial.print("p_parms->g_reverse1: ");
	Serial.println(p_parms->g_reverse1);
	p_parms->g_reverse2 = random8(2);
	p_parms->g_reverse3 = random8(2);
}

void resetPatternGlobals(patternParms *p_parms)
{
	Serial.println();
	Serial.println();
	Serial.println("==================");
	random16_set_seed(millis());
	random16_add_entropy(random());

	p_parms->g_predictableRandomSeed = random16();

	p_parms->g_patternsReset = true;

	// uint8_t patternWeights[] = {10, 90};
	// uncomment for sinelon
	// g_patternIndex = calculateWeightedRandom(patternWeights, NUM_PATTERNS);
	p_parms->g_patternIndex = 1;

	p_parms->g_bpm1 = random8();
	p_parms->g_bpm2 = random8();
	p_parms->g_bpm3 = random8();
	p_parms->g_bpmMax1 = p_parms->g_bpm1;
	p_parms->g_bpmMax2 = p_parms->g_bpm2;
	p_parms->g_bpmMax3 = p_parms->g_bpm3;

	p_parms->g_paletteBlending1 = LINEARBLEND;
	p_parms->g_colorIndex = 0;

	p_parms->g_startTime = millis();

	randomizeReverses(p_parms);
	p_parms->g_animateBPM1 = random(2);
	p_parms->g_animateBPM2 = random(2);
	p_parms->g_animateBPM3 = random(2);
	Serial.print("p_parms->BPMAnimates: ");
	Serial.print(p_parms->g_animateBPM1);
	Serial.print(",");
	Serial.print(p_parms->g_animateBPM2);
	Serial.print(",");
	Serial.println(p_parms->g_animateBPM3);

	p_parms->g_BPMAnimationBPM1 = random(2, 10);
	p_parms->g_BPMAnimationBPM2 = random(2, 10);
	p_parms->g_BPMAnimationBPM3 = random(2, 10);

	p_parms->g_phase1 = random8();
	p_parms->g_phase2 = random8();
	p_parms->g_phase3 = random8();

	p_parms->g_addGlitter = pctToBool(15);
	p_parms->g_glitterChance = 60;
	p_parms->g_glitterPercent = random8(40, 80);

	p_parms->g_sat1 = 255;

	p_parms->g_pulseWidth2 = 255 / random8(2, NUM_LEDS);
	p_parms->g_pulseWidth3 = 255 / random8(2, NUM_LEDS);
	p_parms->g_pulseWidth1 = 255 / random8(2, NUM_LEDS);

	p_parms->g_hue1 = random8();
	p_parms->g_paletteOffset = random8(1, 100);

	p_parms->g_hueSteps1 = random8(1, 32);

	p_parms->g_everyNMillis1 = random16(100, 1000);
	p_parms->g_everyNMillis2 = random(50, 250);
	p_parms->g_everyNSecs = random8(3, 15);

	p_parms->g_phaseStrategy1 = random8(0, 4);
	Serial.print("  -p_parms->g_phaseStrategy1: ");
	Serial.println(phaseStrategyToString(p_parms->g_phaseStrategy1));
	p_parms->g_phaseStrategy2 = random8(0, 4);
	Serial.print("  -p_parms->g_phaseStrategy2: ");
	Serial.println(phaseStrategyToString(p_parms->g_phaseStrategy2));
	p_parms->g_phaseStrategy3 = random8(0, 4);
	Serial.print("  -g_phaseStrategy3: ");
	Serial.println(phaseStrategyToString(p_parms->g_phaseStrategy3));

	p_parms->g_waveStrategy1 = random8(0, 5);
	Serial.print("  -p_parms->g_waveStrategy1: ");
	Serial.println(waveStrategyToString(p_parms->g_waveStrategy1));
	p_parms->g_waveStrategy2 = random8(0, 5);
	Serial.print("  -g_waveStrategy2: ");
	Serial.println(waveStrategyToString(p_parms->g_waveStrategy2));
	p_parms->g_waveStrategy3 = random8(0, 5);
	Serial.print("  -g_waveStrategy3: ");
	Serial.println(waveStrategyToString(p_parms->g_waveStrategy3));

	setupRandomPalette(p_parms);

	p_parms->g_colorStrategy = random8(2, 4);

	uint8_t minAmpMax = 96;
	uint8_t minAmpSpread = 96;
	p_parms->g_minAmplitude1 = random8(minAmpMax);
	p_parms->g_maxAmplitude1 = random8(p_parms->g_minAmplitude1 + minAmpSpread, 255);
	p_parms->g_minAmplitude2 = random8(minAmpMax);
	p_parms->g_maxAmplitude2 = random8(p_parms->g_minAmplitude2 + minAmpSpread, 255);
	p_parms->g_minAmplitude3 = random8(minAmpMax);
	p_parms->g_maxAmplitude3 = random8(p_parms->g_minAmplitude3 + minAmpSpread, 255);

	p_parms->g_bifurcatePatterns = pctToBool(70);
	p_parms->g_bifurcatePatterns = false; // comment out to enable bifurcation
	p_parms->g_bifurcateOscillation = pctToBool(60);
	p_parms->g_bifurcatePatternsBy = random8(2, NUM_LEDS / 2);
	p_parms->g_bifurcationStrategy = random(0, NUM_BIFURCATION_STRATEGIES);
	p_parms->g_bifurcationMode = random(0, 3);
	p_parms->g_bifurcateColorStrategy1 = calculateNextColorStrategy(p_parms->g_colorStrategy);
}

void executeColorStrategy(patternParms *p_parms, uint8_t pixelIndex, uint8_t val1, uint8_t val2, uint8_t val3)
{
	uint8_t curColorStrategy = executeBifurcationStrategy(NUM_LEDS, NUM_COLUMNS, NUM_ROWS, pixelIndex, p_parms);
	CHSV color;

	switch (curColorStrategy)
	{
	case COLOR_STRATEGY_RGB:
		leds[pixelIndex] = CRGB(val1, val2, val3);
		break;
	case COLOR_STRATEGY_HSV:
		leds[pixelIndex] = CHSV(val1, constrainSaturation(val2), val3);
		break;
	case COLOR_STRATEGY_PALETTE:
		color = ColorFromPalette(p_parms->g_palette1, val1, val2, p_parms->g_paletteBlending1);
		color.saturation = constrainSaturation(val3);
		leds[pixelIndex] = color;
		break;
	case COLOR_STRATEGY_OFFSET_PALETTE:
		uint8_t colorIndex = p_parms->g_hue1 + (p_parms->g_paletteOffset * pixelIndex);
		color = ColorFromPalette(p_parms->g_palette1, colorIndex, val1, p_parms->g_paletteBlending1);
		color.saturation = constrainSaturation(val2);
		leds[pixelIndex] = color;
		EVERY_N_MILLISECONDS(p_parms->g_everyNMillis2) { p_parms->g_hue1 += p_parms->g_hueSteps1; }
		break;
	}
}

void animateBPMs(patternParms *p_parms)
{
	if (p_parms->g_animateBPM1)
	{
		p_parms->g_bpm1 = map(beatsin8(p_parms->g_BPMAnimationBPM1), 0, 255, 2, p_parms->g_bpmMax1);
	}

	if (p_parms->g_animateBPM2)
	{
		p_parms->g_bpm2 = map(beatsin8(p_parms->g_BPMAnimationBPM2), 0, 255, 2, p_parms->g_bpmMax2);
	}

	if (p_parms->g_animateBPM3)
	{
		p_parms->g_bpm3 = map(beatsin8(p_parms->g_BPMAnimationBPM3), 0, 255, 2, p_parms->g_bpmMax3);
	}
}

// sequences
void fullThreeWaveStrategy(patternParms *p_parms)
{
	animateBPMs(p_parms);
	if (p_parms->g_patternsReset)
	{
		// Serial.println("fullThreeWaveStrategy");
		switch (p_parms->g_colorStrategy)
		{
		case COLOR_STRATEGY_HSV:
			Serial.println("  -COLOR_STRATEGY_HSV");
			if (p_parms->g_minAmplitude2 < 170)
			{
				p_parms->g_minAmplitude2 = 170; // low values for saturation are kind of boring...
				if (p_parms->g_maxAmplitude2 < p_parms->g_minAmplitude2)
				{
					p_parms->g_maxAmplitude2 = 255;
				}
			}
			break;
		case COLOR_STRATEGY_PALETTE:
			Serial.println("  -COLOR_STRATEGY_PALETTE");
			break;
		case COLOR_STRATEGY_RGB:
			Serial.println("  -COLOR_STRATEGY_RGB");
			// similarly, high RGB mins equal low saturation
			if (p_parms->g_minAmplitude1 > 25)
			{
				p_parms->g_minAmplitude1 = 25;
			}

			if (p_parms->g_minAmplitude2 > 25)
			{
				p_parms->g_minAmplitude2 = 25;
			}

			if (p_parms->g_minAmplitude3 > 25)
			{
				p_parms->g_minAmplitude3 = 25;
			}
			break;
		case COLOR_STRATEGY_OFFSET_PALETTE:
			Serial.println("  -COLOR_STRATEGY_OFFSET_PALETTE");
			break;
		}
		// g_bifurcatePatterns = true;
		// g_bifurcatePatternsBy = 2;
		// g_bifurcationMode = BIFURCATION_MODE_BELOW;
		// g_bifurcationStrategy = BIFURCATION_STRATEGY_ROWS;
		// g_bifurcateOscillation = false;
		// g_addGlitter = false;

		if (p_parms->g_bifurcatePatterns)
		{
			Serial.print("  -Bifurcation Strategy: ");
			Serial.print(p_parms->g_bifurcationStrategy);
			Serial.print("-");
			switch (p_parms->g_bifurcationStrategy)
			{
				case 0:
					Serial.println("PIXELS");
					break;
				case 1:
					Serial.println("ROWS");
					break;
				case 2:
					Serial.println("COLS");
			}
			Serial.print("  -Bifurcation Mode: ");
			Serial.print(p_parms->g_bifurcationMode);
			Serial.print("-");
			switch (p_parms->g_bifurcationMode)
			{
			case 0:
				Serial.println("MODULO");
				break;
			case 1:
				Serial.println("BELOW");
				break;
			case 2:
				Serial.println("ALTERNATE");
				break;
			}
			Serial.print("  -Bifurcating by: ");
			Serial.println(p_parms->g_bifurcatePatternsBy);
			Serial.print("    -Bifurcate Color Strategy: ");
			Serial.print(p_parms->g_bifurcateColorStrategy1);
			Serial.print("-");
			Serial.println(colorStrategyToString(p_parms->g_bifurcateColorStrategy1));
			if (p_parms->g_bifurcateOscillation)
			{
				Serial.println("     -Oscillating");
			}
		}

		p_parms->g_patternsReset = false;
	}

	uint8_t phase1, phase2, phase3;
	uint8_t val1, val2, val3;
	bool reverse = p_parms->g_reverse1 && beatsquare8(p_parms->g_bpm1 / 8) > 0;
	random16_set_seed(p_parms->g_predictableRandomSeed); // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.

	for (uint16_t i = 0; i < NUM_LEDS; i++)
	{
		phase1 = executePixelPhaseStrategy(NUM_COLUMNS, NUM_ROWS, NUM_LEDS, i, p_parms->g_phaseStrategy1, 1, reverse, 0, 0, 0);
		phase2 = executePixelPhaseStrategy(NUM_COLUMNS, NUM_ROWS, NUM_LEDS, i, p_parms->g_phaseStrategy2, 1, reverse, 0, 0, 0);
		phase3 = executePixelPhaseStrategy(NUM_COLUMNS, NUM_ROWS, NUM_LEDS, i, p_parms->g_phaseStrategy3, 1, reverse, 0, 0, 0);
		val1 = executeWaveStrategy(p_parms->g_waveStrategy1, p_parms->g_bpm1, p_parms->g_startTime, phase1, p_parms->g_minAmplitude1, p_parms->g_maxAmplitude1, p_parms->g_pulseWidth1);
		val2 = executeWaveStrategy(p_parms->g_waveStrategy2, p_parms->g_bpm2, p_parms->g_startTime, phase2, p_parms->g_minAmplitude2, p_parms->g_maxAmplitude2, p_parms->g_pulseWidth2);
		val3 = executeWaveStrategy(p_parms->g_waveStrategy3, p_parms->g_bpm3, p_parms->g_startTime, phase3, p_parms->g_minAmplitude3, p_parms->g_maxAmplitude3, p_parms->g_pulseWidth3);

		executeColorStrategy(p_parms, i, val1, val2, val3);
	}
	random16_set_seed(millis()); // Re-randomizing the random number seed for other routines.

	if (p_parms->g_addGlitter)
	{
		addGlitter(p_parms->g_glitterChance);
	}
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
		fullThreeWaveStrategy(&p1_parms);
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