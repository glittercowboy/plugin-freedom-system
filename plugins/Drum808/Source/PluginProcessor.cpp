#include "PluginProcessor.h"
#include "PluginEditor.h"

// Parameter layout creation (BEFORE constructor)
juce::AudioProcessorValueTreeState::ParameterLayout Drum808AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // KICK (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_level", 1 },
        "Kick Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        80.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_tone", 1 },
        "Kick Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_decay", 1 },
        "Kick Decay",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        400.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "kick_tuning", 1 },
        "Kick Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // LOW TOM (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_level", 1 },
        "Low Tom Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        75.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_tone", 1 },
        "Low Tom Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_decay", 1 },
        "Low Tom Decay",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        300.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "lowtom_tuning", 1 },
        "Low Tom Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // MID TOM (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_level", 1 },
        "Mid Tom Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        75.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_tone", 1 },
        "Mid Tom Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_decay", 1 },
        "Mid Tom Decay",
        juce::NormalisableRange<float>(50.0f, 1000.0f, 1.0f),
        250.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "midtom_tuning", 1 },
        "Mid Tom Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        5.0f,
        "st"
    ));

    // CLAP (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_level", 1 },
        "Clap Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        70.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_tone", 1 },
        "Clap Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_snap", 1 },
        "Clap Snap",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "clap_tuning", 1 },
        "Clap Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // CLOSED HAT (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_level", 1 },
        "Closed Hat Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        65.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_tone", 1 },
        "Closed Hat Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_decay", 1 },
        "Closed Hat Decay",
        juce::NormalisableRange<float>(20.0f, 200.0f, 1.0f),
        80.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "closedhat_tuning", 1 },
        "Closed Hat Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    // OPEN HAT (4 parameters)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_level", 1 },
        "Open Hat Level",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_tone", 1 },
        "Open Hat Tone",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        60.0f,
        "%"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_decay", 1 },
        "Open Hat Decay",
        juce::NormalisableRange<float>(100.0f, 1000.0f, 1.0f),
        500.0f,
        "ms"
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "openhat_tuning", 1 },
        "Open Hat Tuning",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f,
        "st"
    ));

    return layout;
}

Drum808AudioProcessor::Drum808AudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Main", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Kick", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Low Tom", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Mid Tom", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Clap", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Closed Hat", juce::AudioChannelSet::stereo(), false)
                        .withOutput("Open Hat", juce::AudioChannelSet::stereo(), false))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

Drum808AudioProcessor::~Drum808AudioProcessor()
{
}

void Drum808AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    // Prepare DSP spec
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Configure and prepare Low Tom
    lowTom.oscillator.initialise([](float x) { return std::sin(x); }); // Sine wave
    lowTom.oscillator.prepare(spec);
    lowTom.filter.prepare(spec);
    lowTom.filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    lowTom.filter.setResonance(0.5f); // Initial Q (will be updated per-sample)
    lowTom.oscillator.reset();
    lowTom.filter.reset();

    // Configure and prepare Mid Tom
    midTom.oscillator.initialise([](float x) { return std::sin(x); }); // Sine wave
    midTom.oscillator.prepare(spec);
    midTom.filter.prepare(spec);
    midTom.filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    midTom.filter.setResonance(0.5f); // Initial Q (will be updated per-sample)
    midTom.oscillator.reset();
    midTom.filter.reset();

    // Configure and prepare Kick
    kick.bodyOscillator.initialise([](float x) { return std::sin(x); }); // Sine wave for body tone
    kick.bodyOscillator.prepare(spec);
    kick.bodyOscillator.reset();

    // Configure and prepare Closed Hi-Hat (6 square wave oscillators)
    for (int i = 0; i < 6; ++i)
    {
        closedHat.oscillators[i].initialise([](float x) {
            return x < 0.0f ? -1.0f : 1.0f; // Square wave
        });
        closedHat.oscillators[i].prepare(spec);
        closedHat.oscillators[i].reset();
    }
    closedHat.filter.prepare(spec);
    closedHat.filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    closedHat.filter.setResonance(4.0f); // High Q for metallic ring
    closedHat.filter.reset();

    // Configure and prepare Open Hi-Hat (6 square wave oscillators)
    for (int i = 0; i < 6; ++i)
    {
        openHat.oscillators[i].initialise([](float x) {
            return x < 0.0f ? -1.0f : 1.0f; // Square wave
        });
        openHat.oscillators[i].prepare(spec);
        openHat.oscillators[i].reset();
    }
    openHat.filter.prepare(spec);
    openHat.filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    openHat.filter.setResonance(4.0f); // High Q for metallic ring
    openHat.filter.reset();

    // Configure and prepare Clap (filtered noise with multi-trigger envelope)
    juce::dsp::ProcessSpec monoSpec;
    monoSpec.sampleRate = sampleRate;
    monoSpec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    monoSpec.numChannels = 1; // Mono voice

    clap.bandpassFilter.prepare(monoSpec);
    clap.bandpassFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    clap.bandpassFilter.reset();

    // Calculate spike transition samples (sample-rate independent)
    clap.spike2StartSample = static_cast<int>(sampleRate * 0.010);  // 10ms
    clap.spike3StartSample = static_cast<int>(sampleRate * 0.020);  // 20ms
    clap.decayStartSample = static_cast<int>(sampleRate * 0.030);   // 30ms
}

void Drum808AudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 3
}

void Drum808AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear all output buses
    buffer.clear();

    const int numSamples = buffer.getNumSamples();

    // Read all voice parameters (atomic, real-time safe)
    // Kick
    auto* kickLevelParam = parameters.getRawParameterValue("kick_level");
    auto* kickToneParam = parameters.getRawParameterValue("kick_tone");
    auto* kickDecayParam = parameters.getRawParameterValue("kick_decay");
    auto* kickTuningParam = parameters.getRawParameterValue("kick_tuning");

    float kickLevel = kickLevelParam->load() / 100.0f;
    float kickTone = kickToneParam->load() / 100.0f;
    float kickDecay = kickDecayParam->load() / 1000.0f; // ms → seconds
    float kickTuning = kickTuningParam->load();
    const float kickBaseFreq = 60.0f * std::pow(2.0f, kickTuning / 12.0f);

    // Tom parameters
    auto* lowTomLevelParam = parameters.getRawParameterValue("lowtom_level");
    auto* lowTomToneParam = parameters.getRawParameterValue("lowtom_tone");
    auto* lowTomDecayParam = parameters.getRawParameterValue("lowtom_decay");
    auto* lowTomTuningParam = parameters.getRawParameterValue("lowtom_tuning");

    auto* midTomLevelParam = parameters.getRawParameterValue("midtom_level");
    auto* midTomToneParam = parameters.getRawParameterValue("midtom_tone");
    auto* midTomDecayParam = parameters.getRawParameterValue("midtom_decay");
    auto* midTomTuningParam = parameters.getRawParameterValue("midtom_tuning");

    float lowTomLevel = lowTomLevelParam->load() / 100.0f;
    float lowTomTone = lowTomToneParam->load() / 100.0f;
    float lowTomDecay = lowTomDecayParam->load() / 1000.0f;
    float lowTomTuning = lowTomTuningParam->load();

    float midTomLevel = midTomLevelParam->load() / 100.0f;
    float midTomTone = midTomToneParam->load() / 100.0f;
    float midTomDecay = midTomDecayParam->load() / 1000.0f;
    float midTomTuning = midTomTuningParam->load();

    // Clap parameters
    auto* clapLevelParam = parameters.getRawParameterValue("clap_level");
    auto* clapToneParam = parameters.getRawParameterValue("clap_tone");
    auto* clapSnapParam = parameters.getRawParameterValue("clap_snap");
    auto* clapTuningParam = parameters.getRawParameterValue("clap_tuning");

    float clapLevel = clapLevelParam->load() / 100.0f;
    float clapTone = clapToneParam->load() / 100.0f;
    float clapSnap = clapSnapParam->load() / 100.0f;
    float clapTuning = clapTuningParam->load();

    // Hi-Hat parameters
    auto* closedHatLevelParam = parameters.getRawParameterValue("closedhat_level");
    auto* closedHatToneParam = parameters.getRawParameterValue("closedhat_tone");
    auto* closedHatDecayParam = parameters.getRawParameterValue("closedhat_decay");
    auto* closedHatTuningParam = parameters.getRawParameterValue("closedhat_tuning");

    auto* openHatLevelParam = parameters.getRawParameterValue("openhat_level");
    auto* openHatToneParam = parameters.getRawParameterValue("openhat_tone");
    auto* openHatDecayParam = parameters.getRawParameterValue("openhat_decay");
    auto* openHatTuningParam = parameters.getRawParameterValue("openhat_tuning");

    float closedHatLevel = closedHatLevelParam->load() / 100.0f;
    float closedHatTone = closedHatToneParam->load() / 100.0f;
    float closedHatDecay = closedHatDecayParam->load() / 1000.0f;
    float closedHatTuning = closedHatTuningParam->load();

    float openHatLevel = openHatLevelParam->load() / 100.0f;
    float openHatTone = openHatToneParam->load() / 100.0f;
    float openHatDecay = openHatDecayParam->load() / 1000.0f;
    float openHatTuning = openHatTuningParam->load();

    // Calculate tuned base frequencies
    const float lowTomBaseFreq = 150.0f * std::pow(2.0f, lowTomTuning / 12.0f);
    const float midTomBaseFreq = 220.0f * std::pow(2.0f, midTomTuning / 12.0f);
    const float clapCenterFreq = 1000.0f * std::pow(2.0f, clapTuning / 12.0f);
    const float closedHatBaseFreq = 3500.0f * std::pow(2.0f, closedHatTuning / 12.0f);
    const float openHatBaseFreq = 3500.0f * std::pow(2.0f, openHatTuning / 12.0f);

    // Map tone parameters
    const float lowTomQ = 0.5f + (lowTomTone * 4.5f);
    const float midTomQ = 0.5f + (midTomTone * 4.5f);
    const float clapQ = 2.0f + (clapTone * 3.0f); // Q range 2.0-5.0
    const float closedHatCenterFreq = 6000.0f + (closedHatTone * 6000.0f); // 6-12 kHz
    const float openHatCenterFreq = 6000.0f + (openHatTone * 6000.0f);

    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int note = message.getNoteNumber();
            float velocity = message.getVelocity() / 127.0f;

            // Map MIDI notes to voices
            if (note == 36) // C1 → Kick
            {
                kick.trigger(velocity);
            }
            else if (note == 38) // D1 → Clap
            {
                clap.trigger(velocity);
            }
            else if (note == 41) // F1 → Low Tom
            {
                lowTom.trigger(velocity, lowTomBaseFreq);
            }
            else if (note == 42) // F#1 → Closed Hat (CHOKES open hat)
            {
                // FIRST: Choke open hat (stop immediately)
                openHat.stop();

                // THEN: Trigger closed hat
                closedHat.trigger(velocity);
            }
            else if (note == 45) // A1 → Mid Tom
            {
                midTom.trigger(velocity, midTomBaseFreq);
            }
            else if (note == 46) // A#1 → Open Hat
            {
                openHat.trigger(velocity);
            }
        }
    }

    // Configure clap filter (outside loop for efficiency)
    clap.bandpassFilter.setCutoffFrequency(clapCenterFreq);
    clap.bandpassFilter.setResonance(clapQ);

    // Synthesize voices (per-sample processing)
    for (int sample = 0; sample < numSamples; ++sample)
    {
        float kickSample = 0.0f;
        float lowTomSample = 0.0f;
        float midTomSample = 0.0f;
        float clapSample = 0.0f;
        float closedHatSample = 0.0f;
        float openHatSample = 0.0f;

        // Kick synthesis (pitch envelope + attack transient)
        if (kick.isPlaying)
        {
            // Pitch envelope: exponential sweep from 2× to 1× base frequency
            float currentFreq = kickBaseFreq * (1.0f + std::exp(-kick.envelopeTime / 0.02f));
            kick.bodyOscillator.setFrequency(currentFreq);

            // Body tone (sine oscillator)
            float bodySignal = kick.bodyOscillator.processSample(0.0f);

            // Attack transient (noise burst scaled by tone parameter)
            float attackSignal = (kick.noiseGenerator.nextFloat() * 2.0f - 1.0f) *
                                 std::exp(-kick.envelopeTime / 0.005f) * kickTone;

            // Amplitude envelope (exponential decay)
            float amplitudeEnv = std::exp(-kick.envelopeTime / kickDecay);

            // Denormal protection
            if (amplitudeEnv < 1e-8f)
            {
                kick.stop();
                amplitudeEnv = 0.0f;
            }

            // Final output
            kickSample = (bodySignal + attackSignal) * amplitudeEnv * kick.velocity * kickLevel;

            // Advance envelope time
            kick.envelopeTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // Low Tom synthesis
        if (lowTom.isPlaying)
        {
            lowTom.filter.setCutoffFrequency(lowTomBaseFreq);
            lowTom.filter.setResonance(lowTomQ);

            float oscSample = lowTom.oscillator.processSample(0.0f);
            float filteredSample = lowTom.filter.processSample(0, oscSample);
            float envelope = std::exp(-lowTom.envelopeTime / lowTomDecay);

            if (envelope < 1e-8f)
            {
                lowTom.stop();
                envelope = 0.0f;
            }

            lowTomSample = filteredSample * envelope * lowTom.velocity * lowTomLevel;
            lowTom.envelopeTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // Mid Tom synthesis
        if (midTom.isPlaying)
        {
            midTom.filter.setCutoffFrequency(midTomBaseFreq);
            midTom.filter.setResonance(midTomQ);

            float oscSample = midTom.oscillator.processSample(0.0f);
            float filteredSample = midTom.filter.processSample(0, oscSample);
            float envelope = std::exp(-midTom.envelopeTime / midTomDecay);

            if (envelope < 1e-8f)
            {
                midTom.stop();
                envelope = 0.0f;
            }

            midTomSample = filteredSample * envelope * midTom.velocity * midTomLevel;
            midTom.envelopeTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // Clap synthesis (multi-trigger envelope + filtered noise)
        if (clap.isPlaying)
        {
            // Generate white noise
            float noise = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;

            // Apply bandpass filter
            float filteredNoise = clap.bandpassFilter.processSample(0, noise);

            // Calculate envelope based on state machine
            float envelope = 0.0f;
            int t = clap.envelopeSample;

            if (clap.envelopeState == ClapEnvelopeState::Spike1)
            {
                float timeInSpike = t / static_cast<float>(currentSampleRate);
                envelope = clapSnap * std::exp(-timeInSpike / 0.003f);

                if (t >= clap.spike2StartSample)
                {
                    clap.envelopeState = ClapEnvelopeState::Spike2;
                }
            }
            else if (clap.envelopeState == ClapEnvelopeState::Spike2)
            {
                float timeInSpike = (t - clap.spike2StartSample) / static_cast<float>(currentSampleRate);
                envelope = clapSnap * 0.6f * std::exp(-timeInSpike / 0.003f);

                if (t >= clap.spike3StartSample)
                {
                    clap.envelopeState = ClapEnvelopeState::Spike3;
                }
            }
            else if (clap.envelopeState == ClapEnvelopeState::Spike3)
            {
                float timeInSpike = (t - clap.spike3StartSample) / static_cast<float>(currentSampleRate);
                envelope = clapSnap * 0.3f * std::exp(-timeInSpike / 0.003f);

                if (t >= clap.decayStartSample)
                {
                    clap.envelopeState = ClapEnvelopeState::Decay;
                }
            }
            else if (clap.envelopeState == ClapEnvelopeState::Decay)
            {
                float timeInDecay = (t - clap.decayStartSample) / static_cast<float>(currentSampleRate);
                envelope = std::exp(-timeInDecay / 1.934f);

                // Stop voice after decay tail (envelope < threshold)
                if (envelope < 1e-4f)
                {
                    clap.stop();
                    envelope = 0.0f;
                }
            }

            // Apply envelope, level, and velocity
            clapSample = filteredNoise * envelope * clapLevel * clap.velocity;

            clap.envelopeSample++;
        }

        // Closed Hi-Hat synthesis (6 oscillators + bandpass)
        if (closedHat.isPlaying)
        {
            // Frequency ratios for inharmonic spectrum
            const float ratios[6] = {1.0f, 1.4f, 1.7f, 2.1f, 2.5f, 3.0f};
            float mixedSignal = 0.0f;

            // Mix 6 square wave oscillators
            for (int i = 0; i < 6; ++i)
            {
                closedHat.oscillators[i].setFrequency(closedHatBaseFreq * ratios[i]);
                mixedSignal += closedHat.oscillators[i].processSample(0.0f) / 6.0f;
            }

            // Bandpass filtering (6-12 kHz controlled by tone)
            closedHat.filter.setCutoffFrequency(closedHatCenterFreq);
            float filteredSignal = closedHat.filter.processSample(0, mixedSignal);

            // Exponential decay
            float envelope = std::exp(-closedHat.envelopeTime / closedHatDecay);

            if (envelope < 1e-8f)
            {
                closedHat.stop();
                envelope = 0.0f;
            }

            closedHatSample = filteredSignal * envelope * closedHat.velocity * closedHatLevel;
            closedHat.envelopeTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // Open Hi-Hat synthesis (6 oscillators + bandpass, longer decay)
        if (openHat.isPlaying)
        {
            const float ratios[6] = {1.0f, 1.4f, 1.7f, 2.1f, 2.5f, 3.0f};
            float mixedSignal = 0.0f;

            for (int i = 0; i < 6; ++i)
            {
                openHat.oscillators[i].setFrequency(openHatBaseFreq * ratios[i]);
                mixedSignal += openHat.oscillators[i].processSample(0.0f) / 6.0f;
            }

            openHat.filter.setCutoffFrequency(openHatCenterFreq);
            float filteredSignal = openHat.filter.processSample(0, mixedSignal);

            float envelope = std::exp(-openHat.envelopeTime / openHatDecay);

            if (envelope < 1e-8f)
            {
                openHat.stop();
                envelope = 0.0f;
            }

            openHatSample = filteredSignal * envelope * openHat.velocity * openHatLevel;
            openHat.envelopeTime += 1.0f / static_cast<float>(currentSampleRate);
        }

        // Write to output buses
        // Main mix (bus 0, stereo)
        if (buffer.getNumChannels() >= 2)
        {
            float mainMix = kickSample + lowTomSample + midTomSample + clapSample + closedHatSample + openHatSample;
            buffer.addSample(0, sample, mainMix); // Left
            buffer.addSample(1, sample, mainMix); // Right
        }

        // Individual outputs (if enabled by DAW)
        // Kick output (bus 1, channels 2-3)
        if (buffer.getNumChannels() >= 4)
        {
            buffer.addSample(2, sample, kickSample); // Kick Left
            buffer.addSample(3, sample, kickSample); // Kick Right
        }

        // Low Tom output (bus 2, channels 4-5)
        if (buffer.getNumChannels() >= 6)
        {
            buffer.addSample(4, sample, lowTomSample); // Low Tom Left
            buffer.addSample(5, sample, lowTomSample); // Low Tom Right
        }

        // Mid Tom output (bus 3, channels 6-7)
        if (buffer.getNumChannels() >= 8)
        {
            buffer.addSample(6, sample, midTomSample); // Mid Tom Left
            buffer.addSample(7, sample, midTomSample); // Mid Tom Right
        }

        // Clap output (bus 4, channels 8-9)
        if (buffer.getNumChannels() >= 10)
        {
            buffer.addSample(8, sample, clapSample); // Clap Left
            buffer.addSample(9, sample, clapSample); // Clap Right
        }

        // Closed Hat output (bus 5, channels 10-11)
        if (buffer.getNumChannels() >= 12)
        {
            buffer.addSample(10, sample, closedHatSample); // Closed Hat Left
            buffer.addSample(11, sample, closedHatSample); // Closed Hat Right
        }

        // Open Hat output (bus 6, channels 12-13)
        if (buffer.getNumChannels() >= 14)
        {
            buffer.addSample(12, sample, openHatSample); // Open Hat Left
            buffer.addSample(13, sample, openHatSample); // Open Hat Right
        }
    }
}

juce::AudioProcessorEditor* Drum808AudioProcessor::createEditor()
{
    return new Drum808AudioProcessorEditor(*this);
}

void Drum808AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Drum808AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Drum808AudioProcessor();
}
