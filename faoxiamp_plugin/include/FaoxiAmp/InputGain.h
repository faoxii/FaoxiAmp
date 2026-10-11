#pragma once

#include <cmath>

#include <juce_audio_basics/juce_audio_basics.h>

namespace faoxiamp {

class InputGain {
public:
  // Gain in dB when the knob is at 10 (to tune by ear).
  static constexpr float maxGainDb = 36.0f;
  // Gain ramp duration when the knob moves (removes clicks).
  static constexpr double rampDurationSeconds = 0.05;

  [[nodiscard]] static float knobToDb(float knob) noexcept {
    return maxGainDb * knob / 10.f;
  }

  [[nodiscard]] static float knobToGain(float knob) noexcept {
    return std::pow(10.f, knobToDb(knob) / 20.f);
  }

  void prepare(double sampleRate) noexcept {
    smoothedGain.reset(sampleRate, rampDurationSeconds);
  }

  void setKnob(float knob) noexcept{ smoothedGain.setTargetValue(knobToGain(knob)); }

  void process(juce::AudioBuffer<float>& buffer) noexcept{
    const auto numFrames = buffer.getNumSamples();
    const auto numChannels = buffer.getNumChannels();


    for ( int frameIndex = 0; frameIndex < numFrames; frameIndex++) {

      const auto gainValue = smoothedGain.getNextValue();

      for ( int channel = 0; channel < numChannels; channel++) {
        // get the input sample
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
