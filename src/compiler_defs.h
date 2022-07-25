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