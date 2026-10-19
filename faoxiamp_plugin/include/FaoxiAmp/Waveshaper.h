#pragma once

#include <cmath>

#include <juce_audio_basics/juce_audio_basics.h>

namespace faoxiamp {
/** Tube-style saturation: soft clipping with tanh, made asymmetric by a bias
 *  to add even harmonics.
 *  Memoryless: each sample is processed on its own. */
class Waveshaper {
public:
  // Shifts the operating point of the tanh: > 0 makes the curve asymmetric,
  // which adds even harmonics (tube-like). Tuned by ear.
  static constexpr float bias = 0.6f;

  // Slope of the curve at 0 (about 0.71 for a bias of 0.6).
  // Dividing by it makes small signals come out at the same level.
  static inline const float smallSignalGain =
      1.f - std::tanh(bias) * std::tanh(bias);

  [[nodiscard]] static float processSample(float sample) noexcept {
    return (std::tanh(sample + bias) - std::tanh(bias)) / smallSignalGain;
  }



  static void process(juce::AudioBuffer<float>& buffer) noexcept {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
      auto* samples = buffer.getWritePointer(channel);

      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        samples[i] = processSample(samples[i]);
      }
    }
  }
};
}  // namespace faoxiamp