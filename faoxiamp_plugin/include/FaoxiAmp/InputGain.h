#pragma once

namespace faoxiamp {

class InputGain {
public:

  // Gain in dB when the knob is at 10 (to tune by ear).
  static constexpr float maxGainDb = 36.0f;

  [[nodiscard]] static float knobToDb(float knob) noexcept {
    return maxGainDb * knob / 10.f;
  }

  [[nodiscard]] static float knobToGain(float knob) noexcept {
    return std::pow(10.f, knobToDb(knob) / 20.f);
  }


private:
};
}  // namespace faoxiamp
