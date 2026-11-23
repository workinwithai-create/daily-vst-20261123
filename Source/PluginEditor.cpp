#include "PluginEditor.h"

AutumnAftertouchEnsembleAudioProcessorEditor::AutumnAftertouchEnsembleAudioProcessorEditor (AutumnAftertouchEnsembleAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Dark cinematic UI notes: deep black background, amber/gold accents, subtle film-like grain suggestion via contrast.
    auto setupSlider = [this] (juce::Slider& s, juce::Label& l, const juce::String& name, const juce::String& paramId)
    {
        addAndMakeVisible (s);
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
        s.setColour (juce::Slider::thumbColourId, juce::Colour (0xffd4a574)); // amber
        s.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff3a2a1a));
        s.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff1a1a1a));

        addAndMakeVisible (l);
        l.setText (name, juce::dontSendNotification);
        l.setColour (juce::Label::textColourId, juce::Colour (0xffc0a080));
        l.setJustificationType (juce::Justification::centred);
        l.attachToComponent (&s, false);
    };

    setupSlider (attackSlider, attackLabel, "Attack", "attack");
    setupSlider (decaySlider, decayLabel, "Decay", "decay");
    setupSlider (sustainSlider, sustainLabel, "Sustain", "sustain");
    setupSlider (releaseSlider, releaseLabel, "Release", "release");
    setupSlider (cutoffSlider, cutoffLabel, "Cutoff", "cutoff");
    setupSlider (resonanceSlider, resonanceLabel, "Resonance", "resonance");
    setupSlider (detuneSlider, detuneLabel, "Detune", "detune");
    setupSlider (vibRateSlider, vibRateLabel, "Vib Rate", "vibRate");
    setupSlider (vibDepthSlider, vibDepthLabel, "Vib Depth", "vibDepth");
    setupSlider (atSensSlider, atSensLabel, "AT Sens", "atSens");
    setupSlider (volumeSlider, volumeLabel, "Volume", "volume");

    auto& apvts = audioProcessor.getAPVTS();
    attackAttach = std::make_unique<Attachment> (apvts, "attack", attackSlider);
    decayAttach = std::make_unique<Attachment> (apvts, "decay", decaySlider);
    sustainAttach = std::make_unique<Attachment> (apvts, "sustain", sustainSlider);
    releaseAttach = std::make_unique<Attachment> (apvts, "release", releaseSlider);
    cutoffAttach = std::make_unique<Attachment> (apvts, "cutoff", cutoffSlider);
    resonanceAttach = std::make_unique<Attachment> (apvts, "resonance", resonanceSlider);
    detuneAttach = std::make_unique<Attachment> (apvts, "detune", detuneSlider);
    vibRateAttach = std::make_unique<Attachment> (apvts, "vibRate", vibRateSlider);
    vibDepthAttach = std::make_unique<Attachment> (apvts, "vibDepth", vibDepthSlider);
    atSensAttach = std::make_unique<Attachment> (apvts, "atSens", atSensSlider);
    volumeAttach = std::make_unique<Attachment> (apvts, "volume", volumeSlider);

    setSize (720, 420);
}

AutumnAftertouchEnsembleAudioProcessorEditor::~AutumnAftertouchEnsembleAudioProcessorEditor() = default;

void AutumnAftertouchEnsembleAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Dark cinematic background
    g.fillAll (juce::Colour (0xff0a0a0c));

    g.setColour (juce::Colour (0xff1e1a16));
    g.fillRoundedRectangle (getLocalBounds().toFloat().reduced (8.0f), 12.0f);

    g.setColour (juce::Colour (0xffd4a574));
    g.setFont (juce::FontOptions (22.0f, juce::Font::bold));
    g.drawText ("Autumn Aftertouch Ensemble", 20, 16, getWidth() - 40, 30, juce::Justification::centred);

    g.setColour (juce::Colour (0xff8a7050));
    g.setFont (juce::FontOptions (12.0f));
    g.drawText ("Aftertouch deepens vibrato & filter  |  Sustain pedal holds resonant tails", 20, 44, getWidth() - 40, 20, juce::Justification::centred);
}

void AutumnAftertouchEnsembleAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (70); // title space

    const int sliderW = 90;
    const int sliderH = 100;
    const int gap = 12;

    auto row1 = area.removeFromTop (sliderH);
    attackSlider.setBounds (row1.removeFromLeft (sliderW));
    row1.removeFromLeft (gap);
    decaySlider.setBounds (row1.removeFromLeft (sliderW));
    row1.removeFromLeft (gap);
    sustainSlider.setBounds (row1.removeFromLeft (sliderW));
    row1.removeFromLeft (gap);
    releaseSlider.setBounds (row1.removeFromLeft (sliderW));

    area.removeFromTop (20);
    auto row2 = area.removeFromTop (sliderH);
    cutoffSlider.setBounds (row2.removeFromLeft (sliderW));
    row2.removeFromLeft (gap);
    resonanceSlider.setBounds (row2.removeFromLeft (sliderW));
    row2.removeFromLeft (gap);
    detuneSlider.setBounds (row2.removeFromLeft (sliderW));
    row2.removeFromLeft (gap);
    volumeSlider.setBounds (row2.removeFromLeft (sliderW));

    area.removeFromTop (20);
    auto row3 = area.removeFromTop (sliderH);
    vibRateSlider.setBounds (row3.removeFromLeft (sliderW));
    row3.removeFromLeft (gap);
    vibDepthSlider.setBounds (row3.removeFromLeft (sliderW));
    row3.removeFromLeft (gap);
    atSensSlider.setBounds (row3.removeFromLeft (sliderW));
}
