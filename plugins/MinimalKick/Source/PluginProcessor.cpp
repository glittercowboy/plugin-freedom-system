#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout MinimalKickAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // sweep - Pitch envelope amount (0.0 to 24.0 semitones, linear)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "sweep", 1 },
        "Sweep",
        juce::NormalisableRange<float>(0.0f, 24.0f, 0.1f),
        12.0f,
        "st"
    ));

    // time - Pitch envelope decay time (5.0 to 500.0 ms, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "time", 1 },
        "Time",
        juce::NormalisableRange<float>(5.0f, 500.0f, 0.1f, 0.3f),
        50.0f,
        "ms"
    ));

    // attack - Amplitude envelope attack time (0.0 to 50.0 ms, linear)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "attack", 1 },
        "Attack",
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f),
        5.0f,
        "ms"
    ));

    // decay - Amplitude envelope decay time (50.0 to 2000.0 ms, logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "decay", 1 },
        "Decay",
        juce::NormalisableRange<float>(50.0f, 2000.0f, 1.0f, 0.3f),
        400.0f,
        "ms"
    ));

    // drive - Saturation/distortion amount (0.0 to 100.0%, linear)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "drive", 1 },
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        20.0f,
        "%"
    ));

    return layout;
}

MinimalKickAudioProcessor::MinimalKickAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
    // Initialize oscillator with sine wave
    oscillator.initialise([](float x) { return std::sin(x); }, 128);
}

MinimalKickAudioProcessor::~MinimalKickAudioProcessor()
{
}

void MinimalKickAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;

    // Prepare oscillator
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    oscillator.prepare(spec);
    oscillator.reset();

    // Reset envelope
    envelope.reset();
}

void MinimalKickAudioProcessor::releaseResources()
{
    // No buffers to release (oscillator and envelope are self-managing)
}

void MinimalKickAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear buffer (instrument starts with silence)
    buffer.clear();

    // Read parameters (atomic, real-time safe)
    auto* attackParam = parameters.getRawParameterValue("attack");
    auto* decayParam = parameters.getRawParameterValue("decay");

    float attackMs = attackParam->load();
    float decayMs = decayParam->load();

    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            // Store note and convert to frequency
            currentNote = message.getNoteNumber();
            currentFrequency = juce::MidiMessage::getMidiNoteInHertz(currentNote);

            // Set oscillator frequency
            oscillator.setFrequency(currentFrequency);

            // Reset oscillator phase for consistent attack
            oscillator.reset();

            // Configure and trigger envelope
            juce::ADSR::Parameters envParams;
            envParams.attack = attackMs / 1000.0f;     // Convert ms to seconds
            envParams.decay = decayMs / 1000.0f;       // Convert ms to seconds
            envParams.sustain = 0.0f;                   // Fixed for kick drums
            envParams.release = 0.0f;                   // Not needed (sustain=0)

            envelope.setParameters(envParams);
            envelope.noteOn();

            isNoteOn = true;
        }
        else if (message.isNoteOff())
        {
            // Note-off can be ignored (sustain=0, envelope decays naturally)
            isNoteOn = false;
        }
    }

    // Generate audio if envelope is active
    const int numSamples = buffer.getNumSamples();

    if (envelope.isActive())
    {
        // Process mono (oscillator generates single channel)
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Generate sine wave sample
            float oscillatorSample = oscillator.processSample(0.0f);

            // Apply amplitude envelope
            float envelopeValue = envelope.getNextSample();
            float outputSample = oscillatorSample * envelopeValue;

            // Write to both channels (mono to stereo)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                buffer.setSample(channel, sample, outputSample);
            }
        }
    }
}

juce::AudioProcessorEditor* MinimalKickAudioProcessor::createEditor()
{
    return new MinimalKickAudioProcessorEditor(*this);
}

void MinimalKickAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MinimalKickAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MinimalKickAudioProcessor();
}
