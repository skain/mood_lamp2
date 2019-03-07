# CircuitPython demo - NeoPixel
import time
import math
import board
import neopixel
import signal_processing as sp


RED = (255, 0, 0)
YELLOW = (255, 150, 0)
GREEN = (0, 255, 0)
CYAN = (0, 255, 255)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)


def wheel(pos):
	# Input a value 0 to 255 to get a color value.
	# The colours are a transition r - g - b - back to r.
	if pos < 0 or pos > 255:
		return (0, 0, 0)
	if pos < 85:
		return (255 - pos * 3, pos * 3, 0)
	if pos < 170:
		pos -= 85
		return (0, 255 - pos * 3, pos * 3)
	pos -= 170
	return (pos * 3, 0, 255 - pos * 3)


def get_pixel_count(pixels):
	return len(pixels)


def color_chase(pixels, color, wait):
	num_pixels = get_pixel_count(pixels)
	for i in range(num_pixels):
		pixels[i] = color
		time.sleep(wait)
		pixels.show()
	# time.sleep(0.5)


def rainbow_cycle(pixels, wait):
	num_pixels = get_pixel_count(pixels)

	for j in range(255):
		for i in range(num_pixels):
			rc_index = (i * 256 // num_pixels) + j
			pixels[i] = wheel(rc_index & 255)
		pixels.show()
		time.sleep(wait)


def show_one_color(pixels, color, wait):
	pixels.fill(color)
	pixels.show()
	time.sleep(wait)


def red_green_blue_sin(pixels, frame_wait=0.0):
	clock = sp.FrameClockSignal()
	frequency = sp.StaticSignal(0.1)
	red_wave = sp.TransformedSignal(
		sp.SineWaveSignal(time=clock, frequency=frequency),
		0,255,discrete=True)
	green_wave = sp.TransformedSignal(
		sp.SineWaveSignal(time=clock, frequency=frequency, phase=math.pi),
		0, 255,  discrete=True)

	blue_wave = sp.TransformedSignal(
		sp.SineWaveSignal(time=clock, frequency=frequency, phase=math.pi/4),
		0, 255,  discrete=True)

	for i in range(255):
		clock.update()
		color = (red_wave(), green_wave(), blue_wave())
		pixels.fill(color)
		pixels.show()
		time.sleep(frame_wait)

def possig_test(pixels, frame_wait=0.0):
	clock = sp.FrameClockSignal()
	frequency = sp.StaticSignal(1.0)
	phase_position = sp.StripPositionPhaseSignal(len(pixels))

	red_wave = sp.TransformedSignal(
		sp.SineWaveSignal(time=clock, frequency=frequency, phase=phase_position),
		0, 255, discrete=True)

	for l in range(100):
		clock.update()
		for i in range(len(pixels)):
			phase_position.update(i)
			red_value = red_wave()
			color = (red_value, 0, 0)
			pixels[i] = color

		pixels.show()
		time.sleep(frame_wait)

def color_sin_test(pixels, frame_wait=0.0):
	clock = sp.FrameClockSignal()
	frequency = sp.StaticSignal(0.02)
	phase_position = sp.StaticSignal(0.0)

	color_wave = sp.TransformedSignal(
		sp.SineWaveSignal(time=clock, frequency=frequency, phase=phase_position),
		0, 255, discrete=True)

	for l in range(255):
		clock.update()
		color_wheel_index = color_wave()
		color = wheel(color_wheel_index)
		pixels.fill(color)

		pixels.show()
		time.sleep(frame_wait)

def color_sin_pos_test(pixels, frame_wait=0.0):
	clock = sp.FrameClockSignal()
	frequency = sp.StaticSignal(0.02)
	phase_position = sp.StripPositionPhaseSignal(len(pixels))

	color_wave = sp.TransformedSignal(
		sp.SineWaveSignal(time=clock, frequency=frequency, phase=phase_position),
		0, 255, discrete=True)

	for l in range(255):
		clock.update()
		for i in range(len(pixels)):
			phase_position.update(i)
			color_wheel_index = color_wave()
			color = wheel(color_wheel_index)
			pixels[i] = color

		pixels.show()
		time.sleep(frame_wait)



