import numpy as np
import matplotlib.pyplot as plt

MAX_GAIN_DB = 36.0  # gain in dB when the knob is at 10 (to tune by ear)


def knob_to_db(k):
    return MAX_GAIN_DB * k / 10


def knob_to_gain(k):
    return 10 ** (knob_to_db(k) / 20)

# (knob position, expected gain)
reference_values = [
    (0.0, 1.0),
    (2.5, 2.82),
    (5.0, 7.94),
    (7.5, 22.39),
    (10.0, 63.1),
]

for k, expected in reference_values:
    assert abs(knob_to_gain(k) - expected) < 0.01, f"k={k}"

for k in range(10):
    ratio = knob_to_gain(k + 1) / knob_to_gain(k)
    assert abs(ratio - 10 ** (MAX_GAIN_DB / 10 / 20)) < 1e-9, f"k={k}"

print("All gain checks passed")

# 101 knob positions evenly spaced from 0 to 10: 0.0, 0.1, 0.2, ..., 10.0
knob = np.linspace(0, 10, 101)

gain_db = knob_to_db(knob)  # gain in dB for every knob position
gain_linear = knob_to_gain(knob)  # linear gain factor for every knob position

fig, (ax_db, ax_linear) = plt.subplots(2, 1, figsize=(8, 7))

ax_db.plot(knob, gain_db)
ax_db.set_title("Gain in dB")
ax_db.set_xlabel("Knob position")
ax_db.set_ylabel("dB")
ax_db.grid()

ax_linear.plot(knob, gain_linear)
ax_linear.set_title("Linear gain factor")
ax_linear.set_xlabel("Knob position")
ax_linear.set_ylabel("g")
ax_linear.grid()

plt.tight_layout()
plt.savefig("prototypes/gain_mapping.png", dpi=120)
plt.show()