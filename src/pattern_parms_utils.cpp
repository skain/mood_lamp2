#include "pattern_parms.h"

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