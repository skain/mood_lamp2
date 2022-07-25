#include "Arduino.h"
#include "helpers.h"
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

#define PHASE_STRATEGY_ROWS 0		 // Phase calculated by row
#define PHASE_STRATEGY_COLUMNS 1	 // Phase calculated by column
#define PHASE_STRATEGY_STRIP_INDEX 2 // Phase calculated directly by pixel location on strip
#define PHASE_STRATEGY_RANDOM 3		 // Random phases calculated with predictable seed

#define WAVE_STRATEGY_SIN 0		 // basic sin
#define WAVE_STRATEGY_SAW 1		 // basic sawtooth
#define WAVE_STRATEGY_TRIANGLE 2 // basic triangle (linear slopes)
#define WAVE_STRATEGY_CUBIC 3	 // basic cubic (spends more time at limits than sine)
#define WAVE_STRATEGY_SQUARE 4	 // basic square (on or off)

#define COLOR_STRATEGY_RGB 0			// apply 3 different waves to R, G and B
#define COLOR_STRATEGY_HSV 1			// apply 3 different waves to H, S and V
#define COLOR_STRATEGY_PALETTE 2		// apply 2 of the three waves to palette H and V (no S with palettes...)
#define COLOR_STRATEGY_OFFSET_PALETTE 3 // use an offset to fill from palette and apply one wave to V (H is calculated directly off of pixel index + offset)

#define NUM_BIFURCATION_STRATEGIES 3  // count of bifurcation strats
#define BIFURCATION_STRATEGY_PIXELS 0 // apply a different pattern to pixels based on the modulo of the pixel index
#define BIFURCATION_STRATEGY_ROWS 1	  // apply a different pattern to pixels based on rows
#define BIFURCATION_STRATEGY_COLS 2	  // apply a different pattern to pixels base on columns
#define BIFURCATION_MODE_MODULO 0	  // apply a different pattern to pixels based on the modulo of the pixel index, row, or column
#define BIFURCATION_MODE_BELOW 1	  // apply a different pattern to pixels that have an index, row or column below a specific value
#define BIFURCATION_MODE_ALTERNATE 2  // apply a different pattern to every other pixel (and handle even count rows correctly)

CRGB leds[NUM_LEDS];

// Shared Global Variables
bool g_patternsReset = true;
unsigned long g_startTime;
uint8_t g_phase1, g_phase2, g_phase3;
uint8_t g_bpm1, g_bpm2, g_bpm3;
uint8_t g_bpmMax1, g_bpmMax2, g_bpmMax3;
uint8_t g_pulseWidth1, g_pulseWidth2, g_pulseWidth3;
uint8_t g_patternIndex;
uint8_t g_hue1, g_paletteOffset;
uint8_t g_hueSteps1;
uint8_t g_sat1;
uint8_t g_glitterChance, g_glitterPercent;
bool g_addGlitter;
uint8_t g_everyNSecs;
uint16_t g_everyNMillis1, g_everyNMillis2;
CHSVPalette16 g_palette1;
TBlendType g_paletteBlending1;
static uint8_t g_colorIndex;
bool g_reverse1, g_reverse2, g_reverse3;
bool g_animateBPM1, g_animateBPM2, g_animateBPM3;
uint8_t g_BPMAnimationBPM1, g_BPMAnimationBPM2, g_BPMAnimationBPM3;
uint8_t g_colorStrategy, g_bifurcateColorStrategy1;
uint8_t g_phaseStrategy1, g_phaseStrategy2, g_phaseStrategy3;
uint8_t g_waveStrategy1, g_waveStrategy2, g_waveStrategy3;
uint8_t g_demoReelPatternIndex;
uint8_t g_minAmplitude1, g_maxAmplitude1, g_minAmplitude2, g_maxAmplitude2, g_minAmplitude3, g_maxAmplitude3;
bool g_bifurcatePatterns, g_bifurcateOscillation;
uint8_t g_bifurcatePatternsBy, g_bifurcationStrategy, g_bifurcationMode;
uint16_t g_predictableRandomSeed;
unsigned long g_transitionUntil;

void resetPatternGlobals();

void setBrightnessFromKnob()
{
	uint8_t val = map(analogRead(A0), 0, 1023, 30, 255);
	FastLED.setBrightness(val);
	// EVERY_N_SECONDS(1) { Serial.println(val); }
}

bool checkPowerSwitch()
{
	if (POWER_SWITCH_ENABLED == 1 && digitalRead(POWER_SWITCH_PIN))
	{
		fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
		FastLED.show();
		if (!g_patternsReset)
		{
			resetPatternGlobals();
		}
		FastLED.delay(250);
		return false;
	}

	return true;
}

String colorStrategyToString(uint8_t strategy)
{
	switch (strategy)
	{
		case COLOR_STRATEGY_RGB:
			return "RGB";
			break;
		case COLOR_STRATEGY_HSV:
			return "HSV";
			break;
		case COLOR_STRATEGY_PALETTE:
			return "PALETTE";
			break;
		case COLOR_STRATEGY_OFFSET_PALETTE:
			return "OFFSET_PALETTE";
			break;
	}

	return "UNAVAILABLE";
}

String phaseStrategyToString(uint8_t strategy)
{
	switch (strategy)
	{
		case PHASE_STRATEGY_ROWS:
			return "PHASE_STRATEGY_ROWS";
			break;
		case PHASE_STRATEGY_COLUMNS:
			return "PHASE_STRATEGY_COLUMNS";
			break;
		case PHASE_STRATEGY_STRIP_INDEX:
			return "PHASE_STRATEGY_STRIP_INDEX";
			break;
		case PHASE_STRATEGY_RANDOM:
			return "PHASE_STRATEGY_RANDOM";
			break;
	}

	return "UNAVAILABLE";
}

String waveStrategyToString(uint8_t strategy)
{
	switch (strategy)
	{
	case WAVE_STRATEGY_SIN:
		return "WAVE_STRATEGY_SIN";
		break;
	case WAVE_STRATEGY_SAW:
		return "WAVE_STRATEGY_SAW";
		break;
	case WAVE_STRATEGY_TRIANGLE:
		return "WAVE_STRATEGY_TRIANGLE";
		break;
	case WAVE_STRATEGY_CUBIC:
		return "WAVE_STRATEGY_CUBIC";
		break;
	case WAVE_STRATEGY_SQUARE:
		return "WAVE_STRATEGY_SQUARE";
		break;
	}

	return "UNAVAILABLE";
}

// helpers that require access to globals TODO: fix this

// This function fills the palette with totally random colors.
void setupRandomPalette()
{
	CHSV c1, c2, c3;
	c1 = CHSV(random(), 255, 255);
	if (c1.value < 125)
	{
		c1.value = 125;
	}

	Serial.print("Palette Type: ");
	if (pctToBool(50))
	{
		// complimentary palette
		Serial.println("Complimentary");
		c2 = CHSV(getHarmonicHue(c1.hue, 2, 1), c1.saturation, c1.value);
		if (pctToBool(50))
		{
			g_palette1 = CHSVPalette16(c1, c2);
		}
		else
		{
			g_palette1 = CHSVPalette16(c1, c2, c1);
		}
	}
	else
	{
		Serial.println("Triadic");
		// triadic palette
		c2 = CHSV(getHarmonicHue(c1.hue, 3, 1), c1.saturation, c1.value);
		c3 = CHSV(getHarmonicHue(c1.hue, 3, 2), c1.saturation, c1.value);
		if (pctToBool(50))
		{
			g_palette1 = CHSVPalette16(c2, c1, c3);
		}
		else
		{
			g_palette1 = CHSVPalette16(c2, c1, c3, c2);
		}
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
	Serial.print("g_reverse1: ");
	Serial.println(g_reverse1);
	g_reverse2 = random8(2);
	g_reverse3 = random8(2);
}

uint8_t calculateNextColorStrategy(uint8_t curColorStrategy)
{
	uint8_t nextColorStrategy = curColorStrategy;
	while (nextColorStrategy == curColorStrategy)
	{
		nextColorStrategy = random8(2, 4);
	}
	return nextColorStrategy;
}

void resetPatternGlobals()
{
	Serial.println();
	Serial.println();
	Serial.println("==================");
	random16_set_seed(millis());
	random16_add_entropy(random());

	g_predictableRandomSeed = random16();

	g_patternsReset = true;

	// uint8_t patternWeights[] = {10, 90};
	// uncomment for sinelon
	// g_patternIndex = calculateWeightedRandom(patternWeights, NUM_PATTERNS);
	g_patternIndex = 1;

	g_bpm1 = random8();
	g_bpm2 = random8();
	g_bpm3 = random8();
	g_bpmMax1 = g_bpm1;
	g_bpmMax2 = g_bpm2;
	g_bpmMax3 = g_bpm3;

	g_paletteBlending1 = LINEARBLEND;
	g_colorIndex = 0;

	g_startTime = millis();

	randomizeReverses();
	g_animateBPM1 = random(2);
	g_animateBPM2 = random(2);
	g_animateBPM3 = random(2);
	Serial.print("BPMAnimates: ");
	Serial.print(g_animateBPM1);
	Serial.print(",");
	Serial.print(g_animateBPM2);
	Serial.print(",");
	Serial.println(g_animateBPM3);

	g_BPMAnimationBPM1 = random(2, 10);
	g_BPMAnimationBPM2 = random(2, 10);
	g_BPMAnimationBPM3 = random(2, 10);

	g_phase1 = random8();
	g_phase2 = random8();
	g_phase3 = random8();

	g_addGlitter = pctToBool(15);
	g_glitterChance = 60;
	g_glitterPercent = random8(40, 80);

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
	Serial.print("  -g_phaseStrategy1: ");
	Serial.println(phaseStrategyToString(g_phaseStrategy1));
	g_phaseStrategy2 = random8(0, 4);
	Serial.print("  -g_phaseStrategy2: ");
	Serial.println(phaseStrategyToString(g_phaseStrategy2));
	g_phaseStrategy3 = random8(0, 4);
	Serial.print("  -g_phaseStrategy3: ");
	Serial.println(phaseStrategyToString(g_phaseStrategy3));

	g_waveStrategy1 = random8(0, 5);
	Serial.print("  -g_waveStrategy1: ");
	Serial.println(waveStrategyToString(g_waveStrategy1));
	g_waveStrategy2 = random8(0, 5);
	Serial.print("  -g_waveStrategy2: ");
	Serial.println(waveStrategyToString(g_waveStrategy2));
	g_waveStrategy3 = random8(0, 5);
	Serial.print("  -g_waveStrategy3: ");
	Serial.println(waveStrategyToString(g_waveStrategy3));

	setupRandomPalette();

	g_colorStrategy = random8(2, 4);

	uint8_t minAmpMax = 96;
	uint8_t minAmpSpread = 96;
	g_minAmplitude1 = random8(minAmpMax);
	g_maxAmplitude1 = random8(g_minAmplitude1 + minAmpSpread, 255);
	g_minAmplitude2 = random8(minAmpMax);
	g_maxAmplitude2 = random8(g_minAmplitude2 + minAmpSpread, 255);
	g_minAmplitude3 = random8(minAmpMax);
	g_maxAmplitude3 = random8(g_minAmplitude3 + minAmpSpread, 255);

	g_bifurcatePatterns = pctToBool(70);
	g_bifurcatePatterns = false; // comment out to enable bifurcation
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
	scale = 1;
	columnGlitchFactor = rowGlitchFactor = pixelGlitchFactor = 0;
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
		phase = map(random8(NUM_LEDS), 0, NUM_LEDS - 1, 0, 255);
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
		case BIFURCATION_STRATEGY_COLS:
		case BIFURCATION_STRATEGY_ROWS:
			if (g_bifurcationStrategy == BIFURCATION_STRATEGY_COLS)
			{
				calculatedIndex = calculatePixelColumn(pixelIndex, NUM_COLUMNS);
				maxAmplitude = NUM_COLUMNS;
				bifurcateVal = bifurcateVal % NUM_COLUMNS;
			}
			else
			{
				calculatedIndex = calculatePixelRow(pixelIndex, NUM_COLUMNS);
				maxAmplitude = NUM_ROWS;
				bifurcateVal = bifurcateVal % NUM_ROWS;
			}
			switch (g_bifurcationMode)
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
		color = ColorFromPalette(g_palette1, val1, val2, g_paletteBlending1);
		color.saturation = constrainSaturation(val3);
		leds[pixelIndex] = color;
		break;
	case COLOR_STRATEGY_OFFSET_PALETTE:
		uint8_t colorIndex = g_hue1 + (g_paletteOffset * pixelIndex);
		color = ColorFromPalette(g_palette1, colorIndex, val1, g_paletteBlending1);
		color.saturation = constrainSaturation(val2);
		leds[pixelIndex] = color;
		EVERY_N_MILLISECONDS(g_everyNMillis2) { g_hue1 += g_hueSteps1; }
		break;
	}
}

void animateBPMs()
{
	if (g_animateBPM1)
	{
		g_bpm1 = map(beatsin8(g_BPMAnimationBPM1), 0, 255, 2, g_bpmMax1);
	}

	if (g_animateBPM2)
	{
		g_bpm2 = map(beatsin8(g_BPMAnimationBPM2), 0, 255, 2, g_bpmMax2);
	}

	if (g_animateBPM3)
	{
		g_bpm3 = map(beatsin8(g_BPMAnimationBPM3), 0, 255, 2, g_bpmMax3);
	}
}

// sequences
void fullThreeWaveStrategy()
{
	animateBPMs();
	if (g_patternsReset)
	{
		// Serial.println("fullThreeWaveStrategy");
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

		if (g_bifurcatePatterns)
		{
			Serial.print("  -Bifurcation Strategy: ");
			Serial.print(g_bifurcationStrategy);
			Serial.print("-");
			switch (g_bifurcationStrategy)
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
			Serial.print(g_bifurcationMode);
			Serial.print("-");
			switch (g_bifurcationMode)
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
			Serial.println(g_bifurcatePatternsBy);
			Serial.print("    -Bifurcate Color Strategy: ");
			Serial.print(g_bifurcateColorStrategy1);
			Serial.print("-");
			Serial.println(colorStrategyToString(g_bifurcateColorStrategy1));
			if (g_bifurcateOscillation)
			{
				Serial.println("     -Oscillating");
			}
		}

		g_patternsReset = false;
	}

	uint8_t phase1, phase2, phase3;
	uint8_t val1, val2, val3;
	bool reverse = g_reverse1 && beatsquare8(g_bpm1 / 8) > 0;
	random16_set_seed(g_predictableRandomSeed); // The randomizer needs to be re-set each time through the loop in order for the 'random' numbers to be the same each time through.

	for (uint16_t i = 0; i < NUM_LEDS; i++)
	{
		phase1 = executePixelPhaseStrategy(i, g_phaseStrategy1, 1, reverse, 0, 0, 0);
		phase2 = executePixelPhaseStrategy(i, g_phaseStrategy2, 1, reverse, 0, 0, 0);
		phase3 = executePixelPhaseStrategy(i, g_phaseStrategy3, 1, reverse, 0, 0, 0);
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
}

void loop()
{
	// g_patternIndex = 1;
	// g_bifurcatePatterns = true;
	// g_colorStrategy = 3;

	if (!checkPowerSwitch())
	{
		return;
	}

	// Call the current pattern function once, updating the 'leds' array
	// patterns[g_patternIndex]();
	if (g_transitionUntil > millis())
	{
		fadeToBlackBy(leds, NUM_LEDS, 65);
	}
	else
	{
		g_transitionUntil = 0;
		fullThreeWaveStrategy();
	}

	// send the 'leds' array out to the actual LED strip
	FastLED.show();
	// insert a delay to keep the framerate modest
	FastLED.delay(1000 / FRAMES_PER_SECOND);

	// do some periodic updates
	EVERY_N_SECONDS(SECONDS_TO_SHOW) 
	{
		g_transitionUntil = millis() + 500;
		resetPatternGlobals();
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

	resetPatternGlobals();
}