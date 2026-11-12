#include <FaoxiAmp/SmoothedGain.h>
#include <gtest/gtest.h>

#include <cmath>

namespace faoxiamp {

constexpr double testSampleRate = 48000.0;

// Catches: a wrong dB-to-gain conversion at 0 dB.
TEST(SmoothedGain, ZeroDbLeavesSignalUnchanged) {
  SmoothedGain smoothedGain;
  smoothedGain.prepare(testSampleRate);
  smoothedGain.setGainDecibels(0.f);

  juce::AudioBuffer<float> buffer{1, 512};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, 1.f);
  }

  smoothedGain.process(buffer);

  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    EXPECT_FLOAT_EQ(buffer.getSample(0, i), 1.f);
  }
}

// Catches: a 10 instead of 20 in the dB formula.
TEST(SmoothedGain, MinusSixDbHalvesAmplitude) {
  SmoothedGain smoothedGain;
  smoothedGain.prepare(testSampleRate);
  smoothedGain.setGainDecibels(-6.f);

  // The buffer must be longer than the ramp
  const int numSamples = static_cast<int>(testSampleRate * 2.0 * SmoothedGain::rampDurationSeconds);
  juce::AudioBuffer<float> buffer{1, numSamples};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, 1.f);
  }

  smoothedGain.process(buffer);

  // Expected final value: 10^(-6/20) which is approx 0.501
  const float expectedFinalValue = std::pow(10.f, -6.f / 20.f);
  EXPECT_NEAR(buffer.getSample(0, numSamples - 1), expectedFinalValue, 1e-4f);
}

// Catches: a gain that jumps instantly (audible click).
TEST(SmoothedGain, GainChangeIsSmoothed) {
  SmoothedGain smoothedGain;
  smoothedGain.prepare(testSampleRate);
  smoothedGain.setGainDecibels(-20.f); // Target: 10^(-20/20) = 0.1

  // We process for twice the ramp duration to ensure the ramp fully resolves
  const int numSamples = static_cast<int>(testSampleRate * 2.0 * SmoothedGain::rampDurationSeconds);
  juce::AudioBuffer<float> buffer{1, numSamples};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, 1.f);
  }

  smoothedGain.process(buffer);

  // The first sample hasn't dropped much yet
  EXPECT_GT(buffer.getSample(0, 0), 0.9f);

  // The ramp decreases continuously (each value is <= to the previous one)
  for (int i = 1; i < buffer.getNumSamples(); ++i) {
    EXPECT_LE(buffer.getSample(0, i), buffer.getSample(0, i - 1));
  }

  // The last sample must have reached the target (0.1)
  EXPECT_NEAR(buffer.getSample(0, numSamples - 1), 0.1f, 1e-3f);
}

// Catches: a channel-major loop giving each channel a different gain.
TEST(SmoothedGain, AllChannelsGetSameGain) {
  SmoothedGain smoothedGain;
  smoothedGain.prepare(testSampleRate);
  smoothedGain.setGainDecibels(-12.f); // Arbitrary change to trigger the ramp

  juce::AudioBuffer<float> buffer{2, 512};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, 1.f);
    buffer.setSample(1, i, 1.f);
  }

  smoothedGain.process(buffer);

  // 512 samples < ramp length: we compare the channels mid-ramp.
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    EXPECT_FLOAT_EQ(buffer.getSample(0, i), buffer.getSample(1, i));
  }
}

}  // namespace faoxiamp