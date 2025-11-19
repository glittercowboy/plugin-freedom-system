#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
Spyder2000AudioProcessorEditor::Spyder2000AudioProcessorEditor(Spyder2000AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Helper lambda: Convert BinaryData to vector<byte>
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    //==========================================================================
    // CRITICAL: Construction order (see juce8-critical-patterns.md #11)
    //==========================================================================

    // 1️⃣ Create relays FIRST (no dependencies)
    gainRelay = std::make_unique<juce::WebSliderRelay>("gain");
    toneRelay = std::make_unique<juce::WebSliderRelay>("tone");
    levelRelay = std::make_unique<juce::WebSliderRelay>("level");

    // 2️⃣ Create WebView SECOND (depends on relays)
    webView = std::make_unique<juce::WebBrowserComponent>(
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()
            .withResourceProvider([this](const auto& url) { return getResource(url); })
            .withOptionsFrom(*gainRelay)
            .withOptionsFrom(*toneRelay)
            .withOptionsFrom(*levelRelay)
    );

    // 3️⃣ Create attachments LAST (depend on relays AND webView)
    // CRITICAL: JUCE 8 requires 3 parameters (parameter, relay, undoManager)
    // Missing nullptr causes silent failure - knobs freeze (pattern #12)
    gainAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.parameters.getParameter("gain"), *gainRelay, nullptr);

    toneAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.parameters.getParameter("tone"), *toneRelay, nullptr);

    levelAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
        *audioProcessor.parameters.getParameter("level"), *levelRelay, nullptr);

    //==========================================================================
    // Display WebView
    //==========================================================================

    addAndMakeVisible(*webView);
    webView->goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Set window size from v1-ui.yaml (450×300)
    setSize(450, 300);

    // Make window non-resizable
    setResizable(false, false);
}

Spyder2000AudioProcessorEditor::~Spyder2000AudioProcessorEditor()
{
    // Destruction happens in reverse order of declaration (automatic)
    // Attachments destroyed first, then webView, then relays
}

//==============================================================================
void Spyder2000AudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView fills entire window
    g.fillAll(juce::Colours::black);
}

void Spyder2000AudioProcessorEditor::resized()
{
    // WebView fills entire editor bounds
    if (webView)
        webView->setBounds(getLocalBounds());
}

//==============================================================================
std::optional<juce::WebBrowserComponent::Resource>
Spyder2000AudioProcessorEditor::getResource(const juce::String& url)
{
    // Helper lambda: Convert BinaryData to vector<byte>
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    //==========================================================================
    // Explicit URL mapping (see juce8-critical-patterns.md #8)
    //==========================================================================
    // BinaryData flattens paths (js/juce/index.js → index_js)
    // HTML/JS use original paths (./js/juce/index.js)
    // Explicit mapping bridges this gap clearly and debuggably
    //==========================================================================

    // Serve index.html (root path or explicit /index.html)
    if (url == "/" || url == "/index.html") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_html, BinaryData::index_htmlSize),
            juce::String("text/html")
        };
    }

    // Serve JUCE frontend library (index.js)
    if (url == "/js/juce/index.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::index_js, BinaryData::index_jsSize),
            juce::String("text/javascript")  // CRITICAL: text/javascript NOT application/javascript
        };
    }

    // Serve JUCE native interop check (REQUIRED - see pattern #13)
    if (url == "/js/juce/check_native_interop.js") {
        return juce::WebBrowserComponent::Resource {
            makeVector(BinaryData::check_native_interop_js, BinaryData::check_native_interop_jsSize),
            juce::String("text/javascript")
        };
    }

    // 404 - resource not found
    return std::nullopt;
}
