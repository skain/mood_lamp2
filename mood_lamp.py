import neopixel
import time
import pixel_sequences as ps


class MoodLamp():
	def __init__(self, pixel_pin, num_pixels, brightness):
		self.pixel_pin = pixel_pin
		self.num_pixels = num_pixels
		self.brightness = brightness

	def create_pixels(self):
		return neopixel.NeoPixel(self.pixel_pin, self.num_pixels, brightness=self.brightness, auto_write=False)

	def run(self, pixels):
		while(True):
			# print('red_green_blue_sin')
			# ps.red_green_blue_sin(pixels)
			
			print('square_test')
			ps.square_test(pixels)

			# self._run_one_cycle(pixels)

			# print('possig_test')
			# ps.possig_test(pixels)

			# print('color_sin_test')
			# ps.color_sin_test(pixels)

			# print('color_sin_pos_test')
			# ps.color_sin_pos_test(pixels)

	# def _run_one_cycle(self, pixels):
	# 	self.show_rgb(pixels, 0.25)

	# 	self.show_color_chase(pixels, .01)

	# 	self.show_rainbow_cycle(pixels, 0)

	# def show_rainbow_cycle(self, pixels, wait):
	# 	print('rainbow cycle')
	# 	ps.rainbow_cycle(pixels, wait)

	# def show_color_chase(self, pixels, wait):
	# 	print('show color chase')
	# 	ps.color_chase(pixels, ps.RED, wait)
	# 	ps.color_chase(pixels, ps.YELLOW, wait)
	# 	ps.color_chase(pixels, ps.GREEN, wait)
	# 	ps.color_chase(pixels, ps.CYAN, wait)
	# 	ps.color_chase(pixels, ps.BLUE, wait)
	# 	ps.color_chase(pixels, ps.PURPLE, wait)

	# def show_rgb(self, pixels, wait):
	# 	print('show rgb')
	# 	ps.show_one_color(pixels, ps.RED, wait)
	# 	ps.show_one_color(pixels, ps.GREEN, wait)
	# 	ps.show_one_color(pixels, ps.BLUE, wait)

	# def blink(self, pixels, num_blinks, blink_time):
	# 	for i in range(num_blinks):
	# 		pixels.fill(ps.WHITE)
	# 		pixels.show()
	# 		time.sleep(blink_time)

	# 		pixels.fill(ps.BLACK)
	# 		pixels.show()
	# 		time.sleep(blink_time)
