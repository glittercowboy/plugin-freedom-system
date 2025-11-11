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

    int getNumPrograms() override { return 0; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public access to parameters (needed by PluginEditor for WebView attachments)
    juce::AudioProcessorValueTreeState parameters;

    // Phase 5.2: Output Level Metering (public for PluginEditor access)
    std::atomic<float> outputLevel { -100.0f };  // Peak level in dB (initialized to silence)

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

    // Phase 4.3: Degradation Features (Dropout + Noise)
    int dropoutCountdown { 0 };  // Samples until next dropout check
    bool inDropout { false };  // Dropout state flag
    int dropoutSamplesRemaining { 0 };  // Current dropout duration
    float dropoutEnvelope { 1.0f };  // Smooth attack/release (1.0 = no attenuation)
    float noiseFilterState[2] { 0.0f, 0.0f };  // One-pole lowpass filter state per channel

    // Phase 4.4: Dry/Wet Mixing
    juce::dsp::DryWetMixer<float> dryWetMixer;

    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeAgeAudioProcessor)
};
