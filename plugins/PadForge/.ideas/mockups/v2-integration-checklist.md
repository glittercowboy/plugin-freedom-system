# Stage 3 (GUI) Integration Checklist - v2

**Plugin:** PadForge
**Mockup Version:** v2
**Generated:** 2025-11-14
**Parameters:** 4 parameters (volume, brightness, space, randomizeAmount)

## Prerequisites

- [x] UI design finalized (v2-ui.yaml with finalized: true marker)
- [x] parameter-spec.md created (generated during v2 finalization)
- [ ] Plugin shell exists (Stage 1 complete - Foundation)
- [ ] DSP implemented (Stage 2 complete - Audio Engine)

## 1. Copy UI Files

```bash
# Create UI directory structure
mkdir -p plugins/PadForge/Source/ui/public/js/juce

# Copy production HTML
cp plugins/PadForge/.ideas/mockups/v2-ui.html \
   plugins/PadForge/Source/ui/public/index.html

# Copy JUCE frontend library (from working plugin or JUCE examples)
# Example source: GainKnob plugin or JUCE/examples/GUI/WebBrowserDemo
cp /path/to/juce/frontend/index.js \
   plugins/PadForge/Source/ui/public/js/juce/index.js

cp /path/to/juce/frontend/check_native_interop.js \
   plugins/PadForge/Source/ui/public/js/juce/check_native_interop.js
```

**Verify:**

- [ ] index.html exists at `Source/ui/public/index.html`
- [ ] index.js exists at `Source/ui/public/js/juce/index.js`
- [ ] check_native_interop.js exists at `Source/ui/public/js/juce/check_native_interop.js`
- [ ] HTML contains production code (no debug monitors)
- [ ] HTML uses ES6 module imports (`type="module"`)

## 2. Update PluginEditor Files

**Replace PluginEditor.h:**

```bash
# Backup existing (if any)
cp plugins/PadForge/Source/PluginEditor.h \
   plugins/PadForge/Source/PluginEditor.h.backup 2>/dev/null || true

# Use v2 template as reference (adapt class names)
# Copy content from v2-PluginEditor-TEMPLATE.h
# Update class name: PadForgeAudioProcessorEditor
```

**Key points to verify in PluginEditor.h:**

- [ ] Member order: relays → webView → attachments
- [ ] 4 relay declarations (volume, brightness, space, randomizeAmount)
- [ ] 1 webView declaration
- [ ] 4 attachment declarations (matching relays)
- [ ] Class name matches: `PadForgeAudioProcessorEditor`
- [ ] Includes: `<JuceHeader.h>` and `"PluginProcessor.h"`

**Replace PluginEditor.cpp:**

```bash
# Backup existing (if any)
cp plugins/PadForge/Source/PluginEditor.cpp \
   plugins/PadForge/Source/PluginEditor.cpp.backup 2>/dev/null || true

# Use v2 template as reference (adapt class names)
# Copy content from v2-PluginEditor-TEMPLATE.cpp
```

**Key points to verify in PluginEditor.cpp:**

- [ ] Relay creation BEFORE WebView construction
- [ ] All 4 relays created with correct parameter IDs
- [ ] WebView options include all 4 relays (`.withOptionsFrom()`)
- [ ] All 4 attachments created AFTER WebView
- [ ] Attachment parameter IDs match APVTS (from parameter-spec.md)
- [ ] Window size set to 400×350 (from YAML)
- [ ] Resource provider returns all 3 files (index.html, index.js, check_native_interop.js)
- [ ] MIME types correct: `text/html`, `application/javascript`

## 3. Update CMakeLists.txt

**Append v2-CMakeLists-SNIPPET.txt to CMakeLists.txt:**

```bash
# Backup existing
cp plugins/PadForge/CMakeLists.txt \
   plugins/PadForge/CMakeLists.txt.backup

# Append WebView configuration
cat plugins/PadForge/.ideas/mockups/v2-CMakeLists-SNIPPET.txt >> \
    plugins/PadForge/CMakeLists.txt
```

**Verify CMakeLists.txt contains:**

- [ ] `juce_add_binary_data(PadForge_UIResources ...)` with all UI files
- [ ] `target_link_libraries(PadForge PRIVATE PadForge_UIResources juce::juce_gui_extra)`
- [ ] `target_compile_definitions(PadForge PUBLIC JUCE_WEB_BROWSER=1 JUCE_USE_CURL=0)`
- [ ] `NEEDS_WEB_BROWSER TRUE` in `juce_add_plugin()` configuration
- [ ] `IS_SYNTH TRUE` in `juce_add_plugin()` (PadForge is a synth)

## 4. Build and Test (Debug)

```bash
# Clean build (recommended after CMake changes)
rm -rf build/plugins/PadForge
cmake --build build --target PadForge_Standalone --parallel

# Or use build script
./scripts/build-and-install.sh PadForge
```

**Verify build:**

- [ ] Build succeeds without warnings
- [ ] No WebView-related errors
- [ ] Binary data embedded correctly
- [ ] CMake finds all UI resources

**Test standalone:**

```bash
# Run standalone app (macOS)
./build/plugins/PadForge/PadForge_artefacts/Debug/Standalone/PadForge.app/Contents/MacOS/PadForge
```

**Check standalone:**

- [ ] Window opens at 400×350 pixels
- [ ] WebView loads (not blank)
- [ ] All 4 knobs visible and positioned correctly
- [ ] RANDOMIZE button visible and centered
- [ ] Preset dropdown + Save/Load buttons visible
- [ ] Right-click → Inspect works (for debugging)
- [ ] Console shows "JUCE backend connected"
- [ ] Console shows "PadForge UI initialized"
- [ ] No JavaScript errors in console

## 5. Build and Test (Release)

```bash
# Build release
cmake --build build --target PadForge_Standalone --config Release --parallel
```

**Critical release build tests:**

- [ ] Release build succeeds
- [ ] No crashes on plugin load (test 10 times)
- [ ] No crashes on plugin reload in DAW
- [ ] Member order correct (prevents release-only crashes)
- [ ] UI still responsive (no frozen knobs)

## 6. Test Parameter Binding

**In standalone or DAW:**

- [ ] Volume knob syncs UI ↔ APVTS (0.0-1.0, default 0.7)
- [ ] Brightness knob syncs UI ↔ APVTS (0.0-1.0, default 0.5)
- [ ] Space knob syncs UI ↔ APVTS (0.0-1.0, default 0.3)
- [ ] Randomize Amount knob syncs UI ↔ APVTS (0.0-1.0, default 0.8)
- [ ] DAW automation updates UI in real-time
- [ ] Preset recall updates all 4 knobs
- [ ] Values persist after plugin reload
- [ ] Knobs use relative drag (frame-delta, not absolute positioning)

## 7. WebView-Specific Validation

**CSS constraints:**

- [ ] No viewport units in CSS (100vh, 100vw, 100dvh)
- [ ] html, body { height: 100%; } present
- [ ] user-select: none present (native feel)
- [ ] Context menu disabled in JavaScript

**Resource provider:**

- [ ] All files return 200 (no 404s in console)
- [ ] MIME types correct: text/html, application/javascript
- [ ] check_native_interop.js loads successfully
- [ ] Explicit URL mapping (no generic loops)

**JavaScript:**

- [ ] ES6 module imports working (`type="module"`)
- [ ] `import { getSliderState }` from index.js works
- [ ] All 4 parameter states retrieved successfully
- [ ] No "Failed to get slider state" errors
- [ ] Drag handlers attached to all knobs

## Parameter List (from parameter-spec.md)

| Parameter ID | Type | Range | Default | Relay Type | Attachment Type |
|--------------|------|-------|---------|------------|-----------------|
| volume | Float | 0.0-1.0 | 0.7 | WebSliderRelay | WebSliderParameterAttachment |
| brightness | Float | 0.0-1.0 | 0.5 | WebSliderRelay | WebSliderParameterAttachment |
| space | Float | 0.0-1.0 | 0.3 | WebSliderRelay | WebSliderParameterAttachment |
| randomizeAmount | Float | 0.0-1.0 | 0.8 | WebSliderRelay | WebSliderParameterAttachment |

**Total:** 4 relays, 4 attachments

## Troubleshooting

### WebView blank screen

**Symptoms:** Plugin window opens but shows blank/white screen.

**Check:**

1. Resource provider returns files (add debug logging)
2. MIME types correct (text/html, application/javascript)
3. check_native_interop.js included in binary data
4. HTML has no viewport units (100vh causes blank screen)

**Solution:** Verify all 3 files in `juce_add_binary_data()`, check resource provider URL mapping.

### Knobs frozen (don't respond to drag)

**Symptoms:** Knobs display correctly but don't rotate when dragged.

**Check:**

1. ES6 module imports working (`type="module"` on script tags)
2. `getSliderState()` returns valid state (not null)
3. WebSliderParameterAttachment has 3 parameters (parameter, relay, nullptr)
4. Drag event handlers attached (check console for errors)

**Solution:** Add `type="module"` to script tags, verify JUCE 8 attachment signature.

### Release build crashes

**Symptoms:** Debug build works, release build crashes on reload.

**Check:**

1. Member order in PluginEditor.h (relays → webView → attachments)
2. Attachments declared AFTER webView (destroyed before webView)
3. All unique_ptr members (not raw members)

**Solution:** Fix member declaration order per v2-PluginEditor-TEMPLATE.h.

### Parameters don't persist

**Symptoms:** Knobs reset to default after plugin reload.

**Check:**

1. Attachments created with correct APVTS parameter IDs
2. Parameter IDs in C++ match YAML parameter names exactly
3. `valueChangedEvent.addListener()` called for each parameter
4. Listener reads value via `getNormalisedValue()` (no callback parameters)

**Solution:** Verify parameter ID consistency across YAML, C++, and JavaScript.

## Success Criteria

Stage 3 (GUI) is complete when:

- ✅ Plugin builds without errors (debug + release)
- ✅ WebView loads in standalone app
- ✅ All 4 knobs visible and functional
- ✅ Parameters sync correctly (UI ↔ APVTS)
- ✅ Automation and preset recall work
- ✅ No crashes on reload (release build tested)
- ✅ Validation-agent tests pass (automatic after Stage 3)

## Next Steps

After successful integration:

1. Automatic validation runs (validation-agent)
2. If validation passes: Plugin complete, ready to install
3. If validation fails: Fix issues, rebuild, retest
4. Install to system folders: `/install-plugin PadForge`
5. Test in multiple DAWs (Logic, Ableton, Reaper)
