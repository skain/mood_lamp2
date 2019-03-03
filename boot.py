# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import gc
#import webrepl
#webrepl.start()
gc.collect()

import board
import mood_lamp

pixel_pin = board.GPIO2
num_pixels = 35
brightness = 1.0

ml = mood_lamp.MoodLamp(pixel_pin, num_pixels, brightness)
with ml.create_pixels() as pixels:
    ml.run(pixels)

