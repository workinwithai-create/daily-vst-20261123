#include "PluginProcessor.h"
#include "PluginEditor.h"

AutumnAftertouchEnsembleAudioProcessor::AutumnAftertouchEnsembleAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

AutumnAftertouchEnsembleAudioProcessor::~AutumnAftertouchEnsembleAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout AutumnAftertouchEnsembleAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> ("attack", "Attack", 0.001f, 2.0f, 0.05f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("decay", "Decay", 0.001f, 2.0f, 0.3f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("sustain", "Sustain", 0.0f, 1.0f, 0.7f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("release", "Release", 0.001f, 5.0f, 0.8f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("cutoff", "Cutoff", 100.0f, 8000.0f, 2000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("resonance", "Resonance", 0.0f, 1.0f, 0.2f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("detune", "Detune", 0.0f, 50.0f, 8.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("vibRate", "Vib Rate", 0.1f, 10.0f, 5.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("vibDepth", "Vib Depth", 0.0f, 0.05f, 0.008f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("atSens", "Aftertouch Sens", 0.0f, 1.0f, 0.6f));
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("volume", "Volume", 0.0f, 1.0f, 0.7f));

    return { params.begin(), params.end() };
}

void AutumnAftertouchEnsembleAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (auto& v : voices)
        v.prepare (sampleRate);

    limiter.prepare ({ sampleRate, static_cast<juce::uint32> (samplesPerBlock), 2 });
    limiter.setThreshold (-1.0f);
    limiter.setRelease (50.0f);

    const double ramp = 0.05;
    attackSmoothed.reset (sampleRate, ramp);
    decaySmoothed.reset (sampleRate, ramp);
    sustainSmoothed.reset (sampleRate, ramp);
    releaseSmoothed.reset (sampleRate, ramp);
    cutoffSmoothed.reset (sampleRate, ramp);
    resonanceSmoothed.reset (sampleRate, ramp);
    detuneSmoothed.reset (sampleRate, ramp);
    vibRateSmoothed.reset (sampleRate, ramp);
    vibDepthSmoothed.reset (sampleRate, ramp);
    atSensSmoothed.reset (sampleRate, ramp);
    volumeSmoothed.reset (sampleRate, ramp);

    attackSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("attack"));
    decaySmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("decay"));
    sustainSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("sustain"));
    releaseSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("release"));
    cutoffSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("cutoff"));
    resonanceSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("resonance"));
    detuneSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("detune"));
    vibRateSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("vibRate"));
    vibDepthSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("vibDepth"));
    atSensSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("atSens"));
    volumeSmoothed.setCurrentAndTargetValue (*apvts.getRawParameterValue ("volume"));
}

void AutumnAftertouchEnsembleAudioProcessor::releaseResources() {}

void AutumnAftertouchEnsembleAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Update smoothed targets
    attackSmoothed.setTargetValue (*apvts.getRawParameterValue ("attack"));
    decaySmoothed.setTargetValue (*apvts.getRawParameterValue ("decay"));
    sustainSmoothed.setTargetValue (*apvts.getRawParameterValue ("sustain"));
    releaseSmoothed.setTargetValue (*apvts.getRawParameterValue ("release"));
    cutoffSmoothed.setTargetValue (*apvts.getRawParameterValue ("cutoff"));
    resonanceSmoothed.setTargetValue (*apvts.getRawParameterValue ("resonance"));
    detuneSmoothed.setTargetValue (*apvts.getRawParameterValue ("detune"));
    vibRateSmoothed.setTargetValue (*apvts.getRawParameterValue ("vibRate"));
    vibDepthSmoothed.setTargetValue (*apvts.getRawParameterValue ("vibDepth"));
    atSensSmoothed.setTargetValue (*apvts.getRawParameterValue ("atSens"));
    volumeSmoothed.setTargetValue (*apvts.getRawParameterValue ("volume"));

    for (const auto metadata : midi)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            int free = -1;
            for (int i = 0; i < maxVoices; ++i)
                if (!voices[i].isActive) { free = i; break; }
            if (free >= 0)
                voices[free].noteOn (msg.getNoteNumber(), msg.getFloatVelocity(),
                                     attackSmoothed.getCurrentValue(),
                                     decaySmoothed.getCurrentValue(),
                                     sustainSmoothed.getCurrentValue(),
                                     releaseSmoothed.getCurrentValue());
        }
        else if (msg.isNoteOff())
        {
            for (auto& v : voices)
                if (v.isActive && v.note == msg.getNoteNumber())
                    v.noteOff();
        }
        else if (msg.isController() && msg.getControllerNumber() == 64)
        {
            sustainPedalDown = msg.getControllerValue() >= 64;
            for (auto& v : voices)
                v.setSustain (sustainPedalDown);
        }
        else if (msg.isChannelPressure())
        {
            channelAftertouch = msg.getChannelPressureValue() / 127.0f;
            for (auto& v : voices)
                v.setAftertouch (channelAftertouch);
        }
    }

    const int numSamples = buffer.getNumSamples();
    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : left;

    for (int s = 0; s < numSamples; ++s)
    {
        float mix = 0.0f;
        float atk = attackSmoothed.getNextValue();
        float dec = decaySmoothed.getNextValue();
        float sus = sustainSmoothed.getNextValue();
        float rel = releaseSmoothed.getNextValue();
        float cut = cutoffSmoothed.getNextValue();
        float res = resonanceSmoothed.getNextValue();
        float det = detuneSmoothed.getNextValue();
        float vr = vibRateSmoothed.getNextValue();
        float vd = vibDepthSmoothed.getNextValue();
        float ats = atSensSmoothed.getNextValue();
        float vol = volumeSmoothed.getNextValue();

        for (auto& v : voices)
        {
            if (v.isActive)
            {
                // update env params if changed (simple)
                v.attack = atk; v.decay = dec; v.sustain = sus; v.release = rel;
                v.envelope.setParameters ({ atk, dec, sus, rel });
                mix += v.process (cut, res, det, vr, vd, ats);
            }
        }

        mix *= vol * 0.3f; // headroom
        left[s] = mix;
        right[s] = mix;
    }

    // Safety limiter
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> ctx (block);
    limiter.process (ctx);
}

juce::AudioProcessorEditor* AutumnAftertouchEnsembleAudioProcessor::createEditor()
{
    return new AutumnAftertouchEnsembleAudioProcessorEditor (*this);
}

void AutumnAftertouchEnsembleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AutumnAftertouchEnsembleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutumnAftertouchEnsembleAudioProcessor();
}
