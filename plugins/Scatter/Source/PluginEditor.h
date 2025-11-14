#pragma once
#include "PluginProcessor.h"

class ScatterAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ScatterAudioProcessorEditor(ScatterAudioProcessor&);
    ~ScatterAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ScatterAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScatterAudioProcessorEditor)
};
