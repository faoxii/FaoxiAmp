#pragma once

namespace faoxiamp {
/** All the user-facing parameters of the plugin (see the spec in README.md).
 *  They are created once, registered in the processor, and accessed through
 *  these references. */
struct Parameters {
  explicit Parameters(juce::AudioProcessor&);

  juce::AudioParameterFloat& gain;     // preamp drive knob, 0 to 10
  juce::AudioParameterFloat& bass;     // tone knob, 0 to 10 (5 = flat)
  juce::AudioParameterFloat& mid;      // tone knob, 0 to 10 (5 = flat)
  juce::AudioParameterFloat& treble;   // tone knob, 0 to 10 (5 = flat)
  juce::AudioParameterFloat& volume;   // output level, in dB
  juce::AudioParameterBool& cabEnabled;  // cabinet IR on/off

  JUCE_DECLARE_NON_COPYABLE(Parameters)
  JUCE_DECLARE_NON_MOVEABLE(Parameters)
};
}  // namespace faoxiamp
