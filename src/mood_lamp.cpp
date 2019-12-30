#include "Arduino.h"
#include "helpers.h"
#include <FastLED.h>
#define FRAMES_PER_SECOND 120

#define LED_TYPE WS2811
#define COLOR_ORDER RGB
#define BRIGHTNESS 150

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

// consts
#define NUM_PATTERNS 2

//choose the correct data pin for your layout
#define DATA_PIN 2
// #define DATA_PIN 4

#define POWER_SWITCH_ENABLED 0
// #define POWER_SWITCH_ENABLED 1
#define POWER_SWITCH_PIN 8

//choose the matrix layout
#define NUM_ROWS 5
#define NUM_COLUMNS 5
// #define NUM_ROWS 8
// #define NUM_COLUMNS 6
// #define NUM_ROWS 7
// #define NUM_COLUMNS 7
// #define NUM_ROWS 10
// #define NUM_COLUMNS 5
#define NUM_LEDS NUM_ROWS * NUM_COLUMNS

#define PHASE_STRATEGY_ROWS 0		 // Phase calculated by row
#define PHASE_STRATEGY_COLUMNS 1	 // Phase calculated by column
#define PHASE_STRATEGY_STRIP_INDEX 2 // Phase calculated directly by pixel location on strip
#define PHASE_STRATEGY_RANDOM 3      // Random phases calculated with predictable seed
// #define PHASE_STRATEGY_SOLID 3 // All pixels assigned a phase of 0

#define WAVE_STRATEGY_SIN 0		 // basic sin
#define WAVE_STRATEGY_SAW 1		 // basic sawtooth
#define WAVE_STRATEGY_TRIANGLE 2 // basic triangle (linear slopes)
#define WAVE_STRATEGY_CUBIC 3	 // basic cubic (spends more time at limits than sine)
#define WAVE_STRATEGY_SQUARE 4   // basic square (on or off)

#define COLOR_STRATEGY_RGB 0	   // apply 3 different waves to R, G and B
#define COLOR_STRATEGY_HSV 1	   // apply 3 different waves to H, S and V
#define COLOR_STRATEGY_PALETTE 2  // apply 2 of the three waves to palette H and V (no S with palettes...)
#define COLOR_STRATEGY_OFFSET_PALETTE 3 // use and offset to fill from palette and apply one wave to V (H is calculated directly off of pixel index + offset)

#define NUM_BIFURCATION_STRATEGIES 3   // count of bifurcation strats
#define BIFURCATION_STRATEGY_PIXELS 0  // apply a different pattern to pixels based on the modulo of the pixel index
#define BIFURCATION_STRATEGY_ROWS 1   // apply a different pattern to pixels based on rows
#define BIFURCATION_STRATEGY_COLS 2   // apply a different pattern to pixels base on columns
#define BIFURCATION_MODE_MODULO 0 // apply a different pattern to pixels based on the modulo of the pixel index, row, or column
#define BIFURCATION_MODE_BELOW 1 // apply a different pattern to pixels that have an index, row or column below a specific value
#define BIFURCATION_MODE_ALTERNATE 2 //apply a different pattern to every other pixel (and handle even count rows correctly)

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
uint8_t g_hue1, g_paletteOffset;
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
uint8_t g_colorStrategy, g_bifurcateColorStrategy1;
uint8_t g_phaseStrategy1, g_phaseStrategy2, g_phaseStrategy3;
uint8_t g_waveStrategy1, g_waveStrategy2, g_waveStrategy3;
uint8_t g_demoReelPatternIndex;
unsigned int g_rowGlitchFactor, g_columnGlitchFactor, g_pixelGlitchFactor;
uint8_t g_minAmplitude1, g_maxAmplitude1, g_minAmplitude2, g_maxAmplitude2, g_minAmplitude3, g_maxAmplitude3;
bool g_bifurcatePatterns, g_bifurcateOscillation;
uint8_t g_bifurcatePatternsBy, g_bifurcationStrategy, g_bifurcationMode;
uint16_t g_predictableRandomSeed;


void resetPatternGlobals();

void doPeriodicUpdates()
{
	resetPatternGlobals();
}

void setBrightnessFromKnob()
{
	uint8_t val = interpolate(analogRead(A0), 0, 1023, 30, 255);
	FastLED.setBrightness(val);
	// EVERY_N_SECONDS(1) { Serial.println(val); }
}

bool checkPowerSwitch()
{
	if (POWER_SWITCH_ENABLED == 1 && digitalRead(POWER_SWITCH_PIN))
	{
		fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
		FastLED.show();
		if (!g_patternsReset) {
			resetPatternGlobals();
		}
		delay(250);
		return false;
	}

	return true;
}

void loop()
{
	// g_patternIndex = 1;
	// g_bifurcatePatterns = true;
	// g_colorStrategy = 3;

	if (! checkPowerSwitch()) {
		return;
	}

	// Call the current pattern function once, updating the 'leds' array
	patterns[g_patternIndex]();

	// send the 'leds' array out to the actual LED strip
	FastLED.show();
	// insert a delay to keep the framerate modest
	FastLED.delay(1000 / FRAMES_PER_SECOND);

	// do some periodic updates
	EVERY_N_SECONDS(20) { doPeriodicUpdates(); } // change patterns periodically
	EVERY_N_MILLIS(25) { setBrightnessFromKnob(); }
}

// helpers that require access to globals TODO: fix this

// This function fills the palette with totally random colors.
void setupRandomPalettes()
{
	uint8_t weights[] = {25, 35, 35};
	switch (calculateWeightedRandom(weights, 3))
	// switch (2)
	{
	case 0:
		Serial.println("16 random palette");
		for (uint8_t i = 0; i < 16; i++)
		{
			g_palette1[i] = CHSV(random8(), 255, random8(10, 255));
		}
		break;
	case 1:
		Serial.println("2 random palette");
		g_palette1 = CRGBPalette16(getRandomColor(), getRandomColor());
		break;
	case 2:
		Serial.println("4 random palette");
		CRGB c1, c2, c3, c4;
		c1 = getRandomColor();
		c2 = getRandomColor();
		c3 = getRandomColor();
		c4 = getRandomColor();
		g_palette1 = CRGBPalette16(c1, c2, c3, c4);
		break;
	}
}

void addGlitter(uint8_t chanceOfGlitter)
{
	if (pctToBool(chanceOfGlitter))
	{
		leds[random16(NUM_LEDS)] += CRGB::White;
	}
}

void randomizeReverses()
{
	g_reverse1 = random8(2);
	g_reverse2 = random8(2);
	g_reverse3 = random8(2);
}

uint8_t calculateNextColorStrategy(uint8_t curColorStrategy)
{
	uint8_t nextColorStrategy = curColorStrategy;
	while (nextColorStrategy == curColorStrategy)
	{
		nextColorStrategy = random8(4);
	}
	return nextColorStrategy;
}

void resetPatternGlobals()
{
	random16_set_seed(millis());
	random16_add_entropy(random());

	g_predictableRandomSeed = random16();

	g_patternsReset = true;

	uint8_t patternWeights[] = {10, 90};
	g_patternIndex = calculateWeightedRandom(patternWeights, NUM_PATTERNS);

	g_bpm1 = random8();
	g_bpm2 = random8();
	g_bpm3 = random8();

	g_paletteBlending1 = LINEARBLEND;
	g_colorIndex = 0;

	g_startTime = millis();

	randomizeReverses();

	g_phase1 = random8();
	g_phase2 = random8();
	g_phase3 = random8();

	g_addGlitter = pctToBool(15);
	g_glitterChance = 60;
	g_glitterPercent = random8(40, 80);

	g_scale1 = getRandomFloat(0.1f, 4.0f);
	g_scale2 = getRandomFloat(0.1f, 4.0f);
	g_scale3 = getRandomFloat(0.1f, 4.0f);

	g_sat1 = 255;

	g_pulseWidth1 = 255 / random8(2, NUM_LEDS);
	g_pulseWidth2 = 255 / random8(2, NUM_LEDS);
	g_pulseWidth3 = 255 / random8(2, NUM_LEDS);

	g_hue1 = random8();
	g_paletteOffset = random8(1, 100);

	g_hueSteps1 = random8(1, 32);

	g_everyNMillis1 = random16(100, 1000);
	g_everyNMillis2 = random(50, 250);
	g_everyNSecs = random8(3, 15);

	g_phaseStrategy1 = random8(0, 4);
	g_phaseStrategy2 = random8(0, 4);
	g_phaseStrategy3 = random8(0, 4);

	g_waveStrategy1 = random8(0, 5);
	g_waveStrategy2 = random8(0, 5);
	g_waveStrategy3 = random8(0, 5);

	setupRandomPalettes();

	g_rowGlitchFactor = g_columnGlitchFactor = g_pixelGlitchFactor = 0;

	if (pctToBool(20))
	{
		g_rowGlitchFactor = random8(1, NUM_COLUMNS);
	}

	if (pctToBool(20))
	{
		g_columnGlitchFactor = random8(1, NUM_ROWS);
	}

	if (pctToBool(20))
	{
		g_pixelGlitchFactor = random8(1, NUM_LEDS);
	}

	g_colorStrategy = random8(0, 4);

	uint8_t minAmpMax = 96;
	uint8_t minAmpSpread = 96;
	g_minAmplitude1 = random8(minAmpMax);
	g_maxAmplitude1 = random8(g_minAmplitude1 + minAmpSpread, 255);
	g_minAmplitude2 = random8(minAmpMax);
	g_maxAmplitude2 = random8(g_minAmplitude2 + minAmpSpread, 255);
	g_minAmplitude3 = random8(minAmpMax);
	g_maxAmplitude3 = random8(g_minAmplitude3 + minAmpSpread, 255);
	
	g_bifurcatePatterns = pctToBool(70);
	g_bifurcateOscillation = pctToBool(60);
	g_bifurcatePatternsBy = random8(2, NUM_LEDS / 2);
	g_bifurcationStrategy = random(0, NUM_BIFURCATION_STRATEGIES);
	g_bifurcationMode = random(0, 3);
	g_bifurcateColorStrategy1 = calculateNextColorStrategy(g_colorStrategy);
}

uint8_t executePixelPhaseStrategy(uint16_t pixelIndex, uint8_t phaseStrategy, float scale, bool reversePattern,
								  uint8_t columnGlitchFactor, uint8_t rowGlitchFactor, uint8_t pixelGlitchFactor)
{
	uint8_t phase = 0;
	switch (phaseStrategy)
	{
	case PHASE_STRATEGY_ROWS:
		phase = phaseFromRowIndex(pixelIndex, NUM_COLUMNS - columnGlitchFactor, NUM_ROWS - rowGlitchFactor, scale, reversePattern);
		break;
	case PHASE_STRATEGY_COLUMNS:
		phase = phaseFromColumnIndex(pixelIndex, NUM_COLUMNS - columnGlitchFactor, scale, reversePattern);
		break;
	case PHASE_STRATEGY_STRIP_INDEX:
		phase = phaseFromPixelIndex(pixelIndex, NUM_LEDS - pixelGlitchFactor, scale, reversePattern);
		break;
	case PHASE_STRATEGY_RANDOM:
		// NOTE: g_predictableRandomSeed must be set OUTSIDE the loop for this strategy to work!
		phase = random8();
		break;
	}
	return phase;
}

uint8_t executeWaveStrategy(uint8_t waveStrategy, uint8_t bpm, unsigned long startTime, uint8_t phase,
							uint8_t minAmplitude, uint8_t maxAmplitude, uint8_t pulseWidth)
{
	uint8_t waveValue = 0;
	switch (waveStrategy)
	{
	case WAVE_STRATEGY_SIN:
		waveValue = beatsin8(bpm, minAmplitude, maxAmplitude, startTime, phase);
		break;
	case WAVE_STRATEGY_SAW:
		waveValue = beatsaw8(bpm, minAmplitude, maxAmplitude, startTime, phase);
		break;
	case WAVE_STRATEGY_TRIANGLE:
		waveValue = beattriwave8(bpm, minAmplitude, maxAmplitude, startTime, phase);
		break;
	case WAVE_STRATEGY_CUBIC:
		waveValue = beatcubicwave8(bpm, minAmplitude, maxAmplitude, startTime, phase);
		break;
	case WAVE_STRATEGY_SQUARE:
		waveValue = beatsquare8(bpm, minAmplitude, maxAmplitude, startTime, phase, pulseWidth);
		break;
	}

	return waveValue;
}

uint8_t executeBifurcationStrategy(uint8_t pixelIndex)
{
	uint8_t curColorStrategy, bifurcateVal, calculatedIndex, maxAmplitude;
	bool isBifurcated = false;

	curColorStrategy = g_colorStrategy;
	bifurcateVal = g_bifurcatePatternsBy;

	if (g_bifurcatePatterns)
	{
		switch (g_bifurcationStrategy)
		{
			case BIFURCATION_STRATEGY_PIXELS:
				switch (g_bifurcationMode)
				{
					case BIFURCATION_MODE_MODULO:
						if (g_bifurcateOscillation)
						{
							bifurcateVal = executeWaveStrategy(g_waveStrategy1, g_bpm1 / 6, g_startTime, 0, 0, g_bifurcatePatternsBy, g_pulseWidth1);
						}

						if (pixelIndex % bifurcateVal == 0)
						{
							isBifurcated = true;
						}
						break;
					case BIFURCATION_MODE_BELOW:
						if (g_bifurcateOscillation)
						{
							bifurcateVal = executeWaveStrategy(g_waveStrategy1, g_bpm1 / 2, g_startTime, 0, 0, NUM_LEDS, g_pulseWidth1);
						}

						if (pixelIndex < bifurcateVal)
						{
							isBifurcated = true;
						}
						break;
					case BIFURCATION_MODE_ALTERNATE:
						if (g_bifurcateOscillation)
						{
							bifurcateVal = beatsquare8(g_bpm1 / 6, 0, 1, g_startTime, 0);
						}

						calculatedIndex = pixelIndex;
						if (NUM_COLUMNS % 2 == 0 && calculatePixelRow(pixelIndex, NUM_COLUMNS) % 2 != 0)
						{
							calculatedIndex += 1;
						}

						if (calculatedIndex % 2 == 0)
						{
							if (bifurcateVal > 0) {
								isBifurcated = true;
							}
						} 
						else 
						{
							if (bifurcateVal == 0)
							{
								isBifurcated = true;
							}
						}
						break;
				}
				break;
			case BIFURCATION_STRATEGY_COLS:
			case BIFURCATION_STRATEGY_ROWS:
				if (g_bifurcationStrategy == BIFURCATION_STRATEGY_COLS)
				{
					calculatedIndex = calculatePixelColumn(pixelIndex, NUM_COLUMNS);
					maxAmplitude = NUM_COLUMNS;
				} else {
					calculatedIndex = calculatePixelRow(pixelIndex, NUM_COLUMNS);
					maxAmplitude = NUM_ROWS;
				}
				switch(g_bifurcationMode)
				{
					case BIFURCATION_MODE_MODULO:
						if (g_bifurcateOscillation)
						{
							bifurcateVal = executeWaveStrategy(g_waveStrategy1, g_bpm1 / 6, g_startTime, 0, 0, g_bifurcatePatternsBy, g_pulseWidth1);
						}

						if (calculatedIndex % bifurcateVal == 0)
						{
							isBifurcated = true;
						}
						break;
					case BIFURCATION_MODE_BELOW:
						if (g_bifurcateOscillation)
						{
							bifurcateVal = executeWaveStrategy(g_waveStrategy1, g_bpm1 / 3, g_startTime, 0, 0, maxAmplitude, g_pulseWidth1);
						}

						if (calculatedIndex < bifurcateVal)
						{
							isBifurcated = true;
						}
						break;
					case BIFURCATION_MODE_ALTERNATE:
						if (g_bifurcateOscillation)
						{
							bifurcateVal = beatsquare8(g_bpm1 / 6, 0, 1, g_startTime, 0);
						}

						if (calculatedIndex % 2 == 0)
						{
							if (bifurcateVal > 0)
							{
								isBifurcated = true;
							}
						}
						else
						{
							if (bifurcateVal == 0)
							{
								isBifurcated = true;
							}
						}
						break;
				}
				break;
		}

		if (isBifurcated)
		{
			curColorStrategy = g_bifurcateColorStrategy1;
		}
	}


	return curColorStrategy;
}

void executeColorStrategy(uint8_t pixelIndex, uint8_t val1, uint8_t val2, uint8_t val3)
{
	uint8_t curColorStrategy = executeBifurcationStrategy(pixelIndex);

	switch (curColorStrategy)
	{
		case COLOR_STRATEGY_RGB:
			leds[pixelIndex] = CRGB(val1, val2, val3);
			break;
		case COLOR_STRATEGY_HSV:
			if (val2 < 170) {
				val2 = 170; // prevent low saturation values
			}
			leds[pixelIndex] = CHSV(val1, val2, val3);
			break;
		case COLOR_STRATEGY_PALETTE:
			leds[pixelIndex] = ColorFromPalette(g_palette1, val1, val2, g_paletteBlending1);
			break;
		case COLOR_STRATEGY_OFFSET_PALETTE:
			uint8_t colorIndex = g_hue1 + (g_paletteOffset * pixelIndex);
			leds[pixelIndex] = ColorFromPalette(g_palette1, colorIndex, val1, g_paletteBlending1);
			EVERY_N_MILLISECONDS(g_everyNMillis2) { g_hue1 += g_hueSteps1; }
			break;
	}

}

//sequences
void fullThreeWaveStrategy()
{
	if (g_patternsReset)
	{
		Serial.println("fullThreeWaveStrategy");
		switch (g_colorStrategy)
		{
		case COLOR_STRATEGY_HSV:
			Serial.println("  -COLOR_STRATEGY_HSV");
			if (g_minAmplitude2 < 170)
			{
				g_minAmplitude2 = 170; // low values for saturation are kind of boring...
				if (g_maxAmplitude2 < g_minAmplitude2)
				{
					g_maxAmplitude2 = 255;
				}
			}
			break;
		case COLOR_STRATEGY_PALETTE:
			Serial.println("  -COLOR_STRATEGY_PALETTE");
			break;
		case COLOR_STRATEGY_RGB:
			Serial.println("  -COLOR_STRATEGY_RGB");
			// similarly, high RGB mins equal low saturation
			if (g_minAmplitude1 > 25)
			{
				g_minAmplitude1 = 25;
			}

			if (g_minAmplitude2 > 25)
			{
				g_minAmplitude2 = 25;
			}

			if (g_minAmplitude3 > 25)
			{
				g_minAmplitude3 = 25;
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

		if (g_bifurcatePatterns) {
			Serial.print("  -Bifurcation Strategy: ");
			Serial.println(g_bifurcationStrategy);
			Serial.print("  -Bifurcation Mode: ");
			Serial.println(g_bifurcationMode);
			Serial.print("  -Bifurcating by: ");
			Serial.println(g_bifurcatePatternsBy);
			Serial.print("    -New Patterns: ");
			Serial.print(g_colorStrategy);
			Serial.print(", ");
			Serial.println(g_bifurcateColorStrategy1);
			if (g_bifurcateOscillation) {
				Serial.println("     -Oscillating");
			}
		}

		g_patternsReset = false;
	}

	uint8_t phase1, phase2, phase3;
	uint8_t val1, val2, val3;
	random16_set_seed(g_predictableRandomSeed); // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.

	for (uint16_t i = 0; i < NUM_LEDS; i++)
	{
		phase1 = executePixelPhaseStrategy(i, g_phaseStrategy1, g_scale1, g_reverse1, g_columnGlitchFactor, g_rowGlitchFactor, g_pixelGlitchFactor);
		phase2 = executePixelPhaseStrategy(i, g_phaseStrategy2, g_scale2, g_reverse1, g_columnGlitchFactor, g_rowGlitchFactor, g_pixelGlitchFactor);
		phase3 = executePixelPhaseStrategy(i, g_phaseStrategy3, g_scale3, g_reverse1, g_columnGlitchFactor, g_rowGlitchFactor, g_pixelGlitchFactor);
		val1 = executeWaveStrategy(g_waveStrategy1, g_bpm1, g_startTime, phase1, g_minAmplitude1, g_maxAmplitude1, g_pulseWidth1);
		val2 = executeWaveStrategy(g_waveStrategy2, g_bpm2, g_startTime, phase2, g_minAmplitude2, g_maxAmplitude2, g_pulseWidth2);
		val3 = executeWaveStrategy(g_waveStrategy3, g_bpm3, g_startTime, phase3, g_minAmplitude3, g_maxAmplitude3, g_pulseWidth3);

		executeColorStrategy(i, val1, val2, val3);
	}
	random16_set_seed(millis()); // Re-randomizing the random number seed for other routines.

	if (g_addGlitter)
	{
		addGlitter(g_glitterChance);
	}

	EVERY_N_SECONDS(g_everyNSecs) { randomizeReverses(); }
}

//from demo reel example
void confetti()
{
	if (g_patternsReset)
	{
		Serial.println("confetti");
		g_patternsReset = false;
	}
	// random colored speckles that blink in and fade smoothly
	fadeToBlackBy(leds, NUM_LEDS, 10);
	uint8_t pos = random16(NUM_LEDS);
	leds[pos] += ColorFromPalette(g_palette1, g_hue1 + random8(64), 255, g_paletteBlending1);

	EVERY_N_MILLISECONDS(g_everyNMillis1) { g_hue1 += g_hueSteps1; }

	if (g_addGlitter)
	{
		addGlitter(g_glitterChance);
	}
}

void sinelon()
{
	if (g_patternsReset)
	{
		Serial.println("sinelon");
		g_bpm1 = random8(5, 80);
		g_patternsReset = false;
	}
	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy(leds, NUM_LEDS, 20);
	uint8_t pos = beatsin16(g_bpm1, 0, NUM_LEDS - 1);
	leds[pos] += ColorFromPalette(g_palette1, g_hue1, 255, g_paletteBlending1);
	EVERY_N_MILLISECONDS(g_everyNMillis1) { g_hue1 += g_hueSteps1; }

	if (g_addGlitter)
	{
		addGlitter(g_glitterChance);
	}
}

void bpm()
{
	if (g_patternsReset)
	{
		Serial.println("bpm");
		g_bpm1 = random8(10, 120);
		g_hueSteps1 = random8(1, 16);
		g_patternsReset = false;
	}

	uint8_t beat = beatsin8(g_bpm1, 64, 255);
	for (uint8_t i = 0; i < NUM_LEDS; i++)
	{
		leds[i] = ColorFromPalette(g_palette1, g_hue1 + (i * 2), beat - g_hue1 + (i * 10), g_paletteBlending1);
	}

	EVERY_N_MILLISECONDS(g_everyNMillis1) { g_hue1 += g_hueSteps1; }

	if (g_addGlitter)
	{
		addGlitter(g_glitterChance);
	}
}

void executeDemoReelPattern()
{
	if (g_patternsReset)
	{
		g_demoReelPatternIndex = random8(3);
	}

	switch (g_demoReelPatternIndex)
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
void setupPatterns()
{
	patterns[0] = executeDemoReelPattern;
	patterns[1] = fullThreeWaveStrategy;
	// patterns[2] = offsetFill;
}

void setup()
{
	pinMode(POWER_SWITCH_PIN, INPUT);
	digitalWrite(POWER_SWITCH_PIN, HIGH);
	delay(1000);
	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	//FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

	// set master brightness control
	FastLED.setBrightness(BRIGHTNESS);
	Serial.begin(9600);
	random16_set_seed(analogRead(3));
	random16_add_entropy(random());

	setupPatterns();
	doPeriodicUpdates();
}