#pragma once

namespace faoxiamp {

class InputGain {
public:
  // Gain in dB when the knob is at 10 (to tune by ear).
  static constexpr float maxGainDb = 36.0f;
  // Gain ramp duration when the knob moves (removes clicks). ;
  static constexpr double rampDurationSeconds = 0.05;

  [[nodiscard]] static float knobToDb(float knob) noexcept {
    return maxGainDb * knob / 10.f;
  }

  [[nodiscard]] static float knobToGain(float knob) noexcept {
    return std::pow(10.f, knobToDb(knob) / 20.f);
  }

  void prepare(double sampleRate) {
    smoothedGain.reset(sampleRate, rampDurationSeconds);
  }

  void setKnob(float knob) noexcept{ smoothedGain.setTargetValue(knobToGain(knob)); }

private:
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>
      smoothedGain{1.f};
};
}  // namespace faoxiamp
