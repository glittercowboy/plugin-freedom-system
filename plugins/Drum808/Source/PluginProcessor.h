#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Drum808AudioProcessor : public juce::AudioProcessor
{
public:
    Drum808AudioProcessor();
    ~Drum808AudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Drum808"; }
    bool acceptsMidi() const override { return true; }
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
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Tom Voice structure (used for both Low Tom and Mid Tom)
    struct TomVoice
    {
        juce::dsp::Oscillator<float> oscillator;
        juce::dsp::StateVariableTPTFilter<float> filter;

        bool isPlaying = false;
        float envelopeTime = 0.0f;
        float velocity = 0.0f;

        void trigger(float velocityGain, float baseFreq)
        {
            isPlaying = true;
            envelopeTime = 0.0f;
            velocity = velocityGain;
            oscillator.setFrequency(baseFreq);
            filter.setCutoffFrequency(baseFreq);
        }

        void stop()
        {
            isPlaying = false;
            envelopeTime = 0.0f;
        }
    };

    // Kick Voice structure
    struct KickVoice
    {
        juce::dsp::Oscillator<float> bodyOscillator;
        juce::Random noiseGenerator;

        bool isPlaying = false;
        float envelopeTime = 0.0f;
        float velocity = 0.0f;

        void trigger(float velocityGain)
        {
            isPlaying = true;
            envelopeTime = 0.0f;
            velocity = velocityGain;
        }

        void stop()
        {
            isPlaying = false;
            envelopeTime = 0.0f;
        }
    };

    // Hi-Hat Voice structure (shared by Closed and Open)
    struct HiHatVoice
    {
        // 6 square wave oscillators for metallic inharmonic spectrum
        juce::dsp::Oscillator<float> oscillators[6];
        juce::dsp::StateVariableTPTFilter<float> filter;

        bool isPlaying = false;
        float envelopeTime = 0.0f;
        float velocity = 0.0f;

        void trigger(float velocityGain)
        {
            isPlaying = true;
            envelopeTime = 0.0f;
            velocity = velocityGain;
        }

        void stop()
        {
            isPlaying = false;
            envelopeTime = 0.0f;
        }
    };

    // Clap Voice structure (multi-trigger envelope + filtered noise)
    enum class ClapEnvelopeState { Spike1, Spike2, Spike3, Decay, Idle };

    struct ClapVoice
    {
        juce::dsp::StateVariableTPTFilter<float> bandpassFilter;
        ClapEnvelopeState envelopeState = ClapEnvelopeState::Idle;
        int envelopeSample = 0;
        float velocity = 0.0f;
        bool isPlaying = false;

        // Sample-rate independent timing (calculated in prepareToPlay)
        int spike2StartSample = 0;
        int spike3StartSample = 0;
        int decayStartSample = 0;

        void trigger(float velocityGain)
        {
            isPlaying = true;
            envelopeState = ClapEnvelopeState::Spike1;
            envelopeSample = 0;
            velocity = velocityGain;
        }

        void stop()
        {
            isPlaying = false;
            envelopeState = ClapEnvelopeState::Idle;
            envelopeSample = 0;
        }
    };

    // DSP Components (BEFORE APVTS for initialization order)
    juce::dsp::ProcessSpec spec;
    TomVoice lowTom;
    TomVoice midTom;
    KickVoice kick;
    HiHatVoice closedHat;
    HiHatVoice openHat;
    ClapVoice clap;

    double currentSampleRate = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Drum808AudioProcessor)
};
