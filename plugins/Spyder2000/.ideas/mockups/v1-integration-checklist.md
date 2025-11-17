# Stage 3 (GUI) Integration Checklist - v1

**Plugin:** Spyder2000
**Mockup Version:** v1
**Generated:** 2025-11-17
**UI Type:** WebView (JUCE WebBrowserComponent)
**Window Size:** 450×300 (non-resizable)

This checklist guides gui-agent through WebView UI integration during Stage 3.

---

## 1. Copy UI Files

**Before starting:** Verify these source files exist in `.ideas/mockups/`:
- `v1-ui.html` (production HTML)
- `v1-PluginEditor.h` (C++ header template)
- `v1-PluginEditor.cpp` (C++ implementation template)
- `v1-CMakeLists.txt` (WebView configuration)

**Tasks:**

- [ ] Create directory: `Source/ui/public/`
- [ ] Create directory: `Source/ui/public/js/juce/`
- [ ] Copy `v1-ui.html` → `Source/ui/public/index.html`
- [ ] Copy JUCE frontend library → `Source/ui/public/js/juce/index.js`
  - Source: Working plugin (e.g., GainKnob) or JUCE examples
- [ ] Copy JUCE interop check → `Source/ui/public/js/juce/check_native_interop.js`
  - Source: Working plugin (e.g., GainKnob) or JUCE examples
  - CRITICAL: Required for native integration (pattern #13)

**Verification:**
```bash
ls -la Source/ui/public/
# Expected: index.html
ls -la Source/ui/public/js/juce/
# Expected: index.js, check_native_interop.js
```

---

## 2. Update PluginEditor Files

**Replace stub PluginEditor with WebView implementation**

- [ ] Backup existing files (optional):
  ```bash
  cp Source/PluginEditor.h Source/PluginEditor.h.backup
  cp Source/PluginEditor.cpp Source/PluginEditor.cpp.backup
  ```

- [ ] Replace `Source/PluginEditor.h` with `v1-PluginEditor.h` content
  - Update class name if needed: `Spyder2000AudioProcessorEditor`
  - Verify member order: relays → webView → attachments (CRITICAL - pattern #11)

- [ ] Replace `Source/PluginEditor.cpp` with `v1-PluginEditor.cpp` content
  - Update includes to match actual processor filename
  - Verify attachment constructor has nullptr (3rd parameter - JUCE 8 requirement, pattern #12)

**Verification:**
```bash
# Check member order in header
grep -A 20 "CRITICAL MEMBER" Source/PluginEditor.h
# Should show: relays, then webView, then attachments

# Check JUCE 8 attachment constructor (3 parameters)
grep "WebSliderParameterAttachment" Source/PluginEditor.cpp
# Should show: (parameter, relay, nullptr)
```

---

## 3. Update CMakeLists.txt

**Append WebView configuration to existing CMakeLists.txt**

- [ ] Open `CMakeLists.txt`
- [ ] Verify `juce_add_plugin` has `NEEDS_WEB_BROWSER TRUE` (REQUIRED - pattern #9)
  - If missing, add it to `juce_add_plugin()` configuration
- [ ] Append entire `v1-CMakeLists.txt` content to end of file
- [ ] Verify `juce_add_binary_data` includes all 3 UI files:
  - index.html
  - index.js
  - check_native_interop.js
- [ ] Verify `juce::juce_gui_extra` is linked (REQUIRED - pattern #3)
- [ ] Verify `JUCE_WEB_BROWSER=1` is defined (REQUIRED)

**Verification:**
```bash
# Check NEEDS_WEB_BROWSER flag
grep "NEEDS_WEB_BROWSER" CMakeLists.txt
# Expected: NEEDS_WEB_BROWSER TRUE

# Check binary data configuration
grep -A 5 "juce_add_binary_data" CMakeLists.txt
# Expected: All 3 files listed

# Check module linkage
grep "juce_gui_extra" CMakeLists.txt
# Expected: juce::juce_gui_extra
```

---

## 4. Build and Test (Debug)

**Compile and test in debug mode**

- [ ] Clean build artifacts:
  ```bash
  rm -rf build/
  ```

- [ ] Configure build:
  ```bash
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
  ```

- [ ] Build plugin:
  ```bash
  cmake --build build --config Debug
  ```

- [ ] Verify build succeeded (no errors)
  - Check for WebView-specific warnings (none expected)
  - Verify PluginEditor.cpp compiled successfully

- [ ] Install to system folders (CRITICAL - pattern #10):
  ```bash
  ./scripts/build-and-install.sh Spyder2000
  # NOT: --no-install (tests stale builds)
  ```

- [ ] Clear DAW caches:
  ```bash
  killall -9 AudioComponentRegistrar
  rm ~/Library/Preferences/Ableton/*/PluginDatabase.cfg
  ```

- [ ] Launch standalone application:
  ```bash
  ./build/Spyder2000_artefacts/Debug/Standalone/Spyder2000.app/Contents/MacOS/Spyder2000
  ```

- [ ] Verify WebView loads (not blank white screen)
- [ ] Right-click → "Inspect" should work (opens developer tools)
- [ ] Check console for JavaScript errors (none expected)
- [ ] Verify `window.__JUCE__` object exists in console
- [ ] Test knobs respond to mouse drag (up = increase, down = decrease)

**Verification:**
```javascript
// In browser console (right-click → Inspect)
window.__JUCE__  // Should be object, not undefined
```

---

## 5. Build and Test (Release)

**Compile release build and test member order correctness**

- [ ] Build release:
  ```bash
  cmake --build build --config Release
  ```

- [ ] Install to system folders:
  ```bash
  ./scripts/build-and-install.sh Spyder2000
  ```

- [ ] Test in standalone (reload 10 times):
  ```bash
  for i in {1..10}; do
    ./build/Spyder2000_artefacts/Release/Standalone/Spyder2000.app/Contents/MacOS/Spyder2000 &
    sleep 2
    killall Spyder2000
  done
  ```

- [ ] Verify no crashes on repeated reload
  - Crashes indicate member order violation (attachments before webView)
  - If crashes occur, verify member order in PluginEditor.h

- [ ] Load in DAW (Ableton, Logic, etc.)
- [ ] Verify plugin shows correct UI (Ferrari Red, POD aesthetic)
- [ ] Verify no blank screen or missing resources

---

## 6. Test Parameter Binding

**Verify bidirectional parameter synchronization**

- [ ] **UI → APVTS:** Drag knobs in UI, verify parameters change
  - Gain: 0.0 to 10.0 (default 5.0)
  - Tone: 0.0 to 10.0 (default 5.0)
  - Level: 0.0 to 10.0 (default 8.0)

- [ ] **APVTS → UI:** DAW automation updates UI
  - Create automation lane for gain parameter
  - Automate value changes
  - Verify UI knob rotates to match automation

- [ ] **Preset recall:** Save/load preset updates UI
  - Adjust all knobs to non-default values
  - Save preset
  - Reset to defaults
  - Load saved preset
  - Verify UI updates to saved values

- [ ] **Reload persistence:** Values persist after plugin reload
  - Set knobs to specific values
  - Close and reopen plugin UI
  - Verify knobs show same values (not reset to defaults)

**Verification:**
- All 3 parameters sync UI ↔ APVTS bidirectionally
- No frozen knobs (would indicate missing nullptr in attachment - pattern #12)
- No jumpy knobs (would indicate wrong event callback pattern - pattern #15)

---

## 7. WebView-Specific Validation

**Verify WebView best practices compliance**

- [ ] **No viewport units in CSS:**
  ```bash
  grep -E "(100vh|100vw|100dvh|100svh)" Source/ui/public/index.html
  # Expected: No matches (would violate pattern)
  ```

- [ ] **Native feel CSS present:**
  ```bash
  grep "user-select: none" Source/ui/public/index.html
  # Expected: Match found (REQUIRED)
  ```

- [ ] **Context menu disabled:**
  ```bash
  grep "contextmenu" Source/ui/public/index.html
  # Expected: preventDefault() found
  ```

- [ ] **Resource provider returns all files:**
  - Open browser console in standalone
  - Check Network tab (if available)
  - Verify no 404 errors for resources

- [ ] **Correct MIME types:**
  ```bash
  grep "text/html" Source/PluginEditor.cpp
  grep "text/javascript" Source/PluginEditor.cpp
  # Expected: Matches found (CRITICAL - wrong MIME breaks loading)
  ```

- [ ] **ES6 module imports work (pattern #21):**
  ```bash
  grep 'type="module"' Source/ui/public/index.html
  # Expected: Both script tags have type="module"

  grep "import { getSliderState }" Source/ui/public/index.html
  # Expected: Match found (REQUIRED)
  ```

---

## 8. Final Validation

**Run complete validation suite**

- [ ] Run pluginval (automatic during workflow):
  ```bash
  ./scripts/build-and-install.sh Spyder2000 --test
  ```

- [ ] Verify all tests pass:
  - VST3 validation
  - AU validation (macOS only)
  - No crashes or memory leaks
  - Parameter automation works

- [ ] Visual inspection:
  - Ferrari Red glossy background renders correctly
  - 3 knobs in horizontal layout (Gain, Tone, Level)
  - POD-style rubber knob texture visible
  - 7-segment display shows "INSANE" when idle
  - Display updates to show parameter values during adjustment
  - Display returns to "INSANE" after 2 seconds of inactivity

---

## Parameter Summary (from parameter-spec.md)

Total: 3 parameters (all Float, all WebSliderRelay)

| Parameter | ID    | Type  | Range     | Default | Relay Type          | Attachment Type                      |
|-----------|-------|-------|-----------|---------|---------------------|--------------------------------------|
| Gain      | gain  | Float | 0.0-10.0  | 5.0     | WebSliderRelay      | WebSliderParameterAttachment         |
| Tone      | tone  | Float | 0.0-10.0  | 5.0     | WebSliderRelay      | WebSliderParameterAttachment         |
| Level     | level | Float | 0.0-10.0  | 8.0     | WebSliderRelay      | WebSliderParameterAttachment         |

---

## Common Issues and Solutions

**Issue:** Knobs display but don't respond to drag
- **Cause:** Missing ES6 module imports (pattern #21)
- **Solution:** Verify `type="module"` on both script tags and `import { getSliderState }` present

**Issue:** Blank white screen (no UI)
- **Cause:** Resource provider not serving files correctly
- **Solution:** Check explicit URL mapping in getResource() method (pattern #8)

**Issue:** Crashes on plugin reload (release build only)
- **Cause:** Member order violation (attachments before webView)
- **Solution:** Verify relays → webView → attachments order in .h file (pattern #11)

**Issue:** Knobs freeze after initial load
- **Cause:** Missing nullptr in attachment constructor (JUCE 8)
- **Solution:** Verify 3 parameters in WebSliderParameterAttachment (pattern #12)

**Issue:** Plugin doesn't appear in DAW
- **Cause:** Missing NEEDS_WEB_BROWSER flag in CMakeLists.txt
- **Solution:** Add NEEDS_WEB_BROWSER TRUE to juce_add_plugin() (pattern #9)

**Issue:** "JuceHeader.h not found" compile error
- **Cause:** Missing juce_generate_juce_header() call
- **Solution:** Add juce_generate_juce_header(Spyder2000) after target_link_libraries() (pattern #1)

---

## Completion Criteria

Stage 3 (GUI) is complete when:

- [ ] All checkboxes in sections 1-8 are checked
- [ ] Plugin builds without warnings (debug + release)
- [ ] WebView UI displays correctly in standalone and DAW
- [ ] All 3 parameters sync bidirectionally (UI ↔ APVTS)
- [ ] No crashes on repeated reload (release build)
- [ ] pluginval tests pass with 0 failures
- [ ] DAW automation, preset recall, and persistence all work

After completion, proceed to:
- Install to system folders: `/install-plugin Spyder2000`
- Package for distribution: `/package Spyder2000` (optional)
- Test in real-world scenarios (guitar processing, mix integration)

---

**Reference:** See `troubleshooting/patterns/juce8-critical-patterns.md` for detailed explanations of all patterns referenced in this checklist.
