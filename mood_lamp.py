import neopixel
import pixel_sequences



class MoodLamp():
    def __init__(self, pixel_pin, num_pixels, brightness):
        self.pixel_pin = pixel_pin
        self.num_pixels = num_pixels
        self.brightness = brightness
        self.pixels = None

    def _create_pixels(self):
        return neopixel.NeoPixel(self.pixel_pin, self.num_pixels, brightness=self.brightness, auto_write=False)

    def run(self):
        with self._create_pixels() as pixels:
            while(True):
                self._run_one_cycle(pixels)

    def _run_one_cycle(self, pixels):
        pixel_sequences.show_one_color(pixels, pixel_sequences.RED, 1)
        pixel_sequences.show_one_color(pixels, pixel_sequences.GREEN, 1)
        pixel_sequences.show_one_color(pixels, pixel_sequences.BLUE, 1)

        pixel_sequences.color_chase(pixels, pixel_sequences.RED, 0.1)
        pixel_sequences.color_chase(pixels, pixel_sequences.YELLOW, 0.1)
        pixel_sequences.color_chase(pixels, pixel_sequences.GREEN, 0.1)
        pixel_sequences.color_chase(pixels, pixel_sequences.CYAN, 0.1)
        pixel_sequences.color_chase(pixels, pixel_sequences.BLUE, 0.1)
        pixel_sequences.color_chase(pixels, pixel_sequences.PURPLE, 0.1)

        pixel_sequences.rainbow_cycle(pixels, 0)