import neopixel
import time
import pixel_sequences



class MoodLamp():
    def __init__(self, pixel_pin, num_pixels, brightness):
        self.pixel_pin = pixel_pin
        self.num_pixels = num_pixels
        self.brightness = brightness

    def create_pixels(self):
        return neopixel.NeoPixel(self.pixel_pin, self.num_pixels, auto_write=False)
        # return neopixel.NeoPixel(self.pixel_pin, self.num_pixels, brightness=self.brightness, auto_write=False)

    def run(self, pixels):
        while(True):
            # self.blink(pixels, 1, 0.1)
            for i in range(len(pixels)):
                pixel_sequences.red_green_sin(pixels)

            # # self.blink(pixels, 2, 0.1)
            # self._run_one_cycle(pixels)

            # # self.blink(pixels, 3, 0.1)
            # for i in range(len(pixels) * 2):
            #     pixel_sequences.possig_test(pixels)

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

    def blink(self, pixels, num_blinks, blink_time):
        for i in range(num_blinks):
            pixels.fill(pixel_sequences.WHITE)
            pixels.show()
            time.sleep(blink_time)

            pixels.fill(pixel_sequences.BLACK)
            pixels.show()
            time.sleep(blink_time)


