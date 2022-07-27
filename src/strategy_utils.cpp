#include <FastLED.h>
#include "compiler_defs.h"
#include "pattern_parms.h"
#include "helpers.h"
#include "pattern_parms_utils.h"

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

uint8_t calculateNextColorStrategy(uint8_t curColorStrategy)
{
	uint8_t nextColorStrategy = curColorStrategy;
	while (nextColorStrategy == curColorStrategy)
	{
		nextColorStrategy = random8(2, 4);
	}
	return nextColorStrategy;
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

uint8_t executePixelPhaseStrategy(uint8_t num_columns, uint8_t num_rows, uint8_t num_leds, uint16_t pixelIndex, uint8_t phaseStrategy, float scale, bool reversePattern,
								  uint8_t columnGlitchFactor, uint8_t rowGlitchFactor, uint8_t pixelGlitchFactor)
{
	uint8_t phase = 0;
	scale = 1;
	columnGlitchFactor = rowGlitchFactor = pixelGlitchFactor = 0;
	switch (phaseStrategy)
	{
	case PHASE_STRATEGY_ROWS:
		phase = phaseFromRowIndex(pixelIndex, num_columns - columnGlitchFactor, num_rows - rowGlitchFactor, scale, reversePattern);
		break;
	case PHASE_STRATEGY_COLUMNS:
		phase = phaseFromColumnIndex(pixelIndex, num_columns - columnGlitchFactor, scale, reversePattern);
		break;
	case PHASE_STRATEGY_STRIP_INDEX:
		phase = phaseFromPixelIndex(pixelIndex, num_leds - pixelGlitchFactor, scale, reversePattern);
		break;
	case PHASE_STRATEGY_RANDOM:
		// NOTE: g_predictableRandomSeed must be set OUTSIDE the loop for this strategy to work!
		phase = map(random8(num_leds), 0, num_leds - 1, 0, 255);
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

uint8_t executeBifurcationStrategy(uint8_t num_leds, uint8_t num_columns, uint8_t num_rows, uint8_t pixelIndex, patternParms *p_parms)
{
	uint8_t curColorStrategy, bifurcateVal, calculatedIndex, maxAmplitude;
	bool isBifurcated = false;

	curColorStrategy = p_parms->g_colorStrategy;
	bifurcateVal = p_parms->g_bifurcatePatternsBy;

	if (p_parms->g_bifurcatePatterns)
	{
		switch (p_parms->g_bifurcationStrategy)
		{
		case BIFURCATION_STRATEGY_PIXELS:
			switch (p_parms->g_bifurcationMode)
			{
			case BIFURCATION_MODE_MODULO:
				if (p_parms->g_bifurcateOscillation)
				{
					bifurcateVal = executeWaveStrategy(p_parms->g_waveStrategy1, p_parms->g_bpm1 / 6, p_parms->g_startTime, 0, 0, p_parms->g_bifurcatePatternsBy, p_parms->g_pulseWidth1);
				}

				if (pixelIndex % bifurcateVal == 0)
				{
					isBifurcated = true;
				}
				break;
			case BIFURCATION_MODE_BELOW:
				if (p_parms->g_bifurcateOscillation)
				{
					bifurcateVal = executeWaveStrategy(p_parms->g_waveStrategy1, p_parms->g_bpm1 / 2, p_parms->g_startTime, 0, 0, num_leds, p_parms->g_pulseWidth1);
				}

				if (pixelIndex < bifurcateVal)
				{
					isBifurcated = true;
				}
				break;
			case BIFURCATION_MODE_ALTERNATE:
				if (p_parms->g_bifurcateOscillation)
				{
					bifurcateVal = beatsquare8(p_parms->g_bpm1 / 6, 0, 1, p_parms->g_startTime, 0);
				}

				calculatedIndex = pixelIndex;
				if (num_columns % 2 == 0 && calculatePixelRow(pixelIndex, num_columns) % 2 != 0)
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
			if (p_parms->g_bifurcationStrategy == BIFURCATION_STRATEGY_COLS)
			{
				calculatedIndex = calculatePixelColumn(pixelIndex, num_columns);
				maxAmplitude = num_columns;
				bifurcateVal = bifurcateVal % num_columns;
			}
			else
			{
				calculatedIndex = calculatePixelRow(pixelIndex, num_columns);
				maxAmplitude = num_rows;
				bifurcateVal = bifurcateVal % num_rows;
			}
			switch (p_parms->g_bifurcationMode)
			{
			case BIFURCATION_MODE_MODULO:
				if (p_parms->g_bifurcateOscillation)
				{
					bifurcateVal = executeWaveStrategy(p_parms->g_waveStrategy1, p_parms->g_bpm1 / 6, p_parms->g_startTime, 0, 0, p_parms->g_bifurcatePatternsBy, p_parms->g_pulseWidth1);
				}

				if (calculatedIndex % bifurcateVal == 0)
				{
					isBifurcated = true;
				}
				break;
			case BIFURCATION_MODE_BELOW:
				if (p_parms->g_bifurcateOscillation)
				{
					bifurcateVal = executeWaveStrategy(p_parms->g_waveStrategy1, p_parms->g_bpm1 / 3, p_parms->g_startTime, 0, 0, maxAmplitude, p_parms->g_pulseWidth1);
				}

				if (calculatedIndex < bifurcateVal)
				{
					isBifurcated = true;
				}
				break;
			case BIFURCATION_MODE_ALTERNATE:
				if (p_parms->g_bifurcateOscillation)
				{
					bifurcateVal = beatsquare8(p_parms->g_bpm1 / 6, 0, 1, p_parms->g_startTime, 0);
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
			curColorStrategy = p_parms->g_bifurcateColorStrategy1;
		}
	}

	return curColorStrategy;
}

int constrainSaturation(int sat)
{
	return map(sat, 0, 255, 50, 255);
}

void executeColorStrategy(CRGB *leds, uint8_t num_leds, uint8_t num_columns, uint8_t num_rows, patternParms *p_parms, uint8_t pixelIndex, uint8_t val1, uint8_t val2, uint8_t val3)
{
	uint8_t curColorStrategy = executeBifurcationStrategy(num_leds, num_columns, num_rows, pixelIndex, p_parms);
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

void addGlitter(CRGB *leds, uint8_t num_leds, uint8_t chanceOfGlitter)
{
	if (pctToBool(chanceOfGlitter))
	{
		leds[random16(num_leds)] += CRGB::White;
	}
}

void fullThreeWaveStrategy(CRGB *leds, uint8_t num_leds, uint8_t num_columns, uint8_t num_rows, patternParms *p_parms)
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

	for (uint16_t i = 0; i < num_leds; i++)
	{
		phase1 = executePixelPhaseStrategy(num_columns, num_rows, num_leds, i, p_parms->g_phaseStrategy1, 1, reverse, 0, 0, 0);
		phase2 = executePixelPhaseStrategy(num_columns, num_rows, num_leds, i, p_parms->g_phaseStrategy2, 1, reverse, 0, 0, 0);
		phase3 = executePixelPhaseStrategy(num_columns, num_rows, num_leds, i, p_parms->g_phaseStrategy3, 1, reverse, 0, 0, 0);
		val1 = executeWaveStrategy(p_parms->g_waveStrategy1, p_parms->g_bpm1, p_parms->g_startTime, phase1, p_parms->g_minAmplitude1, p_parms->g_maxAmplitude1, p_parms->g_pulseWidth1);
		val2 = executeWaveStrategy(p_parms->g_waveStrategy2, p_parms->g_bpm2, p_parms->g_startTime, phase2, p_parms->g_minAmplitude2, p_parms->g_maxAmplitude2, p_parms->g_pulseWidth2);
		val3 = executeWaveStrategy(p_parms->g_waveStrategy3, p_parms->g_bpm3, p_parms->g_startTime, phase3, p_parms->g_minAmplitude3, p_parms->g_maxAmplitude3, p_parms->g_pulseWidth3);

		executeColorStrategy(leds, num_leds, num_columns, num_rows, p_parms, i, val1, val2, val3);
	}
	random16_set_seed(millis()); // Re-randomizing the random number seed for other routines.

	if (p_parms->g_addGlitter)
	{
		addGlitter(leds, num_leds, p_parms->g_glitterChance);
	}
}