import board
import signal_processing
import pixel_sequences
import mood_lamp

import gc
gc.collect()

pixel_pin = board.D4
num_pixels = 49
brightness = 1.0


def run():
	ml = mood_lamp.MoodLamp(pixel_pin, num_pixels, brightness)
	with ml.create_pixels() as pixels:
		ml.run(pixels)


run()
