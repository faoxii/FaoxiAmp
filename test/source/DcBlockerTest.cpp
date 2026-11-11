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

  EXPECT_GT(buffer.getSample(0, 0), 0.49f);  // GT = greater than
  EXPECT_NEAR(buffer.getSample(0, buffer.getNumSamples() - 1), 0.f, 1e-3f);
}

}  // namespace faoxiamp