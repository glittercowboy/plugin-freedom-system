#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout FlutterVerbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // SIZE - Room dimensions
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "SIZE", 1 },
        "Size",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        50.0f,
        "%"
    ));

    // DECAY - Reverb tail length
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "DECAY", 1 },
        "Decay",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f, 1.0f),
        2.5f,
        "s"
    ));

    // MIX - Dry/wet blend
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "MIX", 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        25.0f,
        "%"
    ));

    // AGE - Tape character intensity
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "AGE", 1 },
        "Age",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        20.0f,
        "%"
    ));

    // DRIVE - Tape saturation
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "DRIVE", 1 },
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f, 1.0f),
        20.0f,
        "%"
    ));

    // TONE - DJ-style filter
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "TONE", 1 },
        "Tone",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f, 1.0f),
        0.0f,
        ""
    ));

    // MOD_MODE - Modulation routing
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "MOD_MODE", 1 },
        "Mod Mode",
        false  // Default: WET ONLY (0)
    ));

    return layout;
}

FlutterVerbAudioProcessor::FlutterVerbAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

FlutterVerbAudioProcessor::~FlutterVerbAudioProcessor()
{
}

void FlutterVerbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare DSP spec
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Phase 4.1: Prepare core reverb processing components
    dryWetMixer.prepare(spec);
    dryWetMixer.reset();

    // Prepare reverb with ProcessSpec
    reverb.prepare(spec);
    reverb.reset();
}

void FlutterVerbAudioProcessor::releaseResources()
{
    // DSP cleanup will be added in Stage 4
}

void FlutterVerbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Clear unused channels
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Phase 4.1: Read SIZE, DECAY, MIX parameters (atomic, real-time safe)
    auto* sizeParam = parameters.getRawParameterValue("SIZE");
    auto* decayParam = parameters.getRawParameterValue("DECAY");
    auto* mixParam = parameters.getRawParameterValue("MIX");

    float sizeValue = sizeParam->load() / 100.0f;  // 0-100% → 0.0-1.0
    float decayValue = decayParam->load();          // 0.1-10.0 seconds
    float mixValue = mixParam->load() / 100.0f;     // 0-100% → 0.0-1.0

    // Configure reverb parameters
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = sizeValue;

    // Map decay time to damping (inverse relationship)
    // Short decay → high damping, long decay → low damping
    // Empirical mapping: decay 0.1s → damping 0.9, decay 10s → damping 0.1
    reverbParams.damping = juce::jmap(decayValue, 0.1f, 10.0f, 0.9f, 0.1f);

    reverbParams.width = 1.0f;         // Full stereo
    reverbParams.freezeMode = 0.0f;    // No freeze
    reverbParams.wetLevel = 1.0f;      // Full wet (mixer handles blend)
    reverbParams.dryLevel = 0.0f;      // No dry (mixer handles blend)

    reverb.setParameters(reverbParams);

    // Set dry/wet mix proportion
    dryWetMixer.setWetMixProportion(mixValue);

    // Process audio with DSP pipeline
    juce::dsp::AudioBlock<float> block(buffer);

    // Push dry samples before processing
    dryWetMixer.pushDrySamples(block);

    // Process reverb using modern DSP API
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);

    // Mix dry and wet samples
    dryWetMixer.mixWetSamples(block);
}

juce::AudioProcessorEditor* FlutterVerbAudioProcessor::createEditor()
{
    return new FlutterVerbAudioProcessorEditor(*this);
}

void FlutterVerbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void FlutterVerbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlutterVerbAudioProcessor();
}
