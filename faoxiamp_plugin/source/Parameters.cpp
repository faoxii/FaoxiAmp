namespace faoxiamp {
namespace {
// Version hint of every parameter ID. Increase it only if a parameter
// changes meaning, so hosts (Logic, AU) don't mix old and new sessions.
constexpr auto versionHint = 1;

auto& addParameterToProcessor(juce::AudioProcessor& processor, auto parameter) {
  auto& result = *parameter;
  processor.addParameter(parameter.release());
  return result;
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
      processor, std::make_unique<juce::AudioParameterBool>(
                     juce::ParameterID{"cab.enabled", versionHint}, "Cab", true));
}
}  // namespace

// Ranges are first guesses: we will tune them by ear once the DSP is in.
Parameters::Parameters(juce::AudioProcessor& processor)
    : gain{createDecibelParameter(processor, "input.gain", "Gain", 0.f, 36.f,
                                  12.f)},
      bass{createDecibelParameter(processor, "eq.bass", "Bass", -12.f, 12.f,
                                  0.f)},
      mid{createDecibelParameter(processor, "eq.mid", "Mid", -12.f, 12.f,
                                 0.f)},
      treble{createDecibelParameter(processor, "eq.treble", "Treble", -12.f,
                                    12.f, 0.f)},
      volume{createDecibelParameter(processor, "output.volume", "Volume",
                                    -48.f, 12.f, 0.f)},
      cabEnabled{createCabEnabledParameter(processor)} {}
}  // namespace faoxiamp
