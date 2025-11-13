#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class MinimalKickAudioProcessor : public juce::AudioProcessor
{
public:
    MinimalKickAudioProcessor();
    ~MinimalKickAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MinimalKick"; }
    bool acceptsMidi() const override { return true; }  // Instrument - accepts MIDI
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:
    // DSP Components (declared BEFORE parameters for initialization order)
    juce::dsp::Oscillator<float> oscillator;
    juce::ADSR envelope;

    // Voice state
    bool isNoteOn { false };
    int currentNote { 60 };  // C4 default
    float currentFrequency { 0.0f };
    double sampleRate { 44100.0 };

    // APVTS comes AFTER DSP components
    juce::AudioProcessorValueTreeState parameters;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MinimalKickAudioProcessor)
};
