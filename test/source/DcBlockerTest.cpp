#include <FaoxiAmp/DcBlocker.h>
#include <gtest/gtest.h>

#include <cmath>

namespace faoxiamp {

constexpr double testSampleRate = 48000.0;

TEST(DcBlocker, SilenceStaysSilent) {
  DcBlocker dcBlocker;
  dcBlocker.prepare(testSampleRate, 1);

  juce::AudioBuffer<float> buffer{1, 512};
  buffer.clear();
  dcBlocker.process(buffer);

  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    EXPECT_EQ(buffer.getSample(0, i), 0.f);
  }
}

TEST(DcBlocker, ConstantInputDecaysToZero) {
  DcBlocker dcBlocker;
  dcBlocker.prepare(testSampleRate, 1);

  juce::AudioBuffer<float> buffer{1, static_cast<int>(testSampleRate)};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, 0.5f);
  }
  dcBlocker.process(buffer);

  // First sample still close to the input: the mean has barely started to move.
  EXPECT_GT(buffer.getSample(0, 0), 0.49f);
  EXPECT_NEAR(buffer.getSample(0, buffer.getNumSamples() - 1), 0.f, 1e-3f);
}

TEST(DcBlocker, TimeConstantMatchesCutoff) {
  DcBlocker dcBlocker;
  dcBlocker.prepare(testSampleRate, 1);

  const auto samplesPerTimeConstant = static_cast<int>(
      std::round(testSampleRate /
                 (juce::MathConstants<double>::twoPi * DcBlocker::cutoffHz)));

  juce::AudioBuffer<float> buffer{1, samplesPerTimeConstant};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, 1.f);
  }

  dcBlocker.process(buffer);

  // After one time constant, 63 % of the step is removed: e^-1 remains.
  EXPECT_NEAR(buffer.getSample(0, buffer.getNumSamples() - 1), std::exp(-1.f),
              0.01f);
}

TEST(DcBlocker, ChannelsHaveIndependentState) {
  DcBlocker dcBlocker;
  dcBlocker.prepare(testSampleRate, 2);

  juce::AudioBuffer<float> buffer{2, 512};
  buffer.clear();
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, 1.f);
  }

  dcBlocker.process(buffer);

  // The DC on channel 0 must not leak into channel 1.
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    EXPECT_EQ(buffer.getSample(1, i), 0.f);
  }
}

}  // namespace faoxiamp