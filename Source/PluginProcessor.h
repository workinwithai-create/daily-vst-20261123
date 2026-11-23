#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

struct Voice
{
    Voice() = default;

    void prepare (double sr)
    {
        sampleRate = sr;
        envelope.reset();
        filter.reset();
        osc1.reset();
        osc2.reset();
        vibPhase = 0.0f;
    }

    void noteOn (int midiNote, float vel, float atk, float dec, float sus, float rel)
    {
        note = midiNote;
        velocity = vel;
        frequency = juce::MidiMessage::getMidiNoteInHertz (midiNote);
        phase1 = 0.0f;
        phase2 = 0.0f;
        envelope.noteOn();
        isActive = true;
        isSustaining = false;
        currentAftertouch = 0.0f;
        attack = atk;
        decay = dec;
        sustain = sus;
        release = rel;
        envelope.setParameters ({ attack, decay, sustain, release });
    }

    void noteOff()
    {
        if (!isSustaining)
            envelope.noteOff();
    }

    void setSustain (bool sustainOn)
    {
        isSustaining = sustainOn;
        if (!sustainOn && envelope.getCurrentLevel() > 0.0f)
            envelope.noteOff();
    }

    void setAftertouch (float at)
    {
        currentAftertouch = at;
    }

    float process (float cutoff, float resonance, float detune, float vibRate, float vibDepth, float atSens)
    {
        if (!isActive)
            return 0.0f;

        auto env = envelope.getNextSample();
        if (env <= 0.0001f && !envelope.isActive())
        {
            isActive = false;
            return 0.0f;
        }

        // Vibrato
        vibPhase += (vibRate / static_cast<float> (sampleRate)) * juce::MathConstants<float>::twoPi;
        if (vibPhase > juce::MathConstants<float>::twoPi)
            vibPhase -= juce::MathConstants<float>::twoPi;
        float vib = std::sin (vibPhase) * vibDepth * (0.2f + currentAftertouch * atSens);

        float freq1 = frequency * (1.0f + vib);
        float freq2 = frequency * (1.0f + detune * 0.01f) * (1.0f + vib);

        phase1 += freq1 / static_cast<float> (sampleRate);
        phase2 += freq2 / static_cast<float> (sampleRate);
        if (phase1 >= 1.0f) phase1 -= 1.0f;
        if (phase2 >= 1.0f) phase2 -= 1.0f;

        float s1 = (phase1 < 0.5f ? 1.0f : -1.0f); // simple square-ish for string-like
        float s2 = (phase2 < 0.5f ? 1.0f : -1.0f);
        float sample = (s1 + s2 * 0.7f) * 0.5f * velocity * env;

        // Simple one-pole lowpass approximation + aftertouch filter mod
        float modCutoff = cutoff * (1.0f + currentAftertouch * atSens * 2.0f);
        modCutoff = juce::jlimit (20.0f, 18000.0f, modCutoff);
        float alpha = std::exp (-2.0f * juce::MathConstants<float>::pi * modCutoff / static_cast<float> (sampleRate));
        filterState = alpha * filterState + (1.0f - alpha) * sample;
        sample = filterState;

        // Resonance approximation (simple feedback)
        sample += resonance * 0.3f * (sample - lastSample);
        lastSample = sample;

        return sample;
    }

    bool isActive = false;
    int note = -1;
    float velocity = 0.0f;
    float frequency = 440.0f;
    float phase1 = 0.0f, phase2 = 0.0f;
    float vibPhase = 0.0f;
    float currentAftertouch = 0.0f;
    bool isSustaining = false;
    double sampleRate = 44100.0;
    juce::ADSR envelope;
    float attack = 0.1f, decay = 0.3f, sustain = 0.7f, release = 0.5f;
    float filterState = 0.0f;
    float lastSample = 0.0f;
    // unused but kept for structure
    juce::dsp::Oscillator<float> osc1, osc2;
    juce::dsp::StateVariableTPTFilter<float> filter;
};

class AutumnAftertouchEnsembleAudioProcessor : public juce::AudioProcessor
{
public:
    AutumnAftertouchEnsembleAudioProcessor();
    ~AutumnAftertouchEnsembleAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Autumn Aftertouch Ensemble"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    static constexpr int maxVoices = 8;
    std::array<Voice, maxVoices> voices;
    bool sustainPedalDown = false;
    float channelAftertouch = 0.0f;

    juce::AudioProcessorValueTreeState apvts;
    juce::dsp::Limiter<float> limiter;

    // Smoothed parameters
    juce::SmoothedValue<float> attackSmoothed, decaySmoothed, sustainSmoothed, releaseSmoothed;
    juce::SmoothedValue<float> cutoffSmoothed, resonanceSmoothed, detuneSmoothed;
    juce::SmoothedValue<float> vibRateSmoothed, vibDepthSmoothed, atSensSmoothed, volumeSmoothed;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutumnAftertouchEnsembleAudioProcessor)
};
