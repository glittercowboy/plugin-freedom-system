#include "PluginEditor.h"
#include "PluginProcessor.h"

// ============================================================================
// Resource Provider - Serves HTML/CSS/JS files from binary data
// ============================================================================

class Spyder2000AudioProcessorEditor::UIResourceProvider : public juce::WebBrowserComponent::Resource
{
public:
    UIResourceProvider(const juce::String& htmlContent)
        : html(htmlContent)
    {
    }

    juce::WebBrowserComponent::Resource getResource(const juce::String& url) override
    {
        // Serve the main HTML file
        if (url == "/")
        {
            return {
                html.toStdString(),
                "text/html; charset=utf-8"
            };
        }

        // Return empty resource for unknown URLs
        return { "", "text/plain" };
    }

private:
    juce::String html;
};

// ============================================================================
// Plugin Editor Implementation
// ============================================================================

Spyder2000AudioProcessorEditor::Spyder2000AudioProcessorEditor(Spyder2000AudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
{
    // Load HTML from binary data (will be embedded via juce_add_binary_data)
    // For now, using placeholder - actual binary data will be added in Stage 3
    juce::String htmlContent = R"html(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Spyder2000</title>
    <style>
        html, body {
            height: 100%;
            margin: 0;
            padding: 0;
            background: #DC143C;
            color: white;
            font-family: Arial, sans-serif;
            display: flex;
            align-items: center;
            justify-content: center;
            user-select: none;
        }
        .message {
            text-align: center;
        }
        h1 {
            font-size: 32px;
            margin-bottom: 10px;
        }
        p {
            font-size: 14px;
            opacity: 0.8;
        }
    </style>
</head>
<body>
    <div class="message">
        <h1>SPYDER 2000</h1>
        <p>WebView UI will be integrated in Stage 3</p>
        <p>Mockup v5: Flat silkscreen labels, Davies 1510 knobs</p>
    </div>
    <script>
        document.addEventListener('contextmenu', e => e.preventDefault());
    </script>
</body>
</html>
)html";

    // Create resource provider
    resourceProvider = std::make_unique<UIResourceProvider>(htmlContent);

    // Create WebView with resource provider
    juce::WebBrowserComponent::Options options;
    options = options.withResourceProvider(resourceProvider.get());

    webView = std::make_unique<juce::WebBrowserComponent>(options);
    addAndMakeVisible(webView.get());

    // Navigate to root URL (serves HTML from resource provider)
    webView->goToURL("/");

    // Set window size (450×300 from v5-ui.yaml)
    setSize(450, 300);

    // Make window non-resizable
    setResizable(false, false);
}

Spyder2000AudioProcessorEditor::~Spyder2000AudioProcessorEditor()
{
    // WebView cleanup handled by unique_ptr
}

void Spyder2000AudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView fills entire window - no background painting needed
    g.fillAll(juce::Colours::black);
}

void Spyder2000AudioProcessorEditor::resized()
{
    // WebView fills entire editor window
    if (webView)
    {
        webView->setBounds(getLocalBounds());
    }
}
