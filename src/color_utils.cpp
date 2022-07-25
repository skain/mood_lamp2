#include <FastLED.h>
#include "compiler_defs.h"
#include "pattern_parms.h"
#include "helpers.h"


int constrainSaturation(int sat)
{
	return map(sat, 0, 255, 50, 255);
}

// splits the color wheel into equal segments
// harmonicDivision is how many segments to divide the color wheel into
// harmonicIndex is which segement you want returned (zero-based where zero returns the hue you passed in)
int getHarmonicHue(int origHue, int harmonicDivision, int harmonicIndex)
{
	return (origHue + ((255 / harmonicDivision) * harmonicIndex)) % 255;
}

void setupRandomPalette(patternParms *p_parms)
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
			p_parms->g_palette1 = CHSVPalette16(c1, c2);
		}
		else
		{
			p_parms->g_palette1 = CHSVPalette16(c1, c2, c1);
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
			p_parms->g_palette1 = CHSVPalette16(c2, c1, c3);
		}
		else
		{
			p_parms->g_palette1 = CHSVPalette16(c2, c1, c3, c2);
		}
	}
}


