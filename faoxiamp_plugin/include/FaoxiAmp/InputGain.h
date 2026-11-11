#pragma once

#include <cmath>

#include <juce_audio_basics/juce_audio_basics.h>

#include "FaoxiAmp/SmoothedGain.h"

namespace faoxiamp {
/** Preamp drive: maps the Gain knob (0 to 10) to dB, applied with smoothing. */
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

  void prepare(double sampleRate) noexcept { gain.prepare(sampleRate); }

  void setKnob(float knob) noexcept { gain.setGainDecibels(knobToDb(knob)); }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    gain.process(buffer);
  }

private:
  SmoothedGain gain;
};
}  // namespace faoxiamp
