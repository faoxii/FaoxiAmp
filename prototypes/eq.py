import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import freqz

SAMPLE_RATE = 48000
MAX_EQ_DB = 12.0  # boost/cut at knob 0 or 10 (to tune by ear)

BASS_FREQUENCY_HZ = 100.0
BASS_Q = 0.707  # 1/sqrt(2)

MID_FREQUENCY_HZ = 700.0
MID_Q = 0.7  # wide bell (about 2 octaves)

TREBLE_FREQUENCY_HZ = 2500.0
TREBLE_Q = 0.707


def _rbj_common_terms(f0, q, gain_db, fs):
    """Terms shared by every RBJ Audio EQ Cookbook biquad."""
    A = 10 ** (gain_db / 40)  # sqrt of the linear gain (RBJ notation)
    w0 = 2 * np.pi * f0 / fs  # normalized angular frequency, rad/sample
    alpha = np.sin(w0) / (2 * q)
    return A, np.cos(w0), alpha


def rbj_peak(f0, q, gain_db, fs):
    """Peaking (bell) biquad (RBJ Audio EQ Cookbook). Returns (b, a), a[0] == 1."""
    A, cos_w0, alpha = _rbj_common_terms(f0, q, gain_db, fs)

    b = np.array([1 + alpha * A, -2 * cos_w0, 1 - alpha * A])
    a = np.array([1 + alpha / A, -2 * cos_w0, 1 - alpha / A])
    return b / a[0], a / a[0]


def rbj_low_shelf(f0, q, gain_db, fs):
    """Low shelf biquad (RBJ Audio EQ Cookbook). Returns (b, a), a[0] == 1."""
    A, cos_w0, alpha = _rbj_common_terms(f0, q, gain_db, fs)
    two_sqrt_a_alpha = 2 * np.sqrt(A) * alpha

    b = np.array([
        A * ((A + 1) - (A - 1) * cos_w0 + two_sqrt_a_alpha),
        2 * A * ((A - 1) - (A + 1) * cos_w0),
        A * ((A + 1) - (A - 1) * cos_w0 - two_sqrt_a_alpha),
    ])
    a = np.array([
        (A + 1) + (A - 1) * cos_w0 + two_sqrt_a_alpha,
        -2 * ((A - 1) + (A + 1) * cos_w0),
        (A + 1) + (A - 1) * cos_w0 - two_sqrt_a_alpha,
    ])
    return b / a[0], a / a[0]


def rbj_high_shelf(f0, q, gain_db, fs):
    """High shelf biquad (RBJ Audio EQ Cookbook). Returns (b, a), a[0] == 1."""
    A, cos_w0, alpha = _rbj_common_terms(f0, q, gain_db, fs)
    two_sqrt_a_alpha = 2 * np.sqrt(A) * alpha

    b = np.array([
        A * ((A + 1) + (A - 1) * cos_w0 + two_sqrt_a_alpha),
        -2 * A * ((A - 1) + (A + 1) * cos_w0),
        A * ((A + 1) + (A - 1) * cos_w0 - two_sqrt_a_alpha),
    ])
    a = np.array([
        (A + 1) - (A - 1) * cos_w0 + two_sqrt_a_alpha,
        2 * ((A - 1) - (A + 1) * cos_w0),
        (A + 1) - (A - 1) * cos_w0 - two_sqrt_a_alpha,
    ])
    return b / a[0], a / a[0]


def knob_to_db(k):
    """Amp-style knob (0 to 10, 5 = flat) to EQ gain in dB."""
    return MAX_EQ_DB * (k - 5) / 5


# --- Helpers to read a filter's gain ---
def gain_db_at(b, a, frequency_hz):
    """Gain of the filter at one frequency, in dB."""
    _, h = freqz(b, a, worN=[frequency_hz], fs=SAMPLE_RATE)
    return 20 * np.log10(np.abs(h[0]))


def dc_gain(b, a):
    """Linear gain at 0 Hz (z = 1)."""
    return b.sum() / a.sum()
def nyquist_gain(b, a):
    """Linear gain at fs/2 (z = -1)."""
    return (b[0] - b[1] + b[2]) / (a[0] - a[1] + a[2])


BANDS = {
    "bass": (rbj_low_shelf, BASS_FREQUENCY_HZ, BASS_Q),
    "mid": (rbj_peak, MID_FREQUENCY_HZ, MID_Q),
    "treble": (rbj_high_shelf, TREBLE_FREQUENCY_HZ, TREBLE_Q),
}
FULL_BOOST = 10 ** (MAX_EQ_DB / 20)  # linear gain of +12 dB, about 3.98

# --- Checks ---

# 1. Knob at 5: every band is transparent (b == a, so H = 1 everywhere).
for name, (design, f0, q) in BANDS.items():
    b, a = design(f0, q, knob_to_db(5), SAMPLE_RATE)
    assert np.allclose(b, a, atol=1e-12), f"{name} is not flat at knob 5"

# 2. Bass at 10: full boost far below the corner (at 0 Hz).
b, a = rbj_low_shelf(BASS_FREQUENCY_HZ, BASS_Q, knob_to_db(10), SAMPLE_RATE)
assert abs(dc_gain(b, a) - FULL_BOOST) < 1e-9

# 3. Treble at 10: full boost far above the corner (at fs/2).
b, a = rbj_high_shelf(TREBLE_FREQUENCY_HZ, TREBLE_Q, knob_to_db(10), SAMPLE_RATE)
assert abs(nyquist_gain(b, a) - FULL_BOOST) < 1e-9

# 4. Mid at 10: +12 dB exactly at the bell centre.
b, a = rbj_peak(MID_FREQUENCY_HZ, MID_Q, knob_to_db(10), SAMPLE_RATE)
assert abs(gain_db_at(b, a, MID_FREQUENCY_HZ) - MAX_EQ_DB) < 1e-6

# 5. RBJ shelves reach half the gain (in dB) at their corner frequency.
for design, f0, q in (BANDS["bass"], BANDS["treble"]):
    b, a = design(f0, q, knob_to_db(10), SAMPLE_RATE)
    assert abs(gain_db_at(b, a, f0) - MAX_EQ_DB / 2) < 1e-6

print("All EQ checks passed.")


# --- Frequency response figure ---

KNOB_POSITIONS = [0, 2.5, 5, 7.5, 10]
FREQUENCIES_HZ = np.logspace(np.log10(20), np.log10(20000), 500)


def response_db(b, a):
    """Gain of the filter in dB over FREQUENCIES_HZ."""
    _, h = freqz(b, a, worN=FREQUENCIES_HZ, fs=SAMPLE_RATE)
    return 20 * np.log10(np.abs(h))


def band_response_db(band, knob):
    design, f0, q = BANDS[band]
    return response_db(*design(f0, q, knob_to_db(knob), SAMPLE_RATE))


fig, axes = plt.subplots(4, 1, figsize=(9, 12), layout="constrained")

for ax, band in zip(axes[:3], BANDS):
    for knob in KNOB_POSITIONS:
        ax.plot(FREQUENCIES_HZ, band_response_db(band, knob), label=f"{band} = {knob}")
    ax.axvline(BANDS[band][1], linestyle="--", color="gray")
    ax.set_title(f"{band.capitalize()} ({BANDS[band][1]:g} Hz)")

# Example "Fender-like" setting: the three bands are in series, so their
# gains multiply, i.e. their responses in dB add up.
fender_setting = {"bass": 6, "mid": 3, "treble": 7}
total_db = sum(band_response_db(band, knob) for band, knob in fender_setting.items())
axes[3].plot(FREQUENCIES_HZ, total_db, color="black",
             label="bass 6 / mid 3 / treble 7")
axes[3].set_title("Total response, Fender-like setting")

for ax in axes:
    ax.set_xscale("log")
    ax.set_xlim(20, 20000)
    ax.set_ylim(-14, 14)
    ax.axhline(0, color="gray", linewidth=0.5)
    ax.set_xlabel("frequency (Hz)")
    ax.set_ylabel("gain (dB)")
    ax.grid(True, which="both", alpha=0.3)
    ax.legend(loc="upper right", fontsize="small")

fig.savefig("prototypes/eq_response.png")
plt.show()
