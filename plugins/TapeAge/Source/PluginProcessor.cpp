#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout TapeAgeAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // drive - Tape saturation amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "drive", 1 },
        "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),  // 0-100%, linear
        0.5f  // Default: 50%
    ));

    // age - Tape degradation amount
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "age", 1 },
        "Age",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),  // 0-100%, linear
        0.25f  // Default: 25%
    ));

    // mix - Dry/wet blend
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "mix", 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 1.0f),  // 0-100%, linear
        1.0f  // Default: 100% wet
    ));

    return layout;
}

TapeAgeAudioProcessor::TapeAgeAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , oversampler(2, 1, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple)  // 2x oversampling, 1 stage, FIR filters
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

TapeAgeAudioProcessor::~TapeAgeAudioProcessor()
{
}

void TapeAgeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare DSP spec
    currentSpec.sampleRate = sampleRate;
    currentSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    currentSpec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    currentSampleRate = sampleRate;

    // Phase 4.1: Prepare oversampling engine
    oversampler.initProcessing(static_cast<size_t>(samplesPerBlock));
    oversampler.reset();

    // Phase 4.2: Prepare wow/flutter modulation
    // 200ms delay line buffer for pitch modulation (architecture.md line 28)
    int delaySamples = static_cast<int>(sampleRate * 0.2);
    delayLine.setMaximumDelayInSamples(delaySamples);
    delayLine.prepare(currentSpec);
    delayLine.reset();

    // Initialize random phase offsets per channel for stereo width
    lfoPhase[0] = random.nextFloat() * juce::MathConstants<float>::twoPi;
    lfoPhase[1] = random.nextFloat() * juce::MathConstants<float>::twoPi;
}

void TapeAgeAudioProcessor::releaseResources()
{
    // Phase 4.1: Reset DSP components
    oversampler.reset();

    // Phase 4.2: Reset wow/flutter modulation
    delayLine.reset();
}

void TapeAgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Clear unused channels
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Phase 4.1: Core Saturation Processing
    // Processing chain:
    // 1. Read drive parameter and calculate gain
    // 2. Upsample 2x
    // 3. Apply tanh saturation manually (drive controls gain scaling)
    // 4. Downsample

    // Read drive parameter (0.0 to 1.0)
    auto* driveParam = parameters.getRawParameterValue("drive");
    float drive = driveParam->load();

    // Progressive curve mapping (architecture.md):
    // 0-30%: Very subtle (multiply by 1-2 before tanh)
    // 30-70%: Moderate warmth (multiply by 2-8)
    // 70-100%: Heavy saturation (multiply by 8-20)
    float gain;
    if (drive <= 0.3f)
    {
        // Subtle range: linear interpolation from 1 to 2
        gain = 1.0f + (drive / 0.3f) * 1.0f;
    }
    else if (drive <= 0.7f)
    {
        // Moderate range: linear interpolation from 2 to 8
        gain = 2.0f + ((drive - 0.3f) / 0.4f) * 6.0f;
    }
    else
    {
        // Heavy range: linear interpolation from 8 to 20
        gain = 8.0f + ((drive - 0.7f) / 0.3f) * 12.0f;
    }

    // Create AudioBlock for DSP processing
    juce::dsp::AudioBlock<float> block(buffer);

    // Upsample
    auto oversampledBlock = oversampler.processSamplesUp(block);

    // Apply tanh saturation manually in oversampled domain
    for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel)
    {
        auto* channelData = oversampledBlock.getChannelPointer(channel);
        for (size_t sample = 0; sample < oversampledBlock.getNumSamples(); ++sample)
        {
            channelData[sample] = std::tanh(gain * channelData[sample]);
        }
    }

    // Downsample back to original sample rate
    oversampler.processSamplesDown(block);

    // Phase 4.2: Wow/Flutter Modulation
    // Processing chain: Apply pitch modulation via delay line after saturation
    // Read age parameter (0.0 to 1.0)
    auto* ageParam = parameters.getRawParameterValue("age");
    float age = ageParam->load();

    // Calculate LFO modulation depth based on age
    // Architecture.md: ±10 cents at max age (pitch ratio = 2^(cents/1200))
    // ±10 cents = 2^(10/1200) = 1.005777895 (~0.58% pitch variation)
    const float maxPitchVariationCents = 10.0f;
    const float pitchVariationRatio = std::pow(2.0f, maxPitchVariationCents / 1200.0f) - 1.0f;  // ~0.005777895
    float modulationDepth = age * pitchVariationRatio;

    // LFO frequency: 0.5-2Hz (architecture.md line 29)
    // Use 1.0Hz as base frequency, scaled by age for subtle variation
    const float lfoFrequency = 1.0f + age;  // 1.0-2.0Hz range
    const float lfoPhaseIncrement = (lfoFrequency * juce::MathConstants<float>::twoPi) / static_cast<float>(currentSampleRate);

    // Process each channel
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Calculate LFO modulation (sine wave)
            float lfoValue = std::sin(lfoPhase[channel]);

            // Calculate delay time in samples
            // Base delay at center of buffer (100ms) + modulation
            float baseDelaySamples = static_cast<float>(currentSampleRate) * 0.1f;  // 100ms center
            float modulationSamples = lfoValue * modulationDepth * baseDelaySamples;
            float totalDelay = baseDelaySamples + modulationSamples;

            // Push input sample to delay line
            delayLine.pushSample(channel, channelData[sample]);

            // Read modulated sample from delay line
            channelData[sample] = delayLine.popSample(channel, totalDelay);

            // Advance LFO phase
            lfoPhase[channel] += lfoPhaseIncrement;
            if (lfoPhase[channel] >= juce::MathConstants<float>::twoPi)
                lfoPhase[channel] -= juce::MathConstants<float>::twoPi;
        }
    }
}

juce::AudioProcessorEditor* TapeAgeAudioProcessor::createEditor()
{
    return new TapeAgeAudioProcessorEditor(*this);
}

void TapeAgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TapeAgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapeAgeAudioProcessor();
}
