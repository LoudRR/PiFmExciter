PiFmExciter
=========


## FM exciter using the Raspberry Pi

This program generates a mono FM modulation intended for raw multiplexed data.

PiFmExciter modulates the PLLC instead of the clock divider for better signal purity, which means that the signal is also less noisy. This has a great impact on stereo as it's reception is way better.

For the PLLC modulation to be stable there is one additional step to do. Due to the low voltage detection the PLLC frequency can be reduced to safe value in an attempt to prevent crashes. When this happens, the carrier freqency changes based on the original GPU frequency.
To prevent this, we can easily change the GPU freqency to match the safe frequency. Now when due to the low voltage detection the PLLC frequency changes to safe value, nothing happens as the normal value and safe value are the same.

Simply add `gpu_freq=250` to `/boot/config.txt`.

It is based on the FM transmitter created by [Oliver Mattos and Oskar Weigl](http://www.icrobotics.co.uk/wiki/index.php/Turning_the_Raspberry_Pi_Into_an_FM_Transmitter), and later adapted to using DMA by [Richard Hirst](https://github.com/richardghirst). Christophe Jacquet adapted it and added the RDS data generator and modulator. The transmitter uses the Raspberry Pi's PWM generator to produce VHF signals.

It is compatible with both the Raspberry Pi 1 (the original one) and the Raspberry Pi 2 and 3.

PiFmExciter has been developed for experimentation only. It is not a media center, it is not intended to broadcast music to your stereo system. See the [legal warning](#warning-and-disclaimer).

## MPX

Whilst other PiFM transmitters on GitHub are intended to take an audio file and generate stereo audio and RDS this program does not do this.

Instead it is expected that this program receives 192kHz MPX-coded data for modulation.  Consequently there is no low-pass filter, pre-emphasis, HF limiters or clippers in this code.

## How to use it?

PiFmExciter, depends on the `sndfile` library. To install this library on Debian-like distributions, for instance Raspbian, run `sudo apt-get install libsndfile1-dev`.

PiFmExciter also depends on the Linux `rpi-mailbox` driver, so you need a recent Linux kernel. The Raspbian releases from August 2015 have this.

**Important.** The binaries compiled for the Raspberry Pi 1 are not compatible with the Raspberry Pi 2/3, and conversely. Always re-compile when switching models, so do not skip the `make clean` step in the instructions below!

Clone the source repository and run `make` in the `src` directory:

```bash
git clone https://github.com/LoudRR/PiFmExciter.git
cd PiFmExciter/src
make clean
make
```

Then you can just run:

```
sudo ./pi_fm_adv
```

This will generate an FM transmission on 87.6 MHz without audio. The radiofrequency signal is emitted on GPIO 4 (pin 7 on header P1).


You can add audio by referencing an audio file as follows:

```
sudo ./pi_fm_adv --audio 192mpx.wav
```

The more general syntax for running the program is as follows:

```
pi_fm_exciter
```

All arguments are optional:

* `--freq` specifies the carrier frequency (in MHz). Example: `--freq 87.6`.
* `--audio` specifies an MPX encoded audio file to play as audio. Specify `-` as the file name to read audio data on standard input (useful for piping audio into PiFmExciter, see below).
* `--dev` specifies the frequency deviation (in KHz). Example `--dev 25.0`.
* `--mpx` specifies the output mpx power. Default 30. Example `--mpx 20`.
* `--power` specifies the drive strenght of gpio pads. 0 = 2mA ... 7 = 16mA. Default 7. Example `--power 5`.
* `--gpio` specifies the GPIO pin used for transmitting. Available GPIO pins: 4, 20, 32, 34. Default 4. Example `--gpio 32`.
* `--ppm` specifies your Raspberry Pi's oscillator error in parts per million (ppm), see below.
* `--wait` specifies whether PiFmExciter should wait for the the audio pipe or terminate as soon as there is no audio. It's set to 1 by default. 

### Clock calibration (only if experiencing difficulties)

The RDS standards states that the error for the 57 kHz subcarrier must be less than ± 6 Hz, i.e. less than 105 ppm (parts per million). The Raspberry Pi's oscillator error may be above this figure. That is where the `--ppm` parameter comes into play: you specify your Pi's error and PiFmExciter adjusts the clock dividers accordingly.

In practice, I found that PiFmExciter works okay even without using the `--ppm` parameter. I suppose the receivers are more tolerant than stated in the RDS spec.

One way to measure the ppm error is to play the `pulses.wav` file: it will play a pulse for precisely 1 second, then play a 1-second silence, and so on. Record the audio output from a radio with a good audio card. Say you sample at 44.1 kHz. Measure 10 intervals. Using [Audacity](http://audacity.sourceforge.net/) for example determine the number of samples of these 10 intervals: in the absence of clock error, it should be 441,000 samples. With my Pi, I found 441,132 samples. Therefore, my ppm error is (441132-441000)/441000 * 1e6 = 299 ppm, **assuming that my sampling device (audio card) has no clock error...**


### Piping audio into PiFmExciter

If you use the argument `--audio -`, PiFmExciter reads audio data on standard input. This allows you to pipe the output of a program into PiFmExciter. For instance, this can be used to read wav files:

```
aplay 192mpx.wav -  | sudo ./pi_fm_adv --audio -
```

Or to pipe a working MPX stream using MPlayer:

```
mkdir fifo
mplayer -ao:pcm:waveheader:file=fifo http://5.35.250.101:8000/radio700-mpx.flac
sudo ./pi_fm_exciter --dev 75 --mpx 10.5 --audio fifo
```

### About the sample file

The sample file (192mpx.wav) is a 60 second recording from the above MPX-encoded stream at a 50us pre-emphasis curve.  If you are using a receiver from Europe, Australia or some parts of Asia you should be enjoying high quality stereo output and RDS tagged by Radio700 in Germany.

Listeners form the US might here a slightly more 'bass' sound.

## Warning and Disclaimer

PiFmExciter is an **experimental** program, designed **only for experimentation**. It is not going to replace a commercial FM exciter.

In most countries, transmitting radio waves without a state-issued licence specific to the transmission modalities (frequency, power, bandwidth, etc.) is **illegal**.

Therefore, always connect a shielded transmission line from the Raspberry Pi directly
to a radio receiver, so as **not** to emit radio waves. Never use an antenna.

Even if you are a licensed amateur radio operator, using PiFmExciter to transmit radio waves on ham frequencies without any filtering between the RaspberryPi and an antenna is most probably illegal because the square-wave carrier is very rich in harmonics, so the bandwidth requirements are likely not met.

I could not be held liable for any misuse of your own Raspberry Pi. Any experiment is made under your own responsibility.


## Tests

PiFmExciter was successfully tested with all my RDS-able devices, namely:

* a $10 DAB/FM stereo receiver from AliExpress,
* a HackRF using GnuRadio with all the right filter blocks in place,
* a RTL-SDR dongle using FM Receiver software.

### CPU Usage

CPU usage is extremely low due to no DSP:

* with audio audio: 4%

--------

C [Loudrr](https://www.loudrr.com) & [Miegl](https://miegl.cz) & [Christophe Jacquet](http://www.jacquet80.eu/) (F8FTK), 2014-2017. Released under the GNU GPL v3.
