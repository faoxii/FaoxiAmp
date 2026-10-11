
#include <faoxiamp_plugin/faoxiamp_plugin.h>
#include <gtest/gtest.h>

namespace faoxiamp {

struct TestValue {
  float knobPosition;
  float expectedGain;
};

TEST(InputGain, KnobAtZeroLeavesSignalUnchanged) {
  EXPECT_NEAR(InputGain::knobToGain(0.f), 1.f, 0.01f);
}

TEST(InputGain, MatchesReferenceValues) {
  const std::vector<TestValue> referenceValues = {{0.0f, 1.0f},
                                                  {2.5f, 2.82f},
                                                  {5.0f, 7.94f},
                                                  {7.5f, 22.39f},
                                                  {10.0f, 63.1f}};

  for (const auto& value : referenceValues) {
    EXPECT_NEAR(InputGain::knobToGain(value.knobPosition), value.expectedGain,
                0.01f)
        << "knob = " << value.knobPosition;
  }
}

TEST(InputGain, EachKnobStepAddsTheSameGain) {
  const float expectedRatio =
      std::pow(10.f, (InputGain::maxGainDb / 10.f) / 20.f);
  const float tolerance = 1e-5f;

  for (int k = 0; k < 10; ++k) {
    const float ratio = InputGain::knobToGain(static_cast<float>(k + 1)) /
                        InputGain::knobToGain(static_cast<float>(k));
    EXPECT_NEAR(ratio, expectedRatio, tolerance) << "k = " << k;
  }
}

}  // namespace faoxiamp