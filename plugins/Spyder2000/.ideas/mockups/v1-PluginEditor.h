#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>

// Forward declaration
class Spyder2000AudioProcessor;

//==============================================================================
/**
 * Spyder2000 WebView-based Plugin Editor
 *
 * This editor uses JUCE's WebBrowserComponent to display a custom HTML/CSS/JS interface.
 * Parameters are bound using WebSliderRelay + WebSliderParameterAttachment pattern.
 *
 * CRITICAL: Member declaration order MUST be:
 *   1. Relays (no dependencies)
 *   2. WebView (depends on relays)
 *   3. Attachments (depend on relays AND webView)
 *
 * This order ensures correct destruction sequence (reverse of declaration).
 * Wrong order causes release build crashes when attachments try to call
 * evaluateJavascript() on already-destroyed WebView.
 */
class Spyder2000AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    Spyder2000AudioProcessorEditor(Spyder2000AudioProcessor&);
    ~Spyder2000AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Reference to processor
    Spyder2000AudioProcessor& audioProcessor;

    //==============================================================================
    // CRITICAL MEMBER DECLARATION ORDER (see juce8-critical-patterns.md #11)
    //==============================================================================

    // 1️⃣ RELAYS FIRST (no dependencies)
    std::unique_ptr<juce::WebSliderRelay> gainRelay;
    std::unique_ptr<juce::WebSliderRelay> toneRelay;
    std::unique_ptr<juce::WebSliderRelay> levelRelay;

    // 2️⃣ WEBVIEW SECOND (depends on relays)
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // 3️⃣ ATTACHMENTS LAST (depend on relays AND webView)
    std::unique_ptr<juce::WebSliderParameterAttachment> gainAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> toneAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> levelAttachment;

    //==============================================================================
    // Helper: Resource provider for WebView (serves v1-ui.html and JUCE scripts)
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Spyder2000AudioProcessorEditor)
};
