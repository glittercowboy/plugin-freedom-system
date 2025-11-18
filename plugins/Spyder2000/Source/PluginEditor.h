#pragma once
#include "PluginProcessor.h"

class Spyder2000AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit Spyder2000AudioProcessorEditor(Spyder2000AudioProcessor&);
    ~Spyder2000AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    Spyder2000AudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spyder2000AudioProcessorEditor)
};
