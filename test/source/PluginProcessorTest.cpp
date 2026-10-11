#include <faoxiamp_plugin/faoxiamp_plugin.h>
#include <gtest/gtest.h>

namespace faoxiamp {
TEST(PluginProcessor, Constructs) {
  PluginProcessor processor{};
  EXPECT_EQ(processor.getName(), juce::String{"FaoxiAmp"});
}

TEST(PluginProcessor, MonoInputIsCopiedToBothOutputs) {
  PluginProcessor processor{};
  processor.prepareToPlay(48000.0, 64);

  // 2 channels: channel 0 holds the mono guitar input, channel 1 is garbage.
  juce::AudioBuffer<float> buffer{2, 64};
  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    buffer.setSample(0, i, std::sin(0.1f * static_cast<float>(i)));
    buffer.setSample(1, i, 123.f);
  }

  juce::MidiBuffer midi;
  processor.processBlock(buffer, midi);

  for (int i = 0; i < buffer.getNumSamples(); ++i) {
    EXPECT_FLOAT_EQ(buffer.getSample(1, i), buffer.getSample(0, i));
  }
}
}  // namespace faoxiamp
