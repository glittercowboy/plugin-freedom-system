#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class TapeAgeAudioProcessor : public juce::AudioProcessor
{
public:
    TapeAgeAudioProcessor();
    ~TapeAgeAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "TAPE AGE"; }
    bool acceptsMidi() const override { return false; }
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
    juce::dsp::ProcessSpec currentSpec;

    // Phase 4.1: Core Saturation Processing
    juce::dsp::Oversampling<float> oversampler { 2, 1, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple };

    // Phase 4.2: Wow/Flutter Modulation
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine;
    float lfoPhase[2] { 0.0f, 0.0f };  // Separate phase per channel for stereo width
    juce::Random random;
    double currentSampleRate { 44100.0 };

    // APVTS comes AFTER DSP components
    juce::AudioProcessorValueTreeState parameters;

    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeAgeAudioProcessor)
};
