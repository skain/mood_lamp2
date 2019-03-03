import neopixel
import time
import pixel_sequences



class MoodLamp():
    def __init__(self, pixel_pin, num_pixels, brightness):
        self.pixel_pin = pixel_pin
        self.num_pixels = num_pixels
        self.brightness = brightness
        self.pixels = None

    def create_pixels(self):
        return neopixel.NeoPixel(self.pixel_pin, self.num_pixels, brightness=self.brightness, auto_write=False)

    def run(self, pixels):
        while(True):
        # for i in range(10):
            # self._run_one_cycle(pixels)
            # self.run_debug(pixels)
            # pixel_sequences.red_green_sin(pixels)
            pixel_sequences.possig_test(pixels)

    def _run_one_cycle(self, pixels):
        self.show_rgb(pixels, 1)

        self.show_color_chase(pixels, 0.1)

        self.show_rainbow_cycle(pixels, 0.1)

    def show_rainbow_cycle(self, pixels, wait):
        pixel_sequences.rainbow_cycle(pixels, wait)

    def show_color_chase(self, pixels, wait):
        pixel_sequences.color_chase(pixels, pixel_sequences.RED, wait)
        pixel_sequences.color_chase(pixels, pixel_sequences.YELLOW, wait)
        pixel_sequences.color_chase(pixels, pixel_sequences.GREEN, wait)
        pixel_sequences.color_chase(pixels, pixel_sequences.CYAN, wait)
        pixel_sequences.color_chase(pixels, pixel_sequences.BLUE, wait)
        pixel_sequences.color_chase(pixels, pixel_sequences.PURPLE, wait)

    def show_rgb(self, pixels, wait):
        pixel_sequences.show_one_color(pixels, pixel_sequences.RED, wait)
        pixel_sequences.show_one_color(pixels, pixel_sequences.GREEN, wait)
        pixel_sequences.show_one_color(pixels, pixel_sequences.BLUE, wait)

