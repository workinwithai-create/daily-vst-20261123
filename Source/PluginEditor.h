#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class AutumnAftertouchEnsembleAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AutumnAftertouchEnsembleAudioProcessorEditor (AutumnAftertouchEnsembleAudioProcessor&);
    ~AutumnAftertouchEnsembleAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AutumnAftertouchEnsembleAudioProcessor& audioProcessor;

    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider;
    juce::Slider cutoffSlider, resonanceSlider, detuneSlider;
    juce::Slider vibRateSlider, vibDepthSlider, atSensSlider, volumeSlider;

    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;
    juce::Label cutoffLabel, resonanceLabel, detuneLabel;
    juce::Label vibRateLabel, vibDepthLabel, atSensLabel, volumeLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> attackAttach, decayAttach, sustainAttach, releaseAttach;
    std::unique_ptr<Attachment> cutoffAttach, resonanceAttach, detuneAttach;
    std::unique_ptr<Attachment> vibRateAttach, vibDepthAttach, atSensAttach, volumeAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutumnAftertouchEnsembleAudioProcessorEditor)
};
