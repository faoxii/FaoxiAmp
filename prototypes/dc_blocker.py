import numpy as np
import matplotlib.pyplot as plt

SAMPLE_RATE = 48000
TEST_FREQUENCY_HZ = 110.0   # open A string
TEST_AMPLITUDE = 0.5
GAIN = 4.0                  # will come from InputGain later
BIAS = 0.6                  # > 0: asymmetric, tube-like saturation
CUTOFF_HZ = 10.0

def waveshaper(x, bias):
    small_signal_gain = 1 - np.tanh(bias) ** 2
    return (np.tanh(x + bias) - np.tanh(bias)) / small_signal_gain

def dc_blocker(signal, alpha):
    # array of zeros, same size as signal
    y = np.zeros_like(signal)
    m = np.zeros_like(signal)

    # initialize mean at 0
    running_mean = 0.0

    for i in range(len(signal)):
        running_mean = running_mean + alpha * (signal[i]-running_mean)
        m[i] = running_mean
        y[i] = signal[i] - m[i]

    return y, m



duration = 0.2
n = np.arange(int(SAMPLE_RATE * duration))
t = n / SAMPLE_RATE
x = waveshaper(GAIN * TEST_AMPLITUDE * np.sin(2 * np.pi * TEST_FREQUENCY_HZ * t), BIAS)

alpha = 2 * np.pi * CUTOFF_HZ / SAMPLE_RATE
time_constant_ms = 1000 / (alpha * SAMPLE_RATE)
print(f"Time constant tau: {time_constant_ms:.1f} ms")
y, m = dc_blocker(x, alpha)



# --- Check: after convergence, the output mean must be ~0 ---
samples_per_period = round(SAMPLE_RATE / TEST_FREQUENCY_HZ)
last_period = y[-samples_per_period:]          # the last samples_per_period samples
output_mean = np.mean(last_period)
assert abs(output_mean) < 1e-3, f"output mean is {output_mean}, expected ~0"
print(f"Output mean over the last period: {output_mean:.6f}")

# --- Plot ---
time_ms = t * 1000

fig, (ax_top, ax_bottom) = plt.subplots(2, 1, layout="constrained")

ax_top.plot(time_ms, x, label="input x (saturated)")
ax_top.plot(time_ms, m, label="running mean m")
ax_top.axhline(np.mean(x), linestyle="--", color="gray", label="true mean")
ax_top.set_title("Running mean converges to the DC offset")
ax_top.set_xlabel("time (ms)")
ax_top.legend()

ax_bottom.plot(time_ms, y, label="output y = x - m")
ax_bottom.axhline(0, linestyle="--", color="gray")
ax_bottom.set_title("Output is centered on 0")
ax_bottom.set_xlabel("time (ms)")
ax_bottom.legend()

fig.savefig("prototypes/dc_blocker.png")
plt.show()