---
plugin: TapeAge
date: 2025-11-11
problem_type: ui_layout
component: webview
symptoms:
  - Frame load interrupted error in WebView
  - VST3 plugin not appearing in DAW (only AU visible)
  - White screen with error message instead of UI
root_cause: url_protocol
juce_version: 8.0.9
resolution_type: code_fix
severity: critical
tags: [webview, vst3, resource-provider, frame-load, gui]
---

# WebView "Frame Load Interrupted" - VST3 Missing, UI Not Loading

## Problem

WebView showed "Frame load interrupted" error message on white screen. VST3 plugin didn't appear in Ableton (only AU format visible). Plugin audio processing worked correctly, but UI was completely broken.

## Context

- **Plugin:** TapeAge (Stage 5 - GUI integration complete)
- **JUCE Version:** 8.0.9
- **OS:** macOS (Darwin 24.6.0)
- **DAW:** Ableton Live
- **Stage:** Stage 5 complete (WebView integration with 3 parameters + VU meter)

## Symptoms

1. **Frame load interrupted** - WebView displays error instead of UI
2. **VST3 missing** - Only AU format appears in DAW plugin list
3. **White screen** - No vintage tape aesthetic, just error text
4. **Parameters work** - Audio processing functional (user confirmed "sounds amazing")

## Investigation Attempts

### Attempt 1: Module Import Path Fix (WRONG)
Changed HTML import from `"./js/juce/index.js"` → `"./index.js"` based on BinaryData flattening.

**Result:** Didn't fix (but wasn't testing installed version - testing error)

### Attempt 2: Added NEEDS_WEB_BROWSER Flag
Added `NEEDS_WEB_BROWSER TRUE` to CMakeLists.txt `juce_add_plugin()`.

**Result:** Unknown (not installed to system)

### Attempt 3: Converted to std::unique_ptr<>
Changed raw member declarations to `std::unique_ptr<>` for relays, webView, and attachments.

**Result:** Unknown (not installed to system)

### Attempt 4: Rewrote getResource() + Reverted Import
- Reverted import path back to `"./js/juce/index.js"`
- Rewrote getResource() to explicitly map URLs (matching GainKnob pattern)
- Added `.withKeepPageLoadedWhenBrowserIsHidden()` for FL Studio compatibility

**Result:** SUCCESS (after actually installing to system folders)

## Root Cause

**MULTIPLE ISSUES** (cannot isolate which was critical due to testing error):

1. **Missing `NEEDS_WEB_BROWSER TRUE`** - Required for VST3 WebView support
2. **getResource() URL mapping broken** - Generic loop couldn't match requested paths
3. **Possible member initialization order** - Raw members vs unique_ptr
4. **Testing stale builds** - Used `--no-install` flag, never updated system plugins

**Primary suspect:** getResource() implementation. GainKnob uses explicit URL mapping:

```cpp
// ❌ WRONG (TapeAge original)
for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
    if (path == BinaryData::namedResourceList[i]) { /* ... */ }
}
```

```cpp
// ✅ CORRECT (GainKnob pattern)
if (url == "/js/juce/index.js") {
    return juce::WebBrowserComponent::Resource {
        makeVector(BinaryData::index_js, BinaryData::index_jsSize),
        juce::String("text/javascript")
    };
}
```

**Critical testing error:** Rebuilding without installing (`--no-install` flag) meant DAW was loading OLD broken builds from system folders while testing. All fixes were correct but never actually installed until final attempt.

## Solution

**Complete rewrite matching GainKnob working pattern:**

### 1. CMakeLists.txt - Add NEEDS_WEB_BROWSER

```cmake
juce_add_plugin(TapeAge
    COMPANY_NAME "YourCompany"
    PLUGIN_MANUFACTURER_CODE Manu
    PLUGIN_CODE Plug
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "TAPE AGE"
    NEEDS_WEB_BROWSER TRUE  # ← Add this
)
```

### 2. PluginEditor.h - Use std::unique_ptr<>

```cpp
// Relays
std::unique_ptr<juce::WebSliderRelay> driveRelay;
std::unique_ptr<juce::WebSliderRelay> ageRelay;
std::unique_ptr<juce::WebSliderRelay> mixRelay;

// WebView
std::unique_ptr<juce::WebBrowserComponent> webView;

// Attachments
std::unique_ptr<juce::WebSliderParameterAttachment> driveAttachment;
std::unique_ptr<juce::WebSliderParameterAttachment> ageAttachment;
std::unique_ptr<juce::WebSliderParameterAttachment> mixAttachment;
```

### 3. PluginEditor.cpp - Explicit getResource() Mapping

```cpp
std::optional<juce::WebBrowserComponent::Resource>
TapeAgeAudioProcessorEditor::getResource(const juce::String& url)
{
    auto makeVector = [](const char* data, int size) {
        return std::vector<std::byte>(
            reinterpret_cast<const std::byte*>(data),
            reinterpret_cast<const std::byte*>(data) + size
        );
    };

    // Root URL
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
            juce::String("text/javascript")
        };
    }

    return std::nullopt;  // 404
}
```

### 4. PluginEditor.cpp - Add FL Studio Fix

```cpp
webView = std::make_unique<juce::WebBrowserComponent>(
    juce::WebBrowserComponent::Options{}
        .withNativeIntegrationEnabled()
        .withResourceProvider([this](const auto& url) { return getResource(url); })
        .withKeepPageLoadedWhenBrowserIsHidden()  // ← Add this
        .withOptionsFrom(*driveRelay)
        .withOptionsFrom(*ageRelay)
        .withOptionsFrom(*mixRelay)
);
```

### 5. index.html - Keep Original Import Path

```javascript
// ✅ CORRECT (matches getResource() mapping)
import * as Juce from "./js/juce/index.js";
```

### 6. **CRITICAL: Actually Install to System**

```bash
./scripts/build-and-install.sh TapeAge  # NO --no-install flag!
```

Script performs:
- Phase 1-2: Build VST3 + AU
- Phase 3-5: Install to system folders, remove old versions
- Phase 6: Clear DAW caches (Ableton plugin database, AU cache)
- Phase 7: Verification

**Then restart DAW** to rescan plugins.

## Prevention

### 1. Always Compare to Working Plugin

When WebView fails, diff against known-working plugin (GainKnob):

```bash
diff plugins/GainKnob/CMakeLists.txt plugins/TapeAge/CMakeLists.txt
diff plugins/GainKnob/Source/PluginEditor.cpp plugins/TapeAge/Source/PluginEditor.cpp
```

### 2. Use Explicit URL Mapping

**Never use generic loops for getResource()** - explicit mapping is clearer and debuggable:

```cpp
// ❌ DON'T: Generic loop (hard to debug, easy to break)
for (int i = 0; i < BinaryData::namedResourceListSize; ++i) {
    if (path == BinaryData::namedResourceList[i]) { /* ... */ }
}

// ✅ DO: Explicit mapping (clear, debuggable, reliable)
if (url == "/js/juce/index.js") {
    return juce::WebBrowserComponent::Resource { /* ... */ };
}
```

### 3. ALWAYS Install When Testing GUI Changes

**CRITICAL:** Never use `--no-install` when testing WebView changes. DAW loads from system folders, not build directory.

```bash
# ❌ WRONG (tests stale build)
./scripts/build-and-install.sh PluginName --no-install

# ✅ CORRECT (tests fresh build)
./scripts/build-and-install.sh PluginName
```

### 4. Check Binary Data Embedded Correctly

Verify resources are embedded:

```bash
# Check what's in BinaryData
grep "extern const char" build/plugins/PluginName/juce_binarydata_*/BinaryData.h

# Should see:
# extern const char* index_html;
# extern const char* index_js;
```

### 5. Required CMake Settings for WebView

```cmake
# In juce_add_plugin()
NEEDS_WEB_BROWSER TRUE  # ← Required for VST3

# In target_compile_definitions()
JUCE_WEB_BROWSER=1      # ← Enable WebView
JUCE_USE_CURL=0         # ← Disable CURL (not needed for embedded resources)

# In target_link_libraries()
juce::juce_gui_extra    # ← Required for WebBrowserComponent
```

### 6. Clear DAW Caches After Install

```bash
# Ableton plugin database
rm ~/Library/Preferences/Ableton/*/PluginDatabase.cfg

# AU cache
killall -9 AudioComponentRegistrar
```

## Related Issues

- See also: [webview-knobs-frozen-multiple-causes-TapeAge-20251111.md](webview-knobs-frozen-multiple-causes-TapeAge-20251111.md) - Follow-up issue: frozen knobs after UI loaded (THREE root causes: missing nullptr, missing check_native_interop.js, duplicate PLUGIN_CODE)
- See GainKnob implementation (fully working reference)
- BinaryData flattens directory structure (why explicit mapping needed)

## References

- JUCE WebBrowserComponent resource provider pattern
- GainKnob: `plugins/GainKnob/Source/PluginEditor.cpp` (working reference)
- Build script: `scripts/build-and-install.sh` (7-phase pipeline)
