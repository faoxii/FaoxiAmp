#pragma once

#include <cmath>

#include <juce_audio_basics/juce_audio_basics.h>

namespace faoxiamp {
/** Multiplies every channel by a gain given in dB, smoothed multiplicatively
 *  so that gain changes do not click. */
class SmoothedGain {
public:
  // Gain ramp duration when the knob moves (removes clicks).
  static constexpr double rampDurationSeconds = 0.05;

  void prepare(double sampleRate) noexcept {
    smoothedGain.reset(sampleRate, rampDurationSeconds);
  }

  void setGainDecibels(float gainDb) noexcept {
    smoothedGain.setTargetValue(std::pow(10.0f, gainDb / 20.f));
  }

  void process(juce::AudioBuffer<float>& buffer) noexcept {
    const auto numFrames = buffer.getNumSamples();
    const auto numChannels = buffer.getNumChannels();

    // Frame-major: one gain value per instant, shared by all channels.
    for (int frameIndex = 0; frameIndex < numFrames; frameIndex++) {
      const auto gainValue = smoothedGain.getNextValue();

      for (int channel = 0; channel < numChannels; channel++) {
        const auto inputSample = buffer.getSample(channel, frameIndex);

        const auto outputSample = inputSample * gainValue;

        buffer.setSample(channel, frameIndex, outputSample);
      }
    }
  }

private:
  juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>
      smoothedGain{1.f};
};
}  // namespace faoxiamp
