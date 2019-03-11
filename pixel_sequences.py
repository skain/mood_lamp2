import time
import math
import neopixel


RED = (255, 0, 0)
YELLOW = (255, 150, 0)
GREEN = (0, 255, 0)
CYAN = (0, 255, 255)
BLUE = (0, 0, 255)
PURPLE = (180, 0, 255)
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# helpers
def compute_sin_wave(sin_func=math.sin, pi_val=math.pi, frequency=1.0, phase=0.0, time=0.0, amplitude=1.0):
	return amplitude * sin_func(2* pi_val * frequency * time + phase)

def compute_square_wave(sin_func=math.sin, pi_val=math.pi, frequency=1.0, phase=0.0, time=0.0, amplitude=1.0, duty_cycle=0.5):
	threshold = 1 - (duty_cycle * 2)
	sin_val = compute_sin_wave(sin_func=sin_func, pi_val=pi_val, frequency=frequency, phase=phase, time=time, amplitude=amplitude)
	return 1 if sin_val > threshold else 0

def interpolate(val, in_min, in_max, out_min, out_max):
	return out_min + (val-in_min) * ((out_max-out_min)/(in_max-in_min))

def sin_to_255(interpolate_func=interpolate, sin_func=math.sin, pi_val=math.pi, frequency=1.0, phase=0.0, time=0.0, amplitude=1.0):
	sin_val = compute_sin_wave(sin_func=sin_func, pi_val=pi_val, frequency=frequency, phase=phase, time=time, amplitude=amplitude)
	return int(interpolate(sin_val, -1, 1, 0, 255))

def square_to_255(interpolate_func=interpolate, sin_func=math.sin, pi_val=math.pi, frequency=1.0, phase=0.0, time=0.0, amplitude=1.0, duty_cycle=0.5):
	square_val = compute_square_wave(sin_func=sin_func, pi_val=pi_val, frequency=frequency, 
									 phase=phase, time=time, amplitude=amplitude, duty_cycle=duty_cycle)
	print(square_val)
	return int(interpolate(square_val, 0, 1, 0, 255))

def phase_from_pixel_index(pixel_index, num_pixels, interpolate_func=interpolate, pi_val=math.pi):
    phase = interpolate_func(pixel_index, 0, num_pixels, 0, pi_val)
    return phase

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


# sequences
# def color_chase(pixels, color, wait):
# 	num_pixels = len(pixels)
# 	for i in range(num_pixels):
# 		pixels[i] = color
# 		time.sleep(wait)
# 		pixels.show()
# 	time.sleep(wait)


# def rainbow_cycle(pixels, wait):
# 	num_pixels = len(pixels)

# 	for j in range(255):
# 		for i in range(num_pixels):
# 			rc_index = (i * 256 // num_pixels) + j
# 			pixels[i] = wheel(rc_index & 255)
# 		pixels.show()
# 		time.sleep(wait)


# def show_one_color(pixels, color, wait):
# 	pixels.fill(color)
# 	pixels.show()
# 	time.sleep(wait)

def red_green_blue_sin(pixels, frame_wait=0.0):
	#35.625 monotones
	pi = math.pi
	green_phase = pi
	blue_phase = pi/4
	red_f = 0.2
	green_f = 0.2
	blue_f = 0.2
	r = range(255)
	for i in r:
		t = time.monotonic()
		red_sin = sin_to_255(pi_val=pi, frequency=red_f, time=t)
		green_sin = sin_to_255(pi_val=pi, frequency=green_f, phase=green_phase, time=t)
		blue_sin = sin_to_255(pi_val=pi, frequency=blue_f, phase=blue_phase, time=t)
		pixels.fill((red_sin, green_sin, blue_sin))
		pixels.show()
		time.sleep(frame_wait)

# def possig_test(pixels, frame_wait=0.0):
# 	#48.625 monotones
# 	freq = 0.04
# 	num_pix = len(pixels)
# 	pix_r = range(num_pix)
# 	pi = math.pi
# 	sin = math.sin
# 	r = range(255)
# 	for i in r:
# 		t = time.monotonic()
# 		for i in pix_r:
# 			phase = interpolate(i, 0, num_pix, 0, pi)
# 			cur_sin = compute_sin_wave(sin, pi, freq, phase, t)
# 			red_val = int(interpolate(cur_sin, -1, 1, 0, 255))
# 			color = (red_val, 0, 0)
# 			pixels[i] = color

# 		pixels.show()
# 		time.sleep(frame_wait)

def possig_test(pixels, frame_wait=0.0):
	#51.75 monotones
	freq = 0.04
	num_pix = len(pixels)
	pix_r = range(num_pix)
	pi = math.pi
	r = range(255)
	for i in r:
		t = time.monotonic()
		for i in pix_r:
			phase = phase_from_pixel_index(i, num_pix, pi_val=pi)
			cur_sin = sin_to_255(frequency=freq, phase=phase, time=t)
			color = (cur_sin, 0, 0)
			pixels[i] = color

		pixels.show()
		time.sleep(frame_wait)

# def possig_3_wave_test(pixels, frame_wait=0.0):
# 	#39.375 monotones
# 	red_freq = 0.2
# 	green_freq = 0.1
# 	blue_freq = 0.3
# 	num_pix = len(pixels)
# 	pix_r = range(num_pix)
# 	pi = math.pi
# 	sin = math.sin
# 	r = range(100)
# 	for i in r:
# 		for i in pix_r:
# 			t = time.monotonic()
# 			phase = phase_from_pixel_index(pi, i, num_pix)
# 			red_sin = compute_sin_wave(sin, pi, red_freq, phase, t)
# 			red_val = int(interpolate(red_sin, -1, 1, 0, 255))
# 			green_sin = compute_sin_wave(sin, pi, green_freq, phase, t)
# 			green_val = int(interpolate(green_sin, -1, 1, 0, 255))
# 			blue_sin = compute_sin_wave(sin, pi, blue_freq, phase, t)
# 			blue_val = int(interpolate(blue_sin, -1, 1, 0, 255))
# 			color = (red_val, green_val, blue_val)
# 			pixels[i] = color

# 		pixels.show()
# 		time.sleep(frame_wait)

def possig_3_wave_test(pixels, frame_wait=0.0):
	#42.5 monotones
	red_freq = 0.2
	green_freq = 0.1
	blue_freq = 0.3
	num_pix = len(pixels)
	pix_r = range(num_pix)
	pi = math.pi
	sin = math.sin
	r = range(100)
	for i in r:
		for i in pix_r:
			t = time.monotonic()
			phase = phase_from_pixel_index(i, num_pix, pi_val=pi)
			red_val = sin_to_255(pi_val=pi, frequency=red_freq, phase=phase, time=t)
			green_val = sin_to_255(pi_val=pi, frequency=green_freq, phase=phase, time=t)
			blue_val = sin_to_255(pi_val=pi, frequency=blue_freq, phase=phase, time=t)
			pixels[i] = (red_val, green_val, blue_val)

		pixels.show()
		time.sleep(frame_wait)

def color_sin_test(pixels, frame_wait=0.0):
	freq = 0.2
	pi = math.pi
	r = range(255)
	for i in r:
		t = time.monotonic()
		color_wheel_index = sin_to_255(pi_val=pi, frequency=freq, time=t)
		color = wheel(color_wheel_index)
		pixels.fill(color)

		pixels.show()
		time.sleep(frame_wait)

def color_sin_pos_test(pixels, frame_wait=0.0):
	#49.1875 monotones
	amp = 1.0
	freq = 0.04
	num_pix = len(pixels)
	pix_r = range(num_pix)
	pi = math.pi
	r = range(255)
	for i in r:
		t = time.monotonic()
		for i in pix_r:
			phase = phase_from_pixel_index(i, num_pix, pi_val=pi)
			color_wheel_index = sin_to_255(pi_val=pi, frequency=freq, phase=phase, time=t)
			color = wheel(color_wheel_index)
			pixels[i] = color

		pixels.show()
		time.sleep(frame_wait)

def square_test(pixels, frame_wait=0.0):
	freq = 1.0
	duty_cycle = 0.25
	pi = math.pi

	r = range(255)
	for i in r:
		t = time.monotonic()
		red_val = square_to_255(pi_val=pi, frequency=freq, time=t, duty_cycle=duty_cycle)
		color = (red_val, 0, 0)
		pixels.fill(color)
		pixels.show()
		time.sleep(frame_wait)

def time_it(f, pixels):
	t0 = time.monotonic()
	f(pixels)
	t1 = time.monotonic()
	dt = t1 - t0
	print('{} monotones'.format(dt))