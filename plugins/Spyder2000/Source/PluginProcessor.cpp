#include "PluginProcessor.h"
#include "PluginEditor.h"

// Parameter layout function (BEFORE constructor)
juce::AudioProcessorValueTreeState::ParameterLayout Spyder2000AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // MINIMAL STAGE 1: Only gain parameter
    // tone and level will be added later
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "gain", 1 },  // ID + version (JUCE 8 requirement)
        "Gain",                            // Display name
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f),  // Range: 0-10, step 0.1
        5.0f                               // Default: 5.0
    ));

    return layout;
}

Spyder2000AudioProcessor::Spyder2000AudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

Spyder2000AudioProcessor::~Spyder2000AudioProcessor()
{
}

void Spyder2000AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize DSP spec
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Prepare adaptive pre-clipping filters
    trebleShelfL.prepare(spec);
    trebleShelfR.prepare(spec);
    midrangePeakL.prepare(spec);
    midrangePeakR.prepare(spec);

    // Reset filters to initial state
    trebleShelfL.reset();
    trebleShelfR.reset();
    midrangePeakL.reset();
    midrangePeakR.reset();

    // Preallocate temporary buffers for parallel processing (CRITICAL for real-time safety)
    trebleShelfBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);
    midrangePeakBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);

    // Initialize filter coefficients with default gain parameter value (5.0)
    // Treble shelf @ 2000 Hz, Q = 0.707
    auto trebleShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, 2000.0f, 0.707f, juce::Decibels::decibelsToGain(0.0f));
    *trebleShelfL.coefficients = *trebleShelfCoeffs;
    *trebleShelfR.coefficients = *trebleShelfCoeffs;

    // Midrange peak @ 800 Hz, Q = 1.5
    auto midrangePeakCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 800.0f, 1.5f, juce::Decibels::decibelsToGain(0.0f));
    *midrangePeakL.coefficients = *midrangePeakCoeffs;
    *midrangePeakR.coefficients = *midrangePeakCoeffs;
}

void Spyder2000AudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 2
}

void Spyder2000AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Clear unused channels
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Read gain parameter (atomic, real-time safe)
    auto* gainParam = parameters.getRawParameterValue("gain");
    float gainValue = gainParam->load();

    // Calculate crossfading weights (exponential curves)
    // Low gain (0-4): Treble shelf emphasized
    // High gain (6-10): Midrange peak emphasized
    float shelfWeight = std::exp(-gainValue * 0.8f);
    float peakWeight = 1.0f - std::exp(-(gainValue - 4.0f) * 0.8f);

    // Calculate filter gains based on crossfading
    // Treble shelf: +6dB max at gain=0, fades to 0dB at mid-range
    float trebleShelfGain_dB = shelfWeight * 6.0f;

    // Midrange peak: +9dB max at gain=10, fades from 0dB at low gain
    float midrangePeakGain_dB = peakWeight * 9.0f;

    // Update filter coefficients (per-block, not per-sample for efficiency)
    auto trebleShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        spec.sampleRate, 2000.0f, 0.707f, juce::Decibels::decibelsToGain(trebleShelfGain_dB));
    *trebleShelfL.coefficients = *trebleShelfCoeffs;
    *trebleShelfR.coefficients = *trebleShelfCoeffs;

    auto midrangePeakCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        spec.sampleRate, 800.0f, 1.5f, juce::Decibels::decibelsToGain(midrangePeakGain_dB));
    *midrangePeakL.coefficients = *midrangePeakCoeffs;
    *midrangePeakR.coefficients = *midrangePeakCoeffs;

    // Process audio through parallel filter topology
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Copy input to preallocated temporary buffers (real-time safe - no allocation)
    for (int channel = 0; channel < numChannels; ++channel)
    {
        trebleShelfBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
        midrangePeakBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }

    // Process left channel (channel 0)
    if (numChannels > 0)
    {
        // Treble shelf filter
        auto* trebleData = trebleShelfBuffer.getWritePointer(0);
        for (int sample = 0; sample < numSamples; ++sample)
            trebleData[sample] = trebleShelfL.processSample(trebleData[sample]);

        // Midrange peak filter
        auto* peakData = midrangePeakBuffer.getWritePointer(0);
        for (int sample = 0; sample < numSamples; ++sample)
            peakData[sample] = midrangePeakL.processSample(peakData[sample]);
    }

    // Process right channel (channel 1)
    if (numChannels > 1)
    {
        // Treble shelf filter
        auto* trebleData = trebleShelfBuffer.getWritePointer(1);
        for (int sample = 0; sample < numSamples; ++sample)
            trebleData[sample] = trebleShelfR.processSample(trebleData[sample]);

        // Midrange peak filter
        auto* peakData = midrangePeakBuffer.getWritePointer(1);
        for (int sample = 0; sample < numSamples; ++sample)
            peakData[sample] = midrangePeakR.processSample(peakData[sample]);
    }

    // Mix outputs with crossfading weights
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* outputData = buffer.getWritePointer(channel);
        const auto* trebleData = trebleShelfBuffer.getReadPointer(channel);
        const auto* peakData = midrangePeakBuffer.getReadPointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            outputData[sample] = (trebleData[sample] * shelfWeight) + (peakData[sample] * peakWeight);
        }
    }
}

juce::AudioProcessorEditor* Spyder2000AudioProcessor::createEditor()
{
    return new Spyder2000AudioProcessorEditor(*this);
}

void Spyder2000AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Spyder2000AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Spyder2000AudioProcessor();
}
