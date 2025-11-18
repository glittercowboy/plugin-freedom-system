#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

class Spyder2000AudioProcessor;

/**
 * Spyder2000 Plugin Editor - WebView UI (v5)
 *
 * v5 Design Features:
 * - Flat silkscreen labels (authentic pedal aesthetic)
 * - 5px rounded corners (Hammond 1590BB style)
 * - Davies 1510 knobs with stationary shine
 * - Glossy Ferrari Red chassis
 * - 7-segment display with LED glow
 *
 * Window: 450×300px (non-resizable)
 * Controls: 3 rotary knobs (gain, tone, level)
 */
class Spyder2000AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    Spyder2000AudioProcessorEditor(Spyder2000AudioProcessor&);
    ~Spyder2000AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Reference to audio processor
    Spyder2000AudioProcessor& audioProcessor;

    // WebView component for HTML/CSS/JavaScript UI
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // Resource provider for serving HTML/CSS/JS files
    class UIResourceProvider;
    std::unique_ptr<UIResourceProvider> resourceProvider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spyder2000AudioProcessorEditor)
};
