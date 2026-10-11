#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "FaoxiAmp/InputGain.h"
#include "FaoxiAmp/Parameters.h"

namespace faoxiamp {
class PluginProcessor : public juce::AudioProcessor {
public:
  PluginProcessor();

  void prepareToPlay(double sampleRate, int expectedMaxFramesPerBlock) override;
  void releaseResources() override;

  void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
  using AudioProcessor::processBlock;

  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const juce::String getProgramName(int index) override;
  void changeProgramName(int index, const juce::String& newName) override;

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;

  [[nodiscard]] Parameters& getParameterRefs() noexcept;

private:
  Parameters parameters{*this};
  InputGain inputGain;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};
}  // namespace faoxiamp
