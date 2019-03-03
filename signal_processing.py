import math
import time
## from https://gist.github.com/tdicola/f4324c9ae813410182d5ed00e866c8fb

#Core classes
class SignalSource:
    def __init__(self, source=None):
        self.set_source(source)

    def __call__(self):
        # Get the source signal value and return it when reading this signal
        # source's value.
        return self._source()

    def set_source(self, source):
        # Allow setting this signal source to either another signal (anything
        # callable) or a static value (for convenience when something is a
        # fixed value that never changes).
        if callable(source):
            # Callable source, save it directly.
            self._source = source
        else:
            # Not callable, assume it's a static value and make a lambda
            # that's callable to capture and always return it.
            self._source = lambda: source

class SignalBase(object):
    def __init__(self, value_range=None):
        self.value_range = value_range
        
    def __call__(self):
        raise NotImplementedError('Signal must have a callable implementation!')

    def transform(self, y0, y1):
        # Transform the current value of this signal to a new value inside the
        # specified target range (y0...y1).  If this signal has no bounds/range
        # then the value is just clamped to the specified range.
        x = self()
        if self.value_range is not None:
            # This signal has a known range so we can interpolate between it
            # and the desired target range (y0...y1).
            return y0 + (x-self.value_range[0]) * \
                        ((y1-y0)/(self.value_range[1]-self.value_range[0]))
        else:
            # No range of values for this signal, can't interpolate so just
            # clamp to a value inside desired target range.
            return max(y0, min(y1, x))

    def discrete_transform(self, y0, y1):
        # Transform assuming discrete integer values instead of floats.
        return int(self.transform(y0, y1))

#Signals
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

    # -skain I'm not going to worry about this right now since the complexity is too much at this point.
    # I may have to revisit in the future if I want to have variable amplitudes here
    # @property
    # def range(self):
    #     # Since amplitude might be a changing signal, the range of this signal
    #     # changes too and must be computed on the fly!  This might not really
    #     # be necessary in practice and could be switched back to a
    #     # non-SignalSource static value set once at initialization.
    #     amplitude = self.amplitude()
    #     return (-amplitude, amplitude)

    def __call__(self):
        return self.amplitude() * \
               math.sin(2*math.pi*self.frequency()*self.time() + self.phase())

class FrameClockSignal(SignalBase):

    def __init__(self):
        self.update()

    def _seconds(self):
        return time.monotonic()  # CircuitPython function for current seconds.

    def update(self):
        # Hack below to reduce the impact noisey ADC frequency.  When time
        # values build up to large number then small frequency variations (like
        # noise from the ADC/potentiometer) are greatly magnified.  By running
        # the current seconds through a modulo 60 it will prevent the frame
        # clock from getting large values while still increasing and wrapping
        # at the same rate. This will only work for driving repeating signals
        # like sine waves, etc.
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
        super(StripPositionPhaseSignal, self).__init__(PublicValueSignal(value_range=(0, self.num_pixels-1)), 0, math.pi)

    def update(self, current_pixel_index):
        self.source.update(current_pixel_index)
