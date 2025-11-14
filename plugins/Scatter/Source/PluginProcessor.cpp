#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout ScatterAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // delay_time - Float (100.0 to 2000.0 ms, default: 500.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "delay_time", 1 },
        "Delay Time",
        juce::NormalisableRange<float>(100.0f, 2000.0f, 1.0f, 1.0f),
        500.0f,
        "ms"
    ));

    // grain_size - Float (5.0 to 500.0 ms, default: 100.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "grain_size", 1 },
        "Grain Size",
        juce::NormalisableRange<float>(5.0f, 500.0f, 1.0f, 1.0f),
        100.0f,
        "ms"
    ));

    // density - Float (0.0 to 100.0 %, default: 50.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "density", 1 },
        "Density",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        50.0f,
        "%"
    ));

    // pitch_random - Float (0.0 to 100.0 %, default: 30.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "pitch_random", 1 },
        "Pitch Random",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        30.0f,
        "%"
    ));

    // scale - Choice (Chromatic, Major, Minor, Pentatonic, Blues)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "scale", 1 },
        "Scale",
        juce::StringArray { "Chromatic", "Major", "Minor", "Pentatonic", "Blues" },
        0
    ));

    // root_note - Choice (C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "root_note", 1 },
        "Root Note",
        juce::StringArray { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" },
        0
    ));

    // pan_random - Float (0.0 to 100.0 %, default: 75.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "pan_random", 1 },
        "Pan Random",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        75.0f,
        "%"
    ));

    // feedback - Float (0.0 to 100.0 %, default: 30.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "feedback", 1 },
        "Feedback",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        30.0f,
        "%"
    ));

    // mix - Float (0.0 to 100.0 %, default: 50.0)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "mix", 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        50.0f,
        "%"
    ));

    return layout;
}

ScatterAudioProcessor::ScatterAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

ScatterAudioProcessor::~ScatterAudioProcessor()
{
}

void ScatterAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void ScatterAudioProcessor::releaseResources()
{
}

void ScatterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Parameter access example (for Stage 3 DSP implementation):
    // auto* delayTimeParam = parameters.getRawParameterValue("delay_time");
    // float delayTime = delayTimeParam->load();  // Atomic read (real-time safe)

    // Pass-through for Stage 2 (DSP implementation happens in Stage 3)
    // Audio routing is already handled by JUCE
}

juce::AudioProcessorEditor* ScatterAudioProcessor::createEditor()
{
    return new ScatterAudioProcessorEditor(*this);
}

void ScatterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ScatterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ScatterAudioProcessor();
}
