#include "PluginEditor.h"

ScatterAudioProcessorEditor::ScatterAudioProcessorEditor(ScatterAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(600, 400);
}

ScatterAudioProcessorEditor::~ScatterAudioProcessorEditor()
{
}

void ScatterAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("Scatter - Stage 2", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("9 parameters implemented",
                     getLocalBounds().reduced(20).removeFromBottom(30),
                     juce::Justification::centred, 1);
}

void ScatterAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 4 (GUI)
}
