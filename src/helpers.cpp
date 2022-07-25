#include "Arduino.h"
#include <FastLED.h>

float haveSecsElapsed(uint16_t secs, unsigned long startTime)
{
	unsigned long now = millis();
	if ((now - startTime) > (secs * 1000))
	{
		return true;
	}

	return false;
}

float phaseFromPixelIndex(uint16_t pixelIndex, uint16_t numPixels, float scale, bool reversePattern)
{
	uint16_t fromMin = 0;
	uint16_t fromMax = numPixels * scale;
	if (reversePattern)
	{
		fromMin = fromMax;
		fromMax = 0;
	}
	int phase = map(pixelIndex, fromMin, fromMax, (long int)0, (long int)255);
	return phase;
}

uint16_t calculatePixelRow(uint16_t pixelIndex, uint16_t pixelsPerRow)
{
	return pixelIndex / pixelsPerRow;
}

uint16_t calculatePixelColumn(uint16_t pixelIndex, uint16_t numColumns)
{
	return pixelIndex % numColumns;
}

int phaseFromPixelIndexWithRowOffset(uint16_t pixelIndex, uint16_t numPixels, uint16_t pixelsPerRow, fract8 rowOffsetPercent, float scale, bool reversePattern)
{
	uint16_t fromMin = 0;
	uint16_t pixelsAtScale = numPixels * scale;
	uint16_t fromMax = pixelsAtScale;
	uint16_t rowIndex = calculatePixelRow(pixelIndex, pixelsPerRow);
	rowOffsetPercent = 255 / pixelsPerRow;
	int singleOffsetAmount = (255 * rowOffsetPercent) / 100;
	int offset = singleOffsetAmount * (rowIndex + 1);
	if (reversePattern)
	{
		fromMin = pixelsAtScale;
		fromMax = 0;
		offset = offset * -1;
	}
	int phase = map(pixelIndex + offset, fromMin + offset, fromMax + offset, (long int)0, (long int)255);
	// phase += offset;

	EVERY_N_MILLIS(500)
	{
		// Serial.print("pixelIndex: ");
		// Serial.println(pixelIndex);

		// Serial.print("pixelsAtScale: ");
		// Serial.println(pixelsAtScale);

		// Serial.print("fromMin: ");
		// Serial.println(fromMin);

		// Serial.print("fromMax: ");
		// Serial.println(fromMax);

		// Serial.print("rowIndex: ");
		// Serial.println(rowIndex);

		Serial.print("singleOffsetAmount: ");
		Serial.println(singleOffsetAmount);

		Serial.print("rowOffsetPercent: ");
		Serial.println(rowOffsetPercent);

		Serial.print("offset: ");
		Serial.println(offset);

		Serial.print("phase: ");
		Serial.println(phase);
	}
	return phase;
}

int phaseFromOddEvenIndex(uint16_t pixelIndex)
{
	if (pixelIndex % 2 == 0)
	{
		return 0;
	}
	else
	{
		return 255 / 2;
	}
}

float phaseFromRowIndex(uint16_t pixelIndex, uint16_t pixelsPerRow, uint16_t numRows, float scale, bool reversePattern)
{
	if (pixelIndex == 0)
	{
		return 0;
	}
	uint16_t rowIndex = calculatePixelRow(pixelIndex, pixelsPerRow);
	uint16_t fromMin = 0;
	uint16_t fromMax = (numRows - 1) * scale;
	if (reversePattern)
	{
		fromMin = fromMax;
		fromMax = 0;
	}
	return map(rowIndex, fromMin, fromMax, (long int)0, (long int)255);
}

float phaseFromColumnIndex(uint16_t pixelIndex, uint16_t numCols, float scale, bool reversePattern)
{
	if (pixelIndex == 0)
	{
		return 0;
	}
	uint16_t colIndex = calculatePixelColumn(pixelIndex, numCols);
	//   uint16_t colIndex = pixelIndex % numCols;
	uint16_t fromMin = 0;
	uint16_t fromMax = (numCols - 1) * scale;
	if (reversePattern)
	{
		fromMin = fromMax;
		fromMax = 0;
	}
	return map(colIndex, fromMin, fromMax, (long int)0, (long int)255);
}

float getRandomFloat(float min, float max)
{
	// this is not the greatest implementation but works in the situations I've got right now
	int normMin = min * 1000;
	int normMax = max * 1000;
	long r = random(normMin, normMax);
	float freq = r / 1000.0f;
	return freq;
}

float getRandomPhase()
{
	float r = random8(0, 4);
	if (r == 0)
	{
		return 0.0f;
	}
	return PI / r;
}

uint16_t beatsquare8(accum88 beatsPerMinute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timeBase = 0, uint8_t phaseOffset = 0, uint8_t pulseWidth = 128)
{
	uint8_t beat = beat8(beatsPerMinute, timeBase);
	uint8_t beatSquare = squarewave8(beat + phaseOffset);
	uint8_t rangewidth = highest - lowest;
	uint8_t scaledbeat = scale8(beatSquare, rangewidth);
	uint8_t result = lowest + scaledbeat;
	return result;
}

uint8_t beatsaw8(accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timebase = 0, uint8_t phase_offset = 0)
{
	uint8_t beat = beat8(beats_per_minute, timebase);
	uint8_t beatsaw = beat + phase_offset;
	uint8_t rangewidth = highest - lowest;
	uint8_t scaledbeat = scale8(beatsaw, rangewidth);
	uint8_t result = lowest + scaledbeat;
	return result;
}

uint16_t beattriwave8(accum88 beatsPerMinute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timeBase = 0, uint8_t phaseOffset = 0)
{
	uint8_t beat = beat8(beatsPerMinute, timeBase);
	uint8_t beatSquare = triwave8(beat + phaseOffset);
	uint8_t rangewidth = highest - lowest;
	uint8_t scaledbeat = scale8(beatSquare, rangewidth);
	uint8_t result = lowest + scaledbeat;
	return result;
}

uint16_t beatcubicwave8(accum88 beatsPerMinute, uint8_t lowest = 0, uint8_t highest = 255, uint32_t timeBase = 0, uint8_t phaseOffset = 0)
{
	uint8_t beat = beat8(beatsPerMinute, timeBase);
	uint8_t beatSquare = cubicwave8(beat + phaseOffset);
	uint8_t rangewidth = highest - lowest;
	uint8_t scaledbeat = scale8(beatSquare, rangewidth);
	uint8_t result = lowest + scaledbeat;
	return result;
}

bool pctToBool(fract8 chance)
{
	// rolls a yes/no dice with the specified integer percent of being yes
	return random8(1, 101) < chance;
}

CHSV getRandomColor()
{
	return CHSV(random8(), random8(240, 255), 255);
}

uint8_t calculateWeightedRandom(uint8_t weights[], uint8_t numWeights)
{
	// pass in a list of probabilities (1-99%) and this function
	// generates a random number and returns the index of the weight
	// passed in that's hit

	uint8_t sumOfWeights = 0;
	for (uint8_t i = 0; i < numWeights; i++)
	{
		sumOfWeights += weights[i];
	}

	uint8_t roll = random8(sumOfWeights);

	for (uint8_t i = 0; i < numWeights; i++)
	{
		if (roll < weights[i])
		{
			return i;
		}

		roll -= weights[i];
	}
	return numWeights + 1;
}