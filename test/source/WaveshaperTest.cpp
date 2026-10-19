#include <FaoxiAmp/Waveshaper.h>
#include <gtest/gtest.h>

#include <cmath>

namespace faoxiamp {

TEST(Waveshaper, SilenceStaysSilent) {
  EXPECT_NEAR(Waveshaper::processSample(0.f), 0.f, 1e-6f);
}

TEST(Waveshaper, OutputIsBounded) {
  const float upperLimit = (1.f - std::tanh(Waveshaper::bias)) / Waveshaper::smallSignalGain;
  const float lowerLimit = (-1.f - std::tanh(Waveshaper::bias)) / Waveshaper::smallSignalGain;

  for (const float input : {-1000.f, -10.f, -1.f, 0.f, 1.f, 10.f, 1000.f}) {
    const float output = Waveshaper::processSample(input);
    EXPECT_LE(output, upperLimit) << "input = " << input;
    EXPECT_GE(output, lowerLimit) << "input = " << input;
  }
}

TEST(Waveshaper, CurveIsAsymmetric) {
  const float outputAtOne = Waveshaper::processSample(1.f);
  const float outputAtMinusOne = Waveshaper::processSample(-1.f);

  EXPECT_LT(std::abs(outputAtOne), std::abs(outputAtMinusOne))
      << "f(1) = " << outputAtOne << ", f(-1) = " << outputAtMinusOne;
}

TEST(Waveshaper, SmallSignalsPassAtUnityGain) {
  const float input = 1e-4f;
  const float output = Waveshaper::processSample(input);
  EXPECT_NEAR(output, input, 1e-6f);
}

}  // namespace faoxiamp