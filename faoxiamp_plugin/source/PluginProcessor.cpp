#include "FaoxiAmp/PluginProcessor.h"

#include <ranges>

namespace faoxiamp {
PluginProcessor::PluginProcessor()
    : AudioProcessor(
          BusesProperties()
              // A guitar is a mono source...
              .withInput("Input", juce::AudioChannelSet::mono(), true)
              // ...but the amp outputs stereo (same signal on both sides).
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)) {}

const juce::String PluginProcessor::getName() const {
  return FAOXIAMP_PLUGIN_NAME;
}

bool PluginProcessor::acceptsMidi() const {
  return false;
}

bool PluginProcessor::producesMidi() const {
  return false;
}

bool PluginProcessor::isMidiEffect() const {
  return false;
}

double PluginProcessor::getTailLengthSeconds() const {
  // Will become the cabinet IR length once the convolution is in.
  return 0.0;
}

int PluginProcessor::getNumPrograms() {
  // Some hosts don't cope well with 0 programs, so this must be at least 1.
  return 1;
}

int PluginProcessor::getCurrentProgram() {
  return 0;
}

void PluginProcessor::setCurrentProgram(int index) {
  juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index) {
  juce::ignoreUnused(index);
  return "None";
}

void PluginProcessor::changeProgramName(int index,
                                        const juce::String& newName) {
  juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate,
                                    int expectedMaxFramesPerBlock) {
  // TODO(Mathis): prepare the rest of the chain (waveshaper, EQ, cab, volume)
  juce::ignoreUnused(expectedMaxFramesPerBlock);
  inputGain.prepare(sampleRate);
}

void PluginProcessor::releaseResources() {
  // TODO(Mathis): reset the DSP chain here
}

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  const auto input = layouts.getMainInputChannelSet();
  const auto output = layouts.getMainOutputChannelSet();

  // Supported: mono -> stereo (guitar track), stereo -> stereo, mono -> mono.
  const auto monoToStereo = input == juce::AudioChannelSet::mono() &&
                            output == juce::AudioChannelSet::stereo();
  const auto sameLayout =
      input == output && (output == juce::AudioChannelSet::mono() ||
                          output == juce::AudioChannelSet::stereo());

  return monoToStereo || sameLayout;
}

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                   juce::MidiBuffer& midiMessages) {
  juce::ignoreUnused(midiMessages);
  juce::ScopedNoDenormals noDenormals;

  const auto numInputChannels = getTotalNumInputChannels();
  const auto numOutputChannels = getTotalNumOutputChannels();
  const auto numSamples = buffer.getNumSamples();

  // TODO(Mathis): add the rest of the chain (waveshaper, EQ, cab, volume)
  inputGain.setKnob(parameters.gain.get());

  inputGain.process(buffer);

  // Mono in, stereo out: the processed signal of channel 0 is copied to the
  // other output channels. Identical copies are perfectly in phase.
  if (numInputChannels == 1) {
    for (const auto channel : std::views::iota(1, numOutputChannels)) {
      buffer.copyFrom(channel, 0, buffer, 0, 0, numSamples);
    }
  }
}

bool PluginProcessor::hasEditor() const {
  return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() {
  // Temporary UI: JUCE draws one slider per parameter automatically.
  // The custom UI comes in the week of 26/10.
  return new juce::GenericAudioProcessorEditor(*this);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData) {
  // Saves every parameter (by ID) so Logic restores them with the session.
  juce::XmlElement state{"FaoxiAmpState"};
  for (auto* parameter : getParameters()) {
    if (auto* withId =
            dynamic_cast<juce::AudioProcessorParameterWithID*>(parameter)) {
      state.setAttribute(withId->paramID, parameter->getValue());
    }
  }
  copyXmlToBinary(state, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) {
  const auto state = getXmlFromBinary(data, sizeInBytes);
  if (state == nullptr || !state->hasTagName("FaoxiAmpState")) {
    return;
  }

  for (auto* parameter : getParameters()) {
    if (auto* withId =
            dynamic_cast<juce::AudioProcessorParameterWithID*>(parameter)) {
      if (state->hasAttribute(withId->paramID)) {
        parameter->setValueNotifyingHost(static_cast<float>(
            state->getDoubleAttribute(withId->paramID)));
      }
    }
  }
}

Parameters& PluginProcessor::getParameterRefs() noexcept {
  return parameters;
}
}  // namespace faoxiamp

// Creates new instances of the plugin.
// This function definition must be in the global namespace.
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new faoxiamp::PluginProcessor();
}
