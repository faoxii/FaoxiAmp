#include "FaoxiAmp/Parameters.h"

#include <memory>

namespace faoxiamp {
namespace {
// Version hint of every juce::ParameterID: the plugin version in which the
// parameter first appeared. Hosts use it to keep
// parameters in a stable order across plugin updates.
// - Parameters added in a later release get a higher hint (2, 3...).
// - Existing parameters keep their hint, and their ID never changes.
constexpr auto versionHint = 1;

auto& addParameterToProcessor(juce::AudioProcessor& processor, auto parameter) {
  auto& result = *parameter;
  processor.addParameter(parameter.release());
  return result;
}

// Amp-style knob, from 0 to 10, like on a real amp front panel.
juce::AudioParameterFloat& createKnobParameter(juce::AudioProcessor& processor,
                                               const juce::String& id,
                                               const juce::String& name,
                                               float defaultValue) {
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{id, versionHint}, name,
          juce::NormalisableRange<float>{0.f, 10.f, 0.1f}, defaultValue));
}

juce::AudioParameterFloat& createDecibelParameter(
    juce::AudioProcessor& processor,
    const juce::String& id,
    const juce::String& name,
    float minDb,
    float maxDb,
    float defaultDb) {
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterFloat>(
          juce::ParameterID{id, versionHint}, name,
          juce::NormalisableRange<float>{minDb, maxDb, 0.1f}, defaultDb,
          juce::AudioParameterFloatAttributes{}.withLabel("dB")));
}

juce::AudioParameterBool& createCabEnabledParameter(
    juce::AudioProcessor& processor) {
  return addParameterToProcessor(
      processor,
      std::make_unique<juce::AudioParameterBool>(
          juce::ParameterID{"cab.enabled", versionHint}, "Cab", true));
}
}  // namespace

Parameters::Parameters(juce::AudioProcessor& processor)
    : gain{createKnobParameter(processor, "input.gain", "Gain", 5.f)},
      bass{createKnobParameter(processor, "eq.bass", "Bass", 5.f)},
      mid{createKnobParameter(processor, "eq.mid", "Mid", 5.f)},
      treble{createKnobParameter(processor, "eq.treble", "Treble", 5.f)},
      volume{createDecibelParameter(processor,
                                    "output.volume",
                                    "Volume",
                                    -48.f,
                                    12.f,
                                    0.f)},
      cabEnabled{createCabEnabledParameter(processor)} {}
}  // namespace faoxiamp
