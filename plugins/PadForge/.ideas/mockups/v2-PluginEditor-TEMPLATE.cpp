#include "PluginEditor.h"

//==============================================================================
// Constructor - CRITICAL: Initialize in correct order
//==============================================================================

PadForgeAudioProcessorEditor::PadForgeAudioProcessorEditor(PadForgeAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p)
{
    // ========================================================================
    // INITIALIZATION SEQUENCE (CRITICAL ORDER)
    // ========================================================================
    //
    // 1. Create relays FIRST (before WebView construction)
    // 2. Create WebView with relay options
    // 3. Create parameter attachments LAST (after WebView construction)
    //
    // This matches the member declaration order and ensures safe destruction.
    // ========================================================================

    // ------------------------------------------------------------------------
    // STEP 1: CREATE RELAYS (before WebView!)
    // ------------------------------------------------------------------------

    volumeRelay = std::make_unique<juce::WebSliderRelay>("volume");
    brightnessRelay = std::make_unique<juce::WebSliderRelay>("brightness");
    spaceRelay = std::make_unique<juce::WebSliderRelay>("space");
    randomizeAmountRelay = std::make_unique<juce::WebSliderRelay>("randomizeAmount");

    // ------------------------------------------------------------------------
    // STEP 2: CREATE WEBVIEW (with relay options)
    // ------------------------------------------------------------------------

    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            // REQUIRED: Enable JUCE frontend library
            .withNativeIntegrationEnabled()

            // REQUIRED: Resource provider for embedded files
            .withResourceProvider([this](const auto& url) {
                return getResource(url);
            })

            // OPTIONAL: FL Studio fix (prevents blank screen on focus loss)
            .withKeepPageLoadedWhenBrowserIsHidden()

            // REQUIRED: Register each relay with WebView
            .withOptionsFrom(*volumeRelay)
            .withOptionsFrom(*brightnessRelay)
            .withOptionsFrom(*spaceRelay)
            .withOptionsFrom(*randomizeAmountRelay)
    );

    // ------------------------------------------------------------------------
    // STEP 3: CREATE PARAMETER ATTACHMENTS (after WebView!)
    // ------------------------------------------------------------------------

    volumeAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("volume"),
        *volumeRelay,
        nullptr  // No undo manager
    );

    brightnessAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("brightness"),
        *brightnessRelay,
        nullptr
    );

    spaceAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("space"),
        *spaceRelay,
        nullptr
    );

    randomizeAmountAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.apvts.getParameter("randomizeAmount"),
        *randomizeAmountRelay,
        nullptr
    );

    // ------------------------------------------------------------------------
    // WEBVIEW SETUP
    // ------------------------------------------------------------------------

    // Navigate to root (loads index.html via resource provider)
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Make WebView visible
    addAndMakeVisible(*webView);

    // ------------------------------------------------------------------------
    // WINDOW SIZE (from v2-ui.yaml)
    // ------------------------------------------------------------------------

    setSize(400, 350);  // Fixed size, non-resizable
}

//==============================================================================
// Destructor
//==============================================================================

PadForgeAudioProcessorEditor::~PadForgeAudioProcessorEditor()
{
    // Members are automatically destroyed in reverse order of declaration:
    // 1. Attachments destroyed first (stop calling evaluateJavascript)
    // 2. WebView destroyed second (safe, attachments are gone)
    // 3. Relays destroyed last (safe, nothing using them)
    //
    // No manual cleanup needed if member order is correct!
}

//==============================================================================
// AudioProcessorEditor Overrides
//==============================================================================

void PadForgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView fills the entire editor, so typically no custom painting needed
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PadForgeAudioProcessorEditor::resized()
{
    // Make WebView fill the entire editor bounds
    webView->setBounds(getLocalBounds());
}

//==============================================================================
// Resource Provider (JUCE 8 Required Pattern)
//==============================================================================

std::optional<juce::WebBrowserComponent::Resource> PadForgeAudioProcessorEditor::getResource(
    const juce::String& url
)
{
    // ========================================================================
    // RESOURCE PROVIDER IMPLEMENTATION
    // ========================================================================
    //
    // Maps URLs to embedded binary data (from juce_add_binary_data).
    //
    // File path → BinaryData symbol:
    // - Source/ui/public/index.html       → BinaryData::index_html
    // - Source/ui/public/js/juce/index.js → BinaryData::index_js
    //
    // Pattern: Explicit URL mapping (clear, debuggable, reliable)
    // ========================================================================

    // Helper lambda to convert raw bytes to std::vector<std::byte>
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Root URL (redirect to index.html)
    if (url == "/" || url == "/index.html") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    // JUCE frontend library
    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("application/javascript")
        };
    }

    // JUCE native interop check (REQUIRED)
    if (url == "/js/juce/check_native_interop.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js, BinaryData::check_native_interop_jsSize),
            juce::String("application/javascript")
        };
    }

    // 404 - Resource not found
    return std::nullopt;
}
