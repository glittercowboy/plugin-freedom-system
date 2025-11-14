#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout LushPadAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // timbre - Float (0.0 to 1.0, default: 0.35, linear)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "timbre", 1 },
        "Timbre",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
        0.35f
    ));

    // filter_cutoff - Float (20.0 to 20000.0 Hz, default: 2000.0, skew: 0.3 logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filter_cutoff", 1 },
        "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f),
        2000.0f,
        "Hz"
    ));

    // reverb_amount - Float (0.0 to 1.0, default: 0.4, linear)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "reverb_amount", 1 },
        "Reverb Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
        0.4f
    ));

    return layout;
}

LushPadAudioProcessor::LushPadAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

LushPadAudioProcessor::~LushPadAudioProcessor()
{
}

void LushPadAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    // Prepare DSP spec for stereo reverb
    juce::dsp::ProcessSpec reverbSpec;
    reverbSpec.sampleRate = sampleRate;
    reverbSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    reverbSpec.numChannels = 2;  // Stereo

    // Prepare and configure reverb
    reverb.prepare(reverbSpec);
    reverb.reset();

    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.9f;     // Large hall
    reverbParams.damping = 0.4f;      // Moderate high-frequency rolloff
    reverbParams.wetLevel = 0.4f;     // Default wet level (overridden by parameter)
    reverbParams.dryLevel = 0.6f;     // Default dry level (overridden by parameter)
    reverbParams.width = 1.0f;        // Full stereo width
    reverbParams.freezeMode = 0.0f;   // No freeze
    reverb.setParameters(reverbParams);

    // Prepare DSP spec for mono per-voice filtering
    juce::dsp::ProcessSpec voiceSpec;
    voiceSpec.sampleRate = sampleRate;
    voiceSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    voiceSpec.numChannels = 1;  // Mono per-voice

    // Initialize all voices with filter preparation
    for (auto& voice : voices)
    {
        voice.adsr.setSampleRate(sampleRate);
        voice.filter.prepare(voiceSpec);
        voice.reset();
    }
}

void LushPadAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 3 (DSP)
}

void LushPadAudioProcessor::updateVoiceLFOs(SynthVoice& voice)
{
    // Update tertiary LFOs first (indices 6-8) - slowest layer, modulate primary depths
    for (int i = 0; i < 3; ++i)
    {
        int lfoIndex = 6 + i;
        float phaseIncrement = (voice.lfoBaseFreq[lfoIndex] * juce::MathConstants<float>::twoPi) / static_cast<float>(currentSampleRate);
        voice.lfoPhase[lfoIndex] += phaseIncrement;

        // Wrap phase
        if (voice.lfoPhase[lfoIndex] >= juce::MathConstants<float>::twoPi)
            voice.lfoPhase[lfoIndex] -= juce::MathConstants<float>::twoPi;

        // Generate smooth random value using sine wave
        float targetValue = std::sin(voice.lfoPhase[lfoIndex]);

        // One-pole low-pass filter for smoothing
        voice.lfoSmoothed[lfoIndex] += (targetValue - voice.lfoSmoothed[lfoIndex]) * 0.01f;
    }

    // Update secondary LFOs (indices 3-5) - middle layer, modulate primary speeds
    for (int i = 0; i < 3; ++i)
    {
        int lfoIndex = 3 + i;
        float phaseIncrement = (voice.lfoBaseFreq[lfoIndex] * juce::MathConstants<float>::twoPi) / static_cast<float>(currentSampleRate);
        voice.lfoPhase[lfoIndex] += phaseIncrement;

        // Wrap phase
        if (voice.lfoPhase[lfoIndex] >= juce::MathConstants<float>::twoPi)
            voice.lfoPhase[lfoIndex] -= juce::MathConstants<float>::twoPi;

        // Generate smooth random value
        float targetValue = std::sin(voice.lfoPhase[lfoIndex]);
        voice.lfoSmoothed[lfoIndex] += (targetValue - voice.lfoSmoothed[lfoIndex]) * 0.01f;
    }

    // Update primary LFOs (indices 0-2) - fastest layer, modulated by secondary and tertiary
    for (int i = 0; i < 3; ++i)
    {
        int lfoIndex = i;
        int secondaryIndex = 3 + i;  // Secondary LFO that modulates this primary's speed
        int tertiaryIndex = 6 + i;   // Tertiary LFO that modulates this primary's depth

        // Speed modulation from secondary LFO (±30%)
        float speedMod = 1.0f + (voice.lfoSmoothed[secondaryIndex] * 0.3f);
        float modulatedFreq = voice.lfoBaseFreq[lfoIndex] * speedMod;

        float phaseIncrement = (modulatedFreq * juce::MathConstants<float>::twoPi) / static_cast<float>(currentSampleRate);
        voice.lfoPhase[lfoIndex] += phaseIncrement;

        // Wrap phase
        if (voice.lfoPhase[lfoIndex] >= juce::MathConstants<float>::twoPi)
            voice.lfoPhase[lfoIndex] -= juce::MathConstants<float>::twoPi;

        // Depth modulation from tertiary LFO (±40%)
        float depthMod = 1.0f + (voice.lfoSmoothed[tertiaryIndex] * 0.4f);

        // Generate smooth random value with modulated depth
        float targetValue = std::sin(voice.lfoPhase[lfoIndex]) * depthMod;
        voice.lfoSmoothed[lfoIndex] += (targetValue - voice.lfoSmoothed[lfoIndex]) * 0.01f;
    }
}

void LushPadAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear output buffer
    buffer.clear();

    // Handle MIDI events (sample-accurate timing)
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int note = message.getNoteNumber();
            float velocity = message.getVelocity() / 127.0f;
            allocateVoice(note, velocity);
        }
        else if (message.isNoteOff())
        {
            int note = message.getNoteNumber();
            releaseVoice(note);
        }
    }

    // Read parameters (atomic, done once per buffer for efficiency)
    float timbreValue = parameters.getRawParameterValue("timbre")->load();
    float filterCutoffValue = parameters.getRawParameterValue("filter_cutoff")->load();
    float reverbAmountValue = parameters.getRawParameterValue("reverb_amount")->load();

    // Generate audio per-sample
    const int numSamples = buffer.getNumSamples();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float mixL = 0.0f;
        float mixR = 0.0f;

        // Process all active voices
        for (auto& voice : voices)
        {
            if (!voice.active)
                continue;

            // Update nested LFO system
            updateVoiceLFOs(voice);

            // Get LFO modulation values
            float panModulation = voice.lfoSmoothed[0];    // LFO1: -1 to +1 (panning)
            float fmModulation = voice.lfoSmoothed[1];     // LFO2: -1 to +1 (FM depth)
            float satModulation = voice.lfoSmoothed[2];    // LFO3: -1 to +1 (saturation)

            // Calculate modulated FM feedback depth
            float baseFeedbackDepth = timbreValue * 0.4f;
            float modulatedFeedback = baseFeedbackDepth * (1.0f + fmModulation * 0.2f);  // ±20%
            modulatedFeedback = juce::jlimit(0.0f, 0.4f, modulatedFeedback);

            // Calculate modulated saturation gain
            float baseSaturationGain = 1.0f + (timbreValue * 2.0f);
            float modulatedSaturation = baseSaturationGain * (1.0f + satModulation * 0.15f);  // ±15%
            modulatedSaturation = juce::jlimit(1.0f, 3.0f, modulatedSaturation);

            // Calculate pan position (0.0 = left, 0.5 = center, 1.0 = right)
            float panValue = 0.5f + (panModulation * 0.3f);  // ±30% from center
            panValue = juce::jlimit(0.0f, 1.0f, panValue);

            // Calculate base frequency for this MIDI note
            // f = 440 * 2^((note - 69) / 12)
            float baseFreq = 440.0f * std::pow(2.0f, (voice.currentNote - 69) / 12.0f);

            // Detuning ratios
            // +7 cents: 2^(7/1200) ≈ 1.00407
            // -7 cents: 2^(-7/1200) ≈ 0.99593
            float ratio1 = 1.0f;       // Base frequency
            float ratio2 = 1.00407f;   // +7 cents
            float ratio3 = 0.99593f;   // -7 cents

            // Generate 3 detuned sine oscillators WITH modulated FM feedback
            // Formula: sin(phase + modulatedFeedback * previousOutput)
            float osc1 = std::sin(voice.phase1 + modulatedFeedback * voice.previousOutput1);
            float osc2 = std::sin(voice.phase2 + modulatedFeedback * voice.previousOutput2);
            float osc3 = std::sin(voice.phase3 + modulatedFeedback * voice.previousOutput3);

            // Store outputs for next sample's feedback
            voice.previousOutput1 = osc1;
            voice.previousOutput2 = osc2;
            voice.previousOutput3 = osc3;

            // Sum oscillators (average to prevent clipping)
            float voiceOutput = (osc1 + osc2 + osc3) / 3.0f;

            // Apply modulated harmonic saturation using tanh waveshaping
            voiceOutput = std::tanh(modulatedSaturation * voiceOutput);

            // Calculate velocity-scaled filter cutoff
            // Soft notes (low velocity): darker sound (cutoff reduced by 50%)
            // Hard notes (high velocity): brighter sound (cutoff at parameter value)
            float velocityScaledCutoff = filterCutoffValue * (0.5f + 0.5f * voice.currentVelocity);

            // Clamp to valid range
            velocityScaledCutoff = juce::jlimit(20.0f, 20000.0f, velocityScaledCutoff);

            // Update filter coefficients (12dB/octave low-pass, Q=0.35)
            auto coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(
                currentSampleRate,
                velocityScaledCutoff,
                0.35f  // Fixed resonance
            );
            *voice.filter.coefficients = *coefficients;

            // Process through filter
            voiceOutput = voice.filter.processSample(voiceOutput);

            // Apply ADSR envelope
            float envelope = voice.adsr.getNextSample();
            voiceOutput *= envelope * voice.currentVelocity;

            // Apply LFO-modulated panning
            float leftGain = 1.0f - panValue;
            float rightGain = panValue;

            mixL += voiceOutput * leftGain;
            mixR += voiceOutput * rightGain;

            // Update oscillator phases
            float phaseIncrement1 = (baseFreq * ratio1 * juce::MathConstants<float>::twoPi) / static_cast<float>(currentSampleRate);
            float phaseIncrement2 = (baseFreq * ratio2 * juce::MathConstants<float>::twoPi) / static_cast<float>(currentSampleRate);
            float phaseIncrement3 = (baseFreq * ratio3 * juce::MathConstants<float>::twoPi) / static_cast<float>(currentSampleRate);

            voice.phase1 += phaseIncrement1;
            voice.phase2 += phaseIncrement2;
            voice.phase3 += phaseIncrement3;

            // Wrap phases to [0, 2π] to prevent denormals
            while (voice.phase1 >= juce::MathConstants<float>::twoPi)
                voice.phase1 -= juce::MathConstants<float>::twoPi;
            while (voice.phase2 >= juce::MathConstants<float>::twoPi)
                voice.phase2 -= juce::MathConstants<float>::twoPi;
            while (voice.phase3 >= juce::MathConstants<float>::twoPi)
                voice.phase3 -= juce::MathConstants<float>::twoPi;

            // Mark voice inactive if envelope has finished
            if (!voice.adsr.isActive())
            {
                voice.active = false;
            }
        }

        // Write to output buffer (reduce gain to prevent clipping with 8 voices)
        buffer.setSample(0, sample, mixL * 0.3f);
        if (totalNumOutputChannels > 1)
        {
            buffer.setSample(1, sample, mixR * 0.3f);
        }
    }

    // Apply global reverb with reverb_amount parameter controlling wet/dry
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // Update reverb wet/dry levels based on parameter
    juce::dsp::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.9f;
    reverbParams.damping = 0.4f;
    reverbParams.wetLevel = reverbAmountValue;
    reverbParams.dryLevel = 1.0f - reverbAmountValue;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;
    reverb.setParameters(reverbParams);

    reverb.process(context);
}

juce::AudioProcessorEditor* LushPadAudioProcessor::createEditor()
{
    return new LushPadAudioProcessorEditor(*this);
}

void LushPadAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LushPadAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Voice allocation helper methods
void LushPadAudioProcessor::allocateVoice(int note, float velocity)
{
    // First, try to find a free voice
    for (auto& voice : voices)
    {
        if (!voice.active || !voice.adsr.isActive())
        {
            startVoice(voice, note, velocity);
            return;
        }
    }

    // All voices busy - steal the oldest voice
    SynthVoice* oldest = &voices[0];
    for (auto& voice : voices)
    {
        if (voice.timestamp < oldest->timestamp)
        {
            oldest = &voice;
        }
    }

    // Gracefully release stolen voice before reusing
    oldest->adsr.noteOff();
    startVoice(*oldest, note, velocity);
}

void LushPadAudioProcessor::releaseVoice(int note)
{
    for (auto& voice : voices)
    {
        if (voice.active && voice.currentNote == note)
        {
            voice.adsr.noteOff();
        }
    }
}

void LushPadAudioProcessor::startVoice(SynthVoice& voice, int note, float velocity)
{
    voice.active = true;
    voice.currentNote = note;
    voice.currentVelocity = velocity;
    voice.timestamp = voiceCounter++;
    voice.phase1 = voice.phase2 = voice.phase3 = 0.0f;

    // Initialize random LFO base frequencies for this voice
    // Primary LFOs (0-2): 0.05-0.2 Hz
    for (int i = 0; i < 3; ++i)
    {
        voice.lfoBaseFreq[i] = 0.05f + random.nextFloat() * (0.2f - 0.05f);
    }

    // Secondary LFOs (3-5): 0.02-0.1 Hz
    for (int i = 3; i < 6; ++i)
    {
        voice.lfoBaseFreq[i] = 0.02f + random.nextFloat() * (0.1f - 0.02f);
    }

    // Tertiary LFOs (6-8): 0.01-0.05 Hz
    for (int i = 6; i < 9; ++i)
    {
        voice.lfoBaseFreq[i] = 0.01f + random.nextFloat() * (0.05f - 0.01f);
    }

    // Reset LFO phases and smoothed values
    for (int i = 0; i < 9; ++i)
    {
        voice.lfoPhase[i] = 0.0f;
        voice.lfoSmoothed[i] = 0.0f;
    }

    // Fixed ADSR parameters (Phase 3.1: not parameter-controlled yet)
    voice.adsrParams.attack = 0.3f;   // 300ms attack
    voice.adsrParams.decay = 0.2f;    // 200ms decay
    voice.adsrParams.sustain = 0.8f;  // 80% sustain level
    voice.adsrParams.release = 2.0f;  // 2000ms release

    voice.adsr.setParameters(voice.adsrParams);
    voice.adsr.noteOn();
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LushPadAudioProcessor();
}
