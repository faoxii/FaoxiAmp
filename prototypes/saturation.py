import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf

SAMPLE_RATE = 48000
TEST_FREQUENCY_HZ = 110.0   # open A string
TEST_AMPLITUDE = 0.5
GAIN = 4.0                  # will come from InputGain later
BIAS = 0.6                  # > 0: asymmetric, tube-like saturation


def waveshaper(x, bias):
    return np.tanh(x + bias) - np.tanh(bias)


def spectrum_db(signal):
    window = np.hanning(len(signal))                  # smooths the edges of the signal
    magnitude = np.abs(np.fft.rfft(signal * window))  # amplitude of each frequency
    frequencies = np.fft.rfftfreq(len(signal), 1 / SAMPLE_RATE)
    magnitude_db = 20 * np.log10(magnitude / magnitude.max() + 1e-12)
    return frequencies, magnitude_db


assert abs(waveshaper(0.0, BIAS)) < 1e-12, "silence must stay silence"

# --- Test signal: a sine at TEST_FREQUENCY_HZ, 1 second ---
duration = 1.0
num_samples = int(SAMPLE_RATE * duration)  # 48000 samples for 1 second
n = np.arange(num_samples)                 # sample indices: 0, 1, 2, ..., 47999
t = n / SAMPLE_RATE                        # time of each sample, in seconds
x = TEST_AMPLITUDE * np.sin(2 * np.pi * TEST_FREQUENCY_HZ * t)

y_symmetric = waveshaper(GAIN * x, 0.0)
y_asymmetric = waveshaper(GAIN * x, BIAS)

fig, (ax_curve, ax_wave, ax_spectrum) = plt.subplots(3, 1, figsize=(9, 11), layout="constrained")

# 1) Transfer curve: output as a function of input


x_curve = np.linspace(-1, 1, 500)

ax_curve.plot(x_curve, waveshaper(GAIN * x_curve, 0.0), label="symmetric (no bias)")
ax_curve.plot(x_curve, waveshaper(GAIN * x_curve, BIAS), label=f"asymmetric (bias = {BIAS})")
ax_curve.set_title("Transfer curve")
ax_curve.set_xlabel("input")
ax_curve.set_ylabel("output")
ax_curve.grid()
ax_curve.legend()

# 2) Waveform: the first 2 periods
num_shown = int(2 * SAMPLE_RATE / TEST_FREQUENCY_HZ)

ax_wave.plot(t[:num_shown] * 1000, x[:num_shown], "k--", label="input")
ax_wave.plot(t[:num_shown] * 1000, y_symmetric[:num_shown], label="symmetric")
ax_wave.plot(t[:num_shown] * 1000, y_asymmetric[:num_shown], label="asymmetric")
ax_wave.set_title("Waveform (2 periods)")
ax_wave.set_xlabel("time (ms)")
ax_wave.grid()
ax_wave.legend()

# 3) Spectrum: which frequencies are in the signal
for signal, label in [(y_symmetric, "symmetric"), (y_asymmetric, "asymmetric")]:
    frequencies, magnitude_db = spectrum_db(signal)
    ax_spectrum.plot(frequencies, magnitude_db, label=label, alpha=0.8)

harmonics = [k * TEST_FREQUENCY_HZ for k in range(1, 9)]
ax_spectrum.set_xticks([0] + harmonics)
ax_spectrum.set_xticklabels(["DC"] + [f"{k}f" for k in range(1, 9)])
ax_spectrum.set_xlim(0, 9 * TEST_FREQUENCY_HZ)
ax_spectrum.set_ylim(-100, 5)
ax_spectrum.set_title("Spectrum")
ax_spectrum.set_ylabel("dB")
ax_spectrum.grid()
ax_spectrum.legend()

plt.savefig("prototypes/saturation_curves.png", dpi=120)


# 4) Audio export, to listen to the difference
output_level = 0.5  # keeps both files below 0 dBFS
sf.write("prototypes/input.wav", x, SAMPLE_RATE)
sf.write("prototypes/symmetric.wav", output_level * y_symmetric, SAMPLE_RATE)
sf.write("prototypes/asymmetric.wav", output_level * y_asymmetric, SAMPLE_RATE)

plt.show()