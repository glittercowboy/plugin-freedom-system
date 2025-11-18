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
    // DSP initialization will be added in Stage 2
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void Spyder2000AudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 2
}

void Spyder2000AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Parameter access example (for future Stage 2 DSP implementation):
    // auto* gainParam = parameters.getRawParameterValue("gain");
    // float gainValue = gainParam->load();  // Atomic read (real-time safe)

    // Pass-through for Stage 1 (DSP implementation happens in Stage 2)
    // Audio routing is already handled by JUCE
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
