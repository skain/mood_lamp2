# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import gc
#import webrepl
#webrepl.start()
gc.collect()

import time
import board
import mood_lamp
from clear_boot import cb

pixel_pin = board.GPIO2
num_pixels = 49
brightness = 1.0

ml = mood_lamp.MoodLamp(pixel_pin, num_pixels, brightness)
time.sleep(1.0)
with ml.create_pixels() as pixels:
    ml.run(pixels)
