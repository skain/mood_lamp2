#ifndef patternParms_h
#define patternParms_h
#include <FastLED.h>

typedef struct
{
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
	uint8_t g_colorIndex;
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
} patternParms;

#endif