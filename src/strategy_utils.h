#include <FastLED.h>
#include "pattern_parms.h"

uint8_t calculateNextColorStrategy(uint8_t curColorStrategy);
String phaseStrategyToString(uint8_t strategy);
String waveStrategyToString(uint8_t strategy);
String colorStrategyToString(uint8_t strategy);
uint8_t executePixelPhaseStrategy(uint8_t num_columns, uint8_t num_rows, uint8_t num_leds, uint16_t pixelIndex,
								  uint8_t phaseStrategy, float scale, bool reversePattern,
								  uint8_t columnGlitchFactor, uint8_t rowGlitchFactor, uint8_t pixelGlitchFactor);
uint8_t executeWaveStrategy(uint8_t waveStrategy, uint8_t bpm, unsigned long startTime, uint8_t phase,
							uint8_t minAmplitude, uint8_t maxAmplitude, uint8_t pulseWidth);
uint8_t executeBifurcationStrategy(uint8_t num_leds, uint8_t num_columns, uint8_t num_rows, uint8_t pixelIndex, patternParms *p_parms);