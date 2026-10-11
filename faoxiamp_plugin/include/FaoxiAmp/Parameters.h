#pragma once

namespace faoxiamp {
/** All the user-facing parameters of the plugin (see the spec in README.md).
 *  They are created once, registered in the processor, and accessed through
 *  these references. */
struct Parameters {
  explicit Parameters(juce::AudioProcessor&);

  juce::AudioParameterFloat& gain;     // input drive, in dB
  juce::AudioParameterFloat& bass;     // EQ low band, in dB
  juce::AudioParameterFloat& mid;      // EQ mid band, in dB
  juce::AudioParameterFloat& treble;   // EQ high band, in dB
  juce::AudioParameterFloat& volume;   // output level, in dB
  juce::AudioParameterBool& cabEnabled;  // cabinet IR on/off

  JUCE_DECLARE_NON_COPYABLE(Parameters)
  JUCE_DECLARE_NON_MOVEABLE(Parameters)
};
}  // namespace faoxiamp
