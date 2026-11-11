#pragma once

#include <algorithm>
#include <vector>

#include <juce_audio_basics/juce_audio_basics.h>

namespace faoxiamp {
/** Removes the DC offset by subtracting a running mean of the signal
 *  (first-order high-pass filter).
 *  Cutoff at 10 Hz: inaudible, but fast enough (time constant about 16 ms).
 *  Stateful: one running mean per channel, kept from one block to the next. */
class DcBlocker {
public:
  // Low enough to leave the lowest guitar note (82 Hz) untouched.
  static constexpr float cutoffHz = 10.f;

  // Call before playback (from prepareToPlay). Allocates: not real-time safe.
  void prepare(double sampleRate, int numChannels) {
    alpha = static_cast<float>((juce::MathConstants<double>::twoPi * cutoffHz) /
                               sampleRate);
    runningMeans.resize(static_cast<size_t>(numChannels));
    reset();
  }

  // Forgets the DC estimate (e.g. when playback restarts). Real-time safe.
  void reset() { std::ranges::fill(runningMeans, 0.f); }

  // Removes the DC offset from every channel of the buffer, in place.
  void process(juce::AudioBuffer<float>& buffer) {
    // prepare() must have been called with at least as many channels as the
    // buffer.
    jassert(static_cast<size_t>(buffer.getNumChannels()) <=
            runningMeans.size());
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
      auto* samples = buffer.getWritePointer(channel);
      auto mean = runningMeans[static_cast<size_t>(channel)];

      for (int i = 0; i < buffer.getNumSamples(); ++i) {
        // Move the running mean a small step (alpha) towards the current
        // sample.
        mean += alpha * (samples[i] - mean);
        // Remove the estimated DC offset.
        samples[i] -= mean;
      }
      // Store the mean for the next block.
      runningMeans[static_cast<size_t>(channel)] = mean;
    }
  }

private:
  // Smoothing factor of the running mean, computed in prepare().
  float alpha = 0.0f;

  // Running mean of each channel (the DC estimate).
  std::vector<float> runningMeans;
};
}  // namespace faoxiamp