#!/usr/bin/python


#   PiFmExciter - Advanced FM exciter for the Raspberry Pi
#   Copyright (C) 2018 LOUDRR
#   Based on original works by Miegl (https://github.com/Miegl/PiFmAdv)
#
#   See https://github.com/LoudRR/PiFmExciter

import scipy.io.wavfile as wavfile
import numpy

sample_rate = 228000
samples = numpy.zeros(15 * sample_rate, dtype=numpy.dtype('>i2'))

# 1-second tune
samples[:10*sample_rate] = (numpy.sin(2*numpy.pi*440*numpy.arange(10*sample_rate)/sample_rate)
        * 20000).astype(numpy.dtype('>i2'))

wavfile.write("pulses.wav", sample_rate, samples)
