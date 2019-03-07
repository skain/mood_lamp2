import math
import time
# from https://gist.github.com/tdicola/f4324c9ae813410182d5ed00e866c8fb


class SignalSource:
	def __init__(self, source=None):
		self.set_source(source)

	def __call__(self):
		return self._source()

	def set_source(self, source):
		if callable(source):
			self._source = source
		else:
			self._source = lambda: source


class SignalBase(object):
	def __init__(self, value_range=None):
		self.value_range = value_range

	def __call__(self):
		raise NotImplementedError('Signal must have a callable implementation!')

	def transform(self, y0, y1):
		x = self()
		if self.value_range is not None:
			return y0 + (x-self.value_range[0]) * \
						((y1-y0)/(self.value_range[1]-self.value_range[0]))
		else:
			return max(y0, min(y1, x))

	def discrete_transform(self, y0, y1):
		return int(self.transform(y0, y1))

# Signals


class StaticSignal(SignalBase):
	def __init__(self, static_value=0.0):
		self._static_value = static_value

	def __call__(self):
		return self._static_value


class PublicValueSignal(SignalBase):
	def __init__(self, value=0.0, value_range=None):
		super(PublicValueSignal, self).__init__(value_range)
		self.value = value

	def __call__(self):
		return self.value

	def update(self, value):
		'''
		While it is certainly allowed to directly update value here, it's probably better to use this interface
		'''
		self.value = value


class SineWaveSignal(SignalBase):
	def __init__(self, time=0.0, amplitude=1.0, frequency=1.0, phase=0.0):
		value_range = (-amplitude, amplitude)
		super(SineWaveSignal, self).__init__(value_range=value_range)
		self.time = SignalSource(time)
		self.amplitude = SignalSource(amplitude)
		self.frequency = SignalSource(frequency)
		self.phase = SignalSource(phase)

	def __call__(self):
		return self.amplitude() * \
			math.sin(2*math.pi*self.frequency()*self.time() + self.phase())


class FrameClockSignal(SignalBase):
	def __init__(self):
		self._current_s = None
		self.update()

	def _seconds(self):
		return time.monotonic()

	def update(self):
		self._current_s = self._seconds() % 60

	def __call__(self):
		return self._current_s


class TransformedSignal(SignalBase):
	def __init__(self, source_signal, y0, y1, discrete=False, value_range=None):
		super(TransformedSignal, self).__init__(value_range)
		self.source = source_signal
		self.y0 = y0
		self.y1 = y1
		if not discrete:
			self._transform = self.source.transform
		else:
			self._transform = self.source.discrete_transform

	@property
	def range(self):
		return (self.y0, self.y1)

	def __call__(self):
		return self._transform(self.y0, self.y1)


class StripPositionPhaseSignal(TransformedSignal):
	'''
	Uses the current pixel's strip postiion (managed externally) to calculate a proportional phase value
	'''

	def __init__(self, num_pixels):
		self.num_pixels = num_pixels
		super(StripPositionPhaseSignal, self).__init__(
			PublicValueSignal(value_range=(0, self.num_pixels-1)), 0, math.pi)

	def update(self, current_pixel_index):
		self.source.update(current_pixel_index)
