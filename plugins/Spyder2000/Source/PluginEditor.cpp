#include "PluginEditor.h"

Spyder2000AudioProcessorEditor::Spyder2000AudioProcessorEditor(Spyder2000AudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(600, 400);
}

Spyder2000AudioProcessorEditor::~Spyder2000AudioProcessorEditor()
{
}

void Spyder2000AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("Spyder 2000 - Stage 1 (Minimal)", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("1 parameter implemented (gain only)",
                     getLocalBounds().reduced(20).removeFromBottom(30),
                     juce::Justification::centred, 1);
}

void Spyder2000AudioProcessorEditor::resized()
{
    // Layout will be added in Stage 3 (GUI)
}
