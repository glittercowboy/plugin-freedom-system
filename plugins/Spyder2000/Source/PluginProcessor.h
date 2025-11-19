#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Spyder2000AudioProcessor : public juce::AudioProcessor
{
public:
    Spyder2000AudioProcessor();
    ~Spyder2000AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Spyder 2000"; }
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

    juce::AudioProcessorValueTreeState parameters;

private:
    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // DSP Components (declare BEFORE parameters for initialization order)
    juce::dsp::ProcessSpec spec;

    // Adaptive pre-clipping filters (parallel topology)
    juce::dsp::IIR::Filter<float> trebleShelfL, trebleShelfR;
    juce::dsp::IIR::Filter<float> midrangePeakL, midrangePeakR;

    // Temporary buffers for parallel processing (preallocated for real-time safety)
    juce::AudioBuffer<float> trebleShelfBuffer;
    juce::AudioBuffer<float> midrangePeakBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spyder2000AudioProcessor)
};
