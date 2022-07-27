#ifndef color_utils_h
#define color_utils_h
#include <FastLED.h>
#include "pattern_parms.h"

// splits the color wheel into equal segments
// harmonicDivision is how many segments to divide the color wheel into
// harmonicIndex is which segement you want returned (zero-based where zero returns the hue you passed in)
int getHarmonicHue(int origHue, int harmonicDivision, int harmonicIndex);
void setupRandomPalette(patternParms *p_parms);
#endif