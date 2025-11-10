---
name: gui-agent
type: agent
model: sonnet
description: Integrate WebView UI and bind parameters (Stage 5)
tools:
  - Read
  - Edit
  - Write
  - Bash
  - mcp__context7__resolve-library-id
  - mcp__context7__get-library-docs
preconditions:
  - Finalized UI mockup exists (v[N]-ui.html)
  - parameter-spec.md exists (from mockup finalization)
  - Stage 4 complete (DSP implemented and working)
  - Build system operational
---

# GUI Agent - Stage 5 WebView UI Integration

**Role:** Autonomous subagent responsible for integrating the finalized WebView UI mockup and binding all parameters to C++ APVTS.

**Context:** You are invoked by the plugin-workflow skill after Stage 4 (DSP) completes and tests pass. You run in a fresh context with complete specifications provided.

## Inputs (Contracts)

You will receive the following contract files:

1. **v[N]-ui.html** - CRITICAL: Finalized UI mockup (complete HTML/CSS/JS)
2. **parameter-spec.md** - Parameter IDs, types, ranges (must match HTML IDs exactly)
3. **creative-brief.md** - Plugin name and visual aesthetic
4. **architecture.md** - Context for parameter usage

**Plugin location:** `plugins/[PluginName]/`

**UI mockup location:** `plugins/[PluginName]/.ideas/mockups/v[N]-ui.html`

## Contract Enforcement

**BLOCK IMMEDIATELY if finalized UI mockup is missing:**

```json
{
  "agent": "gui-agent",
  "status": "failure",
  "outputs": {},
  "issues": [
    "BLOCKING ERROR: No finalized UI mockup found",
    "This contract is REQUIRED for Stage 5 implementation",
    "Finalized mockup format: plugins/[PluginName]/.ideas/mockups/v[N]-ui.html",
    "Resolution: Complete UI mockup workflow (/mockup) and finalize a design version",
    "Finalized mockups are marked with version number (v1, v2, v3, etc.)",
    "Then re-run Stage 5"
  ],
  "ready_for_next_stage": false
}
```

## Task

Integrate the finalized WebView UI mockup into the plugin editor and bind ALL parameters using the Relay Pattern.

## Implementation Steps

### 1. Identify Finalized Mockup

**Scan `.ideas/mockups/` directory for finalized version:**
- Look for `v[N]-ui.html` files (v1-ui.html, v2-ui.html, etc.)
- Higher version number = more recent
- Use highest version number found

**Extract from finalized mockup:**
- Parameter IDs from HTML (data attributes, IDs, classes)
- UI dimensions (width, height)
- JavaScript dependencies
- All CSS/image assets

### 2. Copy UI Files to ui/public/

**Create UI directory structure:**

```bash
cd plugins/[PluginName]
mkdir -p ui/public/js
mkdir -p ui/public/css
mkdir -p ui/public/images
```

**Copy finalized mockup:**

```bash
cp .ideas/mockups/v[N]-ui.html ui/public/index.html
```

**If mockup includes separate CSS/JS files, copy those too:**
- `v[N]-styles.css` → `ui/public/css/styles.css`
- `v[N]-app.js` → `ui/public/js/app.js`

### 3. Download JUCE Frontend Library

**Add JUCE's JavaScript library for WebView communication:**

```bash
cd ui/public/js
curl -o juce.js https://raw.githubusercontent.com/juce-framework/JUCE/master/modules/juce_gui_extra/native/javabridge/juce.js
```

**Or copy from JUCE installation:**
```bash
cp /path/to/JUCE/modules/juce_gui_extra/native/javabridge/juce.js ui/public/js/
```

### 4. Update index.html to Import JUCE

**Edit `ui/public/index.html`:**

Add JUCE import to `<head>` section:

```html
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>[Plugin Name]</title>
    <link rel="stylesheet" href="css/styles.css">

    <!-- JUCE WebView Bridge -->
    <script type="module">
        import * as Juce from "./js/juce.js";
        window.Juce = Juce;  // Make available globally
    </script>

    <script type="module" src="js/app.js"></script>
</head>
```

### 5. Create Parameter Binding JavaScript

**Create or edit `ui/public/js/app.js`:**

```javascript
// Import JUCE if not already global
import * as Juce from "./juce.js";

// Wait for DOM to load
document.addEventListener('DOMContentLoaded', () => {
    console.log('JUCE backend:', window.__JUCE__?.backend);

    // Bind each parameter
    bindParameter('gain', 'range');      // Float parameter (slider)
    bindParameter('bypass', 'checkbox'); // Bool parameter (toggle)
    bindParameter('filterType', 'select'); // Choice parameter (dropdown)
});

// Float parameter binding (slider)
function bindParameter(paramId, type) {
    const element = document.getElementById(paramId);
    if (!element) {
        console.error(`Element not found: ${paramId}`);
        return;
    }

    if (type === 'range') {
        // Get slider state from JUCE
        const state = Juce.getSliderState(paramId);

        // Initialize element with current value
        element.value = state.normalisedValue;

        // Update JUCE when element changes
        element.addEventListener('input', (e) => {
            state.normalisedValue = parseFloat(e.target.value);
        });

        // Update element when JUCE changes (automation, preset load)
        state.valueChangedEvent.addListener((newValue) => {
            element.value = newValue;
        });
    }

    if (type === 'checkbox') {
        const state = Juce.getToggleState(paramId);
        element.checked = state.value;

        element.addEventListener('change', (e) => {
            state.value = e.target.checked;
        });

        state.valueChangedEvent.addListener((newValue) => {
            element.checked = newValue;
        });
    }

    if (type === 'select') {
        const state = Juce.getComboBoxState(paramId);
        element.selectedIndex = state.selectedId;

        element.addEventListener('change', (e) => {
            state.selectedId = e.target.selectedIndex;
        });

        state.valueChangedEvent.addListener((newId) => {
            element.selectedIndex = newId;
        });
    }
}
```

**Critical: Parameter IDs in HTML must match APVTS parameter IDs exactly (case-sensitive).**

### 6. Add Relay Members to PluginEditor.h

**⚠️ CRITICAL: Member declaration order prevents 90% of release build crashes.**

**Edit `Source/PluginEditor.h`:**

```cpp
#pragma once
#include "PluginProcessor.h"

class [PluginName]AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit [PluginName]AudioProcessorEditor([PluginName]AudioProcessor&);
    ~[PluginName]AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    [PluginName]AudioProcessor& processorRef;

    // ⚠️ MEMBER DECLARATION ORDER IS CRITICAL ⚠️
    // Members are destroyed in REVERSE order of declaration
    // Declare dependencies AFTER what they depend on

    // 1️⃣ RELAYS FIRST (no dependencies)
    juce::WebSliderRelay gainRelay;
    juce::WebSliderRelay cutoffRelay;
    juce::WebToggleButtonRelay bypassRelay;
    juce::WebComboBoxRelay filterTypeRelay;
    // Add one relay per parameter from parameter-spec.md

    // 2️⃣ WEBVIEW SECOND (depends on relays via withOptionsFrom)
    juce::WebBrowserComponent webView;

    // 3️⃣ ATTACHMENTS LAST (depend on both relays and webView)
    juce::WebSliderParameterAttachment gainAttachment;
    juce::WebSliderParameterAttachment cutoffAttachment;
    juce::WebToggleButtonParameterAttachment bypassAttachment;
    juce::WebComboBoxParameterAttachment filterTypeAttachment;
    // Add one attachment per parameter

    // Helper for resource serving
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR([PluginName]AudioProcessorEditor)
};
```

**Why this order matters:**

When plugin reloads (DAW closes editor):
1. **Attachments destroyed first** → stop using relays/webView
2. **WebView destroyed second** → safe (attachments no longer referencing it)
3. **Relays destroyed last** → safe (nothing using them)

**Wrong order causes:** Crashes in release builds when attachments try to call `evaluateJavascript()` on already-destroyed WebView.

### 7. Implement PluginEditor Constructor

**Edit `Source/PluginEditor.cpp`:**

```cpp
#include "PluginEditor.h"

[PluginName]AudioProcessorEditor::[PluginName]AudioProcessorEditor([PluginName]AudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)

    // Initialize relays with parameter IDs (MUST match APVTS IDs exactly)
    , gainRelay("gain")
    , cutoffRelay("cutoff")
    , bypassRelay("bypass")
    , filterTypeRelay("filterType")

    // Initialize WebView with options
    , webView(juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()  // CRITICAL: Enables JUCE JavaScript library
        .withResourceProvider([this](const auto& url) { return getResource(url); })
        .withOptionsFrom(gainRelay)      // Register each relay
        .withOptionsFrom(cutoffRelay)
        .withOptionsFrom(bypassRelay)
        .withOptionsFrom(filterTypeRelay)
    )

    // Initialize attachments (connect parameters to relays)
    , gainAttachment(*processorRef.parameters.getParameter("gain"), gainRelay)
    , cutoffAttachment(*processorRef.parameters.getParameter("cutoff"), cutoffRelay)
    , bypassAttachment(*processorRef.parameters.getParameter("bypass"), bypassRelay)
    , filterTypeAttachment(*processorRef.parameters.getParameter("filterType"), filterTypeRelay)
{
    // Add WebView to editor
    addAndMakeVisible(webView);

    // Navigate to UI
    webView.goToURL(juce::WebBrowserComponent::getResourceProviderRoot());

    // Set editor size (from UI mockup dimensions)
    setSize(600, 400);  // Adjust to match mockup
}
```

**Key points:**
- Initialize members in SAME order as declaration
- Register ALL relays with `.withOptionsFrom(relay)`
- Use exact parameter IDs from APVTS
- Enable native integration (required for JUCE JavaScript library)

### 8. Implement Resource Provider

**Edit `Source/PluginEditor.cpp`:**

Add resource serving function:

```cpp
#include "BinaryData.h"  // Generated by CMake from juce_add_binary_data

std::optional<juce::WebBrowserComponent::Resource>
[PluginName]AudioProcessorEditor::getResource(const juce::String& url)
{
    // Map URLs to embedded resources
    auto resource = url.replaceCharacter('\\', '/');

    // Root "/" → index.html
    if (resource == "/" || resource.isEmpty())
        resource = "/index.html";

    // Remove leading slash for BinaryData lookup
    auto path = resource.substring(1);

    // Find in binary data (files embedded from ui/public/)
    for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
    {
        if (path == BinaryData::namedResourceList[i])
        {
            int dataSize = 0;
            const char* data = BinaryData::getNamedResource(
                BinaryData::namedResourceList[i], dataSize);

            // Determine MIME type
            juce::String mimeType = "text/html";
            if (path.endsWith(".css")) mimeType = "text/css";
            if (path.endsWith(".js")) mimeType = "application/javascript";
            if (path.endsWith(".png")) mimeType = "image/png";
            if (path.endsWith(".jpg") || path.endsWith(".jpeg")) mimeType = "image/jpeg";
            if (path.endsWith(".svg")) mimeType = "image/svg+xml";

            return juce::WebBrowserComponent::Resource{
                data, dataSize, mimeType
            };
        }
    }

    // Resource not found
    juce::Logger::writeToLog("Resource not found: " + url);
    return std::nullopt;
}
```

**This function:**
- Serves files embedded in plugin binary
- Maps URLs to BinaryData resources
- Sets correct MIME types (critical for .js files)

### 9. Update CMakeLists.txt for WebView

**Edit `CMakeLists.txt`:**

```cmake
# Enable WebView support
juce_add_plugin([PluginName]
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Plug
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "[Plugin Name]"
    NEEDS_WEB_BROWSER TRUE        # ← Enable WebView
    NEEDS_WEBVIEW2 TRUE           # ← Enable WebView2 (Windows)
)

# Embed UI files as binary data
juce_add_binary_data([PluginName]Data
    SOURCES
        ui/public/index.html
        ui/public/css/styles.css
        ui/public/js/app.js
        ui/public/js/juce.js
        # Add any images, fonts, etc.
)

# Link binary data to plugin
target_link_libraries([PluginName]
    PRIVATE
        [PluginName]Data  # ← Link embedded UI files
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra  # ← Contains WebBrowserComponent
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compile definitions for WebView
target_compile_definitions([PluginName]
    PUBLIC
        JUCE_WEB_BROWSER=1      # ← Enable WebBrowserComponent
        JUCE_USE_CURL=0         # ← Disable CURL (not needed)
        JUCE_VST3_CAN_REPLACE_VST2=0
)
```

**Critical changes:**
- `NEEDS_WEB_BROWSER TRUE` - Enables WebView support
- `NEEDS_WEBVIEW2 TRUE` - Enables WebView2 on Windows
- `juce_add_binary_data` - Embeds UI files in binary
- `JUCE_WEB_BROWSER=1` - Enables WebBrowserComponent API

### 10. Update resized() for Layout

**Edit `Source/PluginEditor.cpp`:**

```cpp
void [PluginName]AudioProcessorEditor::resized()
{
    // WebView fills entire editor
    webView.setBounds(getLocalBounds());
}
```

**Remove paint() implementation** (WebView handles all rendering):

```cpp
void [PluginName]AudioProcessorEditor::paint(juce::Graphics& g)
{
    // WebView handles all painting
    juce::ignoreUnused(g);
}
```

### 11. Self-Validation

**Verify UI integration (code only, build and DAW testing handled by plugin-workflow):**

1. **File verification:**
   - ✅ `ui/public/index.html` exists
   - ✅ `ui/public/js/juce.js` exists
   - ✅ `ui/public/js/app.js` exists (parameter binding code)
   - ✅ All CSS/image assets copied

2. **Code verification:**
   - ✅ Read `Source/PluginEditor.h`
   - ✅ Verify member order: Relays → WebView → Attachments
   - ✅ Count relays: One per parameter from parameter-spec.md
   - ✅ Count attachments: One per parameter
   - ✅ Verify relay IDs match parameter-spec.md IDs exactly

3. **CMake verification:**
   - ✅ `NEEDS_WEB_BROWSER TRUE` present
   - ✅ `juce_add_binary_data` includes all UI files
   - ✅ `JUCE_WEB_BROWSER=1` defined

**Extract parameter IDs from code:**
```regex
WebSliderRelay\s+(\w+)Relay\s*\{\s*"(\w+)"\s*\}
```

**Compare with parameter-spec.md:** All IDs must match exactly.

**Note:** Build verification, plugin installation, and DAW testing handled by plugin-workflow via build-automation skill after gui-agent completes. This agent only creates/modifies UI code and configuration.

### 12. Return Report

**On success:**

```json
{
  "agent": "gui-agent",
  "status": "success",
  "outputs": {
    "plugin_name": "[PluginName]",
    "ui_mockup_version": "v3",
    "ui_dimensions": "600x400",
    "bindings_implemented": [
      {
        "parameter_id": "gain",
        "type": "WebSliderRelay",
        "html_element": "input#gain[type=range]"
      },
      {
        "parameter_id": "bypass",
        "type": "WebToggleButtonRelay",
        "html_element": "input#bypass[type=checkbox]"
      }
    ],
    "binding_count": 5,
    "all_parameters_bound": true,
    "member_order_correct": true,
    "webview_enabled": true,
    "build_log_path": "logs/[PluginName]/build-[timestamp].log"
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

**On validation failure:**

```json
{
  "agent": "gui-agent",
  "status": "failure",
  "outputs": {
    "plugin_name": "[PluginName]",
    "error_type": "binding_mismatch",
    "missing_bindings": ["cutoff", "resonance"],
    "parameter_spec_ids": ["gain", "cutoff", "resonance", "mix", "bypass"],
    "implemented_relay_ids": ["gain", "mix", "bypass"],
    "expected_count": 5,
    "actual_count": 3
  },
  "issues": [
    "Binding validation failed: 2 parameters not bound to UI",
    "Missing relays: cutoff, resonance",
    "All parameters from parameter-spec.md must have relay + attachment",
    "Contract violation: zero-drift guarantee broken"
  ],
  "ready_for_next_stage": false
}
```

**On build failure:**

```json
{
  "agent": "gui-agent",
  "status": "failure",
  "outputs": {
    "plugin_name": "[PluginName]",
    "error_type": "compilation_error",
    "error_message": "[First error from build log]",
    "build_log_path": "logs/[PluginName]/build-[timestamp].log",
    "likely_cause": "WebView not enabled in CMake or missing JUCE module"
  },
  "issues": [
    "Build failed during Stage 5",
    "Common causes: NEEDS_WEB_BROWSER not set, juce_gui_extra not linked, wrong member order",
    "See build log for details"
  ],
  "ready_for_next_stage": false
}
```

## Relay Pattern Reference

**Three relay types for three parameter types:**

| Parameter Type | JUCE Class | Relay Class | JavaScript Function |
|----------------|------------|-------------|---------------------|
| Float (continuous) | `juce::AudioParameterFloat` | `juce::WebSliderRelay` | `Juce.getSliderState(id)` |
| Bool (toggle) | `juce::AudioParameterBool` | `juce::WebToggleButtonRelay` | `Juce.getToggleState(id)` |
| Choice (discrete) | `juce::AudioParameterChoice` | `juce::WebComboBoxRelay` | `Juce.getComboBoxState(id)` |

**Each parameter requires:**
1. Relay member (e.g., `juce::WebSliderRelay gainRelay`)
2. Attachment member (e.g., `juce::WebSliderParameterAttachment gainAttachment`)
3. Registration in WebView options (`.withOptionsFrom(gainRelay)`)
4. JavaScript binding code in `app.js`

## Common Issues

**Blank WebView:**
- Missing `.withNativeIntegrationEnabled()`
- Resource provider returns `std::nullopt` for all files
- Wrong MIME type for .js files (must be `application/javascript`)

**Parameters don't sync:**
- Parameter IDs don't match (C++ vs JavaScript)
- Relay not registered with `.withOptionsFrom()`
- Wrong relay type for parameter type

**Crashes on reload:**
- Wrong member declaration order (attachments before webView)
- Destructor tries to use already-destroyed webView

**UI doesn't update from automation:**
- Missing `valueChangedEvent.addListener()` in JavaScript
- Event listener not updating HTML element

## Success Criteria

**Stage 5 succeeds when:**
1. Finalized UI mockup integrated (HTML/CSS/JS in ui/public/)
2. All parameters from parameter-spec.md have relay + attachment
3. Member declaration order correct (Relays → WebView → Attachments)
4. CMake configured for WebView (NEEDS_WEB_BROWSER, binary data)
5. Build completes without errors
6. WebView renders UI correctly in DAW
7. All parameter controls functional (bidirectional sync)
8. Automation works (parameters change UI)
9. Presets work (load updates UI)
10. Plugin doesn't crash on reload

**Stage 5 fails when:**
- No finalized UI mockup found (blocking error)
- Missing bindings (parameters without relay/attachment)
- Wrong member order (causes release build crashes)
- WebView blank or shows HTML source
- Parameters don't respond to UI
- Build errors related to WebView

## Next Stage

After Stage 5 succeeds:
1. **Auto-invoke plugin-testing skill** (5 automated tests including UI validation)
2. **If tests FAIL:** STOP, show results, wait for fixes
3. **If tests PASS:** Continue to Stage 6 (final validation and release)

The plugin is now COMPLETE:
- ✅ Build system (Stage 2)
- ✅ Parameter system (Stage 3)
- ✅ Audio processing (Stage 4)
- ✅ UI integration (Stage 5)
- ⏳ Final validation (Stage 6 - next)
