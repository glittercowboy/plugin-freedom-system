#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

/**
 * PadForge WebView-based Plugin Editor
 *
 * Generated from: v2-ui.yaml
 * Parameter count: 4 parameters
 *
 * CRITICAL: Member order prevents 90% of release build crashes.
 *
 * Destruction order (reverse of declaration):
 * 1. Attachments destroyed FIRST (stop using relays and WebView)
 * 2. WebView destroyed SECOND (safe, attachments are gone)
 * 3. Relays destroyed LAST (safe, nothing using them)
 */

class PadForgeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    PadForgeAudioProcessorEditor(PadForgeAudioProcessor& p);
    ~PadForgeAudioProcessorEditor() override;

    // AudioProcessorEditor overrides
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    /**
     * Resource provider (JUCE 8 required pattern)
     * Maps URLs to embedded binary data.
     *
     * @param url Requested resource URL (e.g., "/", "/js/juce/index.js")
     * @return Resource data and MIME type, or std::nullopt for 404
     */
    std::optional<juce::WebBrowserComponent::Resource> getResource(
        const juce::String& url
    );

    // Reference to audio processor
    PadForgeAudioProcessor& audioProcessor;

    // ========================================================================
    // ⚠️ CRITICAL MEMBER DECLARATION ORDER ⚠️
    //
    // Order: Relays → WebView → Attachments
    //
    // Why: Members are destroyed in REVERSE order of declaration.
    // - Attachments must be destroyed BEFORE WebView (they call evaluateJavascript)
    // - WebView must be destroyed BEFORE Relays (it holds references via Options)
    //
    // DO NOT REORDER without understanding destructor sequence!
    // ========================================================================

    // ------------------------------------------------------------------------
    // 1️⃣ RELAYS FIRST (created first, destroyed last)
    // ------------------------------------------------------------------------

    std::unique_ptr<juce::WebSliderRelay> volumeRelay;
    std::unique_ptr<juce::WebSliderRelay> brightnessRelay;
    std::unique_ptr<juce::WebSliderRelay> spaceRelay;
    std::unique_ptr<juce::WebSliderRelay> randomizeAmountRelay;

    // ------------------------------------------------------------------------
    // 2️⃣ WEBVIEW SECOND (created after relays, destroyed before relays)
    // ------------------------------------------------------------------------

    std::unique_ptr<juce::WebBrowserComponent> webView;

    // ------------------------------------------------------------------------
    // 3️⃣ PARAMETER ATTACHMENTS LAST (created last, destroyed first)
    // ------------------------------------------------------------------------

    std::unique_ptr<juce::WebSliderParameterAttachment> volumeAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> brightnessAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> spaceAttachment;
    std::unique_ptr<juce::WebSliderParameterAttachment> randomizeAmountAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PadForgeAudioProcessorEditor)
};
