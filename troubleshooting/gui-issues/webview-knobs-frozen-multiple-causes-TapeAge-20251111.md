---
plugin: TapeAge
date: 2025-11-11
problem_type: ui_layout
component: webview
symptoms:
  - WebView UI displays correctly but knobs completely frozen (no response to mouse drag)
  - VST3 plugin missing from DAW (only AU format visible)
  - Parameters don't update when knobs are moved
  - Audio processing works with default parameter values
root_cause: wrong_api
juce_version: 8.0.9
resolution_type: code_fix
severity: critical
tags: [webview, parameter-binding, juce8-api, vst3, component-id, frozen-ui]
---

# WebView Knobs Frozen - Three Root Causes (JUCE 8 API Changes)

## Problem

TapeAge plugin WebView UI displayed correctly with visible knobs and VU meter, but all three parameter knobs were completely unresponsive to mouse interaction. Additionally, VST3 format didn't appear in DAW plugin list (only AU visible). Deep parallel investigation revealed THREE separate root causes that all needed fixing.

## Environment

- Plugin: TapeAge (vintage tape saturator)
- JUCE Version: 8.0.9
- Affected: WebView UI parameter binding (Stage 5 complete)
- OS: macOS Darwin 24.6.0
- DAW: Ableton Live
- Date: 2025-11-11

## Symptoms

- **UI displays correctly**: WebView loads, shows vintage tape aesthetic with three knobs and VU meter
- **Knobs completely frozen**: No response to mouse drag on any of the three parameters (drive, age, mix)
- **Parameters don't update**: Moving knobs doesn't change parameter values
- **Audio processing works**: Plugin processes audio with default parameter values
- **VST3 missing from DAW**: Only AU format appears in Ableton plugin list
- **No compiler errors or warnings**: Code compiled successfully, appeared to be correct

## What Didn't Work

**Attempted Solution 1: Changed HTML module import path**
- Changed `"./js/juce/index.js"` to `"./index.js"` based on BinaryData flattening behavior
- **Why it failed**: The original path was correct - getResource() handles URL mapping explicitly
- **Impact**: Wasted time, had to revert

**Attempted Solution 2: Added NEEDS_WEB_BROWSER flag**
- Added `NEEDS_WEB_BROWSER TRUE` to CMakeLists.txt
- **Why it partially worked**: Fixed VST3 not appearing, but didn't fix frozen knobs
- **Impact**: Solved one of three issues

**Attempted Solution 3: Converted to std::unique_ptr<>**
- Changed raw member declarations to `std::unique_ptr<>` for relays, webView, attachments
- **Why it partially worked**: Proper member initialization order, but didn't fix parameter binding
- **Impact**: Good practice but not the root cause

**CRITICAL ERROR: Testing stale builds**
- Used `--no-install` flag repeatedly, so DAW loaded OLD plugins from system folders
- All fixes appeared broken because they were never actually installed
- **User feedback**: "you stupidly weren't actually installing it"
- **Impact**: Couldn't verify ANY fixes until final install

**Attempted Solution 4: Level 3 Parallel Deep Research**
- Spawned 3 parallel investigation agents to explore different angles
- **VST3 Investigation Agent**: Found PLUGIN_CODE duplicate issue
- **UI Freeze Investigation Agent**: Found missing nullptr parameter
- **Binary Comparison Agent**: Found missing check_native_interop.js
- **Why it worked**: Comprehensive multi-angle investigation revealed ALL THREE root causes

## Solution

**THREE separate fixes were required:**

### Fix 1: Missing nullptr in WebSliderParameterAttachment (JUCE 8 API Change)

**Code change - PluginEditor.cpp lines 25-30:**

```cpp
// BEFORE (JUCE 7 style - 2 parameters):
driveAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("drive"), *driveRelay);
ageAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("age"), *ageRelay);
mixAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("mix"), *mixRelay);

// AFTER (JUCE 8 style - 3 parameters with undoManager):
driveAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("drive"), *driveRelay, nullptr);
ageAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("age"), *ageRelay, nullptr);
mixAttachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *processorRef.parameters.getParameter("mix"), *mixRelay, nullptr);
```

### Fix 2: Missing check_native_interop.js File

**File copy:**
```bash
# Copy critical JUCE WebView initialization file from working plugin
cp plugins/GainKnob/Source/ui/public/js/juce/check_native_interop.js \
   plugins/TapeAge/Source/ui/public/js/juce/
```

**CMakeLists.txt change - line 27-31:**
```cmake
# BEFORE (missing check_native_interop.js):
juce_add_binary_data(TapeAge_UIResources
    SOURCES
        Source/ui/public/index.html
        Source/ui/public/js/juce/index.js
)

# AFTER (added check_native_interop.js):
juce_add_binary_data(TapeAge_UIResources
    SOURCES
        Source/ui/public/index.html
        Source/ui/public/js/juce/index.js
        Source/ui/public/js/juce/check_native_interop.js
)
```

**PluginEditor.cpp resource handler - after line 98:**
```cpp
// BEFORE (only serving 2 files):
if (url == "/js/juce/index.js") {
    return juce::WebBrowserComponent::Resource {
        makeVector(BinaryData::index_js, BinaryData::index_jsSize),
        juce::String("text/javascript")
    };
}
// Missing handler for check_native_interop.js

// AFTER (added handler):
if (url == "/js/juce/index.js") {
    return juce::WebBrowserComponent::Resource {
        makeVector(BinaryData::index_js, BinaryData::index_jsSize),
        juce::String("text/javascript")
    };
}

// JUCE native interop checker
if (url == "/js/juce/check_native_interop.js") {
    return juce::WebBrowserComponent::Resource {
        makeVector(BinaryData::check_native_interop_js,
                  BinaryData::check_native_interop_jsSize),
        juce::String("text/javascript")
    };
}
```

### Fix 3: Generic PLUGIN_CODE Creating Duplicate Component ID

**CMakeLists.txt changes:**

```cmake
# BEFORE (line 7 - generic code creates duplicate VST3 Component ID):
PLUGIN_CODE Plug

# AFTER (unique code):
PLUGIN_CODE Tape
```

```cmake
# BEFORE (line 9 - space in product name causes issues):
PRODUCT_NAME "TAPE AGE"

# AFTER (no space):
PRODUCT_NAME "TapeAge"
```

### Rebuild and Install

```bash
# Clean build artifacts
rm -rf build/plugins/TapeAge/

# Build and install to system folders (NO --no-install flag!)
./scripts/build-and-install.sh TapeAge

# Manually remove old plugins with old product name
rm -rf ~/Library/Audio/Plug-Ins/VST3/"TAPE AGE.vst3"
rm -rf ~/Library/Audio/Plug-Ins/Components/"TAPE AGE.component"

# Clear DAW caches
killall -9 AudioComponentRegistrar
rm ~/Library/Preferences/Ableton/*/PluginDatabase.cfg

# Restart DAW for plugin rescan
```

## Why This Works

### Root Cause 1: JUCE 8 Changed WebSliderParameterAttachment Constructor

**What changed:**
- JUCE 7: `WebSliderParameterAttachment(parameter, relay)`
- JUCE 8: `WebSliderParameterAttachment(parameter, relay, undoManager)`

**Why it failed silently:**
- Missing third parameter causes template instantiation to succeed but binding to fail
- No compiler error because overload resolution still works
- Runtime initialization appears successful but parameter updates don't propagate
- Results in one-way communication failure (UI → C++ broken, C++ → UI may work)

**Why nullptr works:**
- Third parameter is `UndoManager*` for undo/redo support
- `nullptr` means "no undo manager" which is fine for basic parameter binding
- Proper three-parameter construction enables full bidirectional parameter binding

### Root Cause 2: Missing WebView Native Bridge Initialization

**What check_native_interop.js does:**
- JUCE's verification script that tests C++ ↔ JavaScript bridge functionality
- Checks that `__JUCE__` native object is available in JavaScript context
- Validates that native methods can be called from JS
- Sets up proper event listeners for parameter updates

**Why missing it breaks things:**
- WebView loads HTML/CSS correctly (visual display works)
- Native integration initialization fails silently
- JavaScript can't communicate with C++ parameter system
- Results in frozen UI that displays but doesn't respond

**Why adding it fixes:**
- Completes WebView native bridge initialization sequence
- Enables JavaScript to call C++ methods through `__JUCE__` object
- Establishes bidirectional communication channel for parameter updates
- Activates mouse event handlers that update parameters

### Root Cause 3: Duplicate VST3 Component ID

**How Component IDs work:**
- VST3 uses unique Component ID (CID) to identify each plugin
- CID is generated from PLUGIN_CODE (4-character code)
- DAW maintains registry of all installed plugins by CID
- Duplicate CIDs cause DAW to reject the plugin silently

**Why "Plug" was wrong:**
- Generic code like "Plug" likely used by other plugins or examples
- Creates hash collision with existing plugin
- VST3 scanner detects duplicate and ignores it
- AU format uses different identification system, so it still appears

**Why "Tape" works:**
- Unique 4-character code generates unique Component ID
- No hash collision with existing plugins
- VST3 scanner accepts and registers the plugin
- Both VST3 and AU formats now appear in DAW

## Prevention

### 1. Always Check JUCE API Changes Between Versions

When upgrading JUCE or using newer templates:
- Compare working plugin (GainKnob) constructor signatures
- Check JUCE docs for API changes in WebView components
- Don't assume JUCE 7 patterns still work in JUCE 8

**Specific check for WebSliderParameterAttachment:**
```cpp
// Always verify constructor signature matches JUCE version
// JUCE 8 requires: (parameter, relay, undoManager)
attachment = std::make_unique<juce::WebSliderParameterAttachment>(
    *param, *relay, nullptr  // Third parameter required in JUCE 8
);
```

### 2. Include check_native_interop.js in ALL WebView Plugins

**Checklist when creating new WebView plugin:**
- [ ] Copy `check_native_interop.js` from GainKnob or JUCE examples
- [ ] Add to `juce_add_binary_data()` sources in CMakeLists.txt
- [ ] Add resource handler in `getResource()` method
- [ ] Test that file is served (check browser console for 404s)

**File location:**
```
plugins/[PluginName]/Source/ui/public/js/juce/check_native_interop.js
```

### 3. Use Unique PLUGIN_CODE for Every Plugin

**Never use generic codes:**
- ❌ "Plug" - Too generic, likely to collide
- ❌ "Test" - Obvious collision risk
- ❌ "Fx01" - Generic pattern

**Use plugin-specific codes:**
- ✅ "Tape" - Unique to TapeAge
- ✅ "Gain" - Unique to GainKnob
- ✅ "Dely" - Unique to TapeDelay (4 chars max)

**Verification:**
```bash
# Check for duplicates across all plugins
grep "PLUGIN_CODE" plugins/*/CMakeLists.txt | sort
# Each line should have unique 4-char code
```

### 4. ALWAYS Install When Testing GUI Changes

**CRITICAL WORKFLOW:**
```bash
# ❌ WRONG - Tests stale cached builds
./scripts/build-and-install.sh PluginName --no-install
# DAW loads OLD version from system folders

# ✅ CORRECT - Tests fresh build
./scripts/build-and-install.sh PluginName
# Script installs to system folders AND clears DAW caches
```

**Why this matters:**
- DAWs load plugins from system folders (`~/Library/Audio/Plug-Ins/`), NOT build directories
- Using `--no-install` means testing old builds while developing new code
- All "fixes" appear broken when testing stale binaries

**After ANY of these changes, MUST reinstall:**
- PluginEditor code (UI changes)
- CMakeLists.txt (configuration)
- Resource files (HTML/CSS/JS)
- Parameter bindings

### 5. Use Parallel Deep Research for Complex Multi-Symptom Issues

When a problem has multiple symptoms (VST3 missing AND knobs frozen):
- Launch parallel investigation agents to explore different angles
- Each agent investigates one hypothesis independently
- Compare findings to identify if multiple root causes exist
- Don't assume single fix will solve everything

**In this case:**
- VST3 missing → Component ID investigation → Found PLUGIN_CODE issue
- Knobs frozen → Parameter binding investigation → Found nullptr issue
- Knobs frozen → Binary comparison → Found missing check_native_interop.js

All three issues needed fixing. Single-threaded investigation would have missed some.

### 6. Clean Up When Changing PRODUCT_NAME

When renaming plugin (changing PRODUCT_NAME in CMakeLists.txt):
```bash
# BEFORE changing PRODUCT_NAME, remove old versions:
rm -rf ~/Library/Audio/Plug-Ins/VST3/"OLD NAME.vst3"
rm -rf ~/Library/Audio/Plug-Ins/Components/"OLD NAME.component"

# THEN change PRODUCT_NAME and rebuild
# Build script can't find old versions because it searches for NEW name
```

## Related Issues

- See also: [webview-frame-load-interrupted-TapeAge-20251111.md](webview-frame-load-interrupted-TapeAge-20251111.md) - Earlier WebView loading issue
- See also: [webview-parameter-not-updating-wrong-event-format-JUCE-20251108.md](../api-usage/webview-parameter-not-updating-wrong-event-format-JUCE-20251108.md) - Different WebView parameter issue (event format mismatch)
- Related pattern: troubleshooting/patterns/juce8-critical-patterns.md #12 (WebSliderParameterAttachment three parameters)
- Related pattern: troubleshooting/patterns/juce8-critical-patterns.md #13 (check_native_interop.js required)

## References

- JUCE 8 WebSliderParameterAttachment API: Changed constructor signature (added undoManager parameter)
- Working reference: plugins/GainKnob/ (all three fixes already implemented correctly)
- VST3 Component ID specification: 4-character codes must be globally unique
- DAW plugin caching: System folders are source of truth, build directories are ignored
