#include "pattern_parms.h"
#include "config.h"
#include "helpers.h"
#include "strategy_utils.h"
#include "color_utils.h"
#include "compiler_defs.h"

void randomizeReverses(patternParms *p_parms)
{
	p_parms->g_reverse1 = random8(2);
	// Serial.print("p_parms->g_reverse1: ");
	Serial.println(p_parms->g_reverse1);
	p_parms->g_reverse2 = random8(2);
	p_parms->g_reverse3 = random8(2);
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