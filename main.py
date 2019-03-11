import board
# import signal_processing
import pixel_sequences
import mood_lamp

import gc
gc.collect()

pixel_pin = board.D4
num_pixels = 49
brightness = 1.0


def run():
	ml = create_mood_lamp()
	with ml.create_pixels() as pixels:
		ml.run(pixels)

def create_mood_lamp():
	pixel_pin = board.D4
	num_pixels = 49
	brightness = 1.0
	return mood_lamp.MoodLamp(pixel_pin, num_pixels, brightness)

# ml = create_mood_lamp()
# with ml.create_pixels() as pixels:
# 	pixel_sequences.time_it(pixel_sequences.square_test, pixels)
run()
