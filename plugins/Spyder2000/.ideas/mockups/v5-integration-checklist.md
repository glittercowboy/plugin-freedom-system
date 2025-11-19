# Stage 3 (GUI) Integration Checklist - Spyder2000 v5

**Plugin:** Spyder2000 (Guitar Overdrive/Distortion)
**Mockup Version:** v5 (FINALIZED)
**Generated:** 2025-11-18
**Window Size:** 450×300px (non-resizable)
**Parameters:** 3 (gain, tone, level)

## v5 Design Features

This checklist guides integration of the **finalized v5 design**:

- **Flat silkscreen labels** - Authentic pedal aesthetic (NO text-shadow on labels)
- **5px rounded corners** - Hammond 1590BB aluminum enclosure style
- **Davies 1510 knobs** - Black polished cylindrical with white marker
- **Stationary knob shine** - Glossy highlight stays fixed (doesn't rotate)
- **Glossy Ferrari Red chassis** - Highly polished with radial gradient
- **7-segment display** - "INSANE" with authentic LED glow (DSEG7 font)

---

## 1. File Preparation

**Before starting Stage 3, verify these files exist:**

- [ ] `plugins/Spyder2000/.ideas/mockups/v5-ui.html` (production HTML)
- [ ] `plugins/Spyder2000/.ideas/mockups/v5-PluginEditor.h` (C++ header template)
- [ ] `plugins/Spyder2000/.ideas/mockups/v5-PluginEditor.cpp` (C++ implementation template)
- [ ] `plugins/Spyder2000/.ideas/mockups/v5-CMakeLists.txt` (CMake snippet)
- [ ] `plugins/Spyder2000/.ideas/parameter-spec.md` (parameter contract)

**If any files are missing, regenerate them before proceeding.**

---

## 2. Copy UI Files to Source Tree

**Create UI directory structure:**

```bash
cd plugins/Spyder2000
mkdir -p Source/ui/public
```

**Copy production HTML:**

```bash
cp .ideas/mockups/v5-ui.html Source/ui/public/index.html
```

**Verify copy:**

```bash
# File should be ~15-20KB
ls -lh Source/ui/public/index.html

# Verify no placeholders remain
grep -i "{{" Source/ui/public/index.html
# (should return no results)
```

- [ ] Directory `Source/ui/public/` created
- [ ] `v5-ui.html` copied to `Source/ui/public/index.html`
- [ ] No `{{PLACEHOLDERS}}` in index.html
- [ ] File size reasonable (~15-20KB)

---

## 3. Update PluginEditor Files

**Replace placeholder editor with WebView implementation:**

### 3.1 Update PluginEditor.h

**Backup existing file:**

```bash
cp Source/PluginEditor.h Source/PluginEditor.h.backup
```

**Open both files for comparison:**

- Reference: `.ideas/mockups/v5-PluginEditor.h`
- Target: `Source/PluginEditor.h`

**Update the header file:**

1. Add `#include <juce_gui_extra/juce_gui_extra.h>` (WebView support)
2. Add private members:
   - `std::unique_ptr<juce::WebBrowserComponent> webView;`
   - `class UIResourceProvider;`
   - `std::unique_ptr<UIResourceProvider> resourceProvider;`
3. Keep existing class name (`Spyder2000AudioProcessorEditor`)
4. Verify `paint()` and `resized()` declarations present

**Verification:**

- [ ] Backed up original `PluginEditor.h`
- [ ] Added `juce_gui_extra` include
- [ ] Added `webView` member (unique_ptr)
- [ ] Added `UIResourceProvider` forward declaration
- [ ] Added `resourceProvider` member (unique_ptr)
- [ ] Class name unchanged (`Spyder2000AudioProcessorEditor`)

### 3.2 Update PluginEditor.cpp

**Backup existing file:**

```bash
cp Source/PluginEditor.cpp Source/PluginEditor.cpp.backup
```

**Open both files for comparison:**

- Reference: `.ideas/mockups/v5-PluginEditor.cpp`
- Target: `Source/PluginEditor.cpp`

**Update the implementation file:**

1. Add `UIResourceProvider` class definition (serves HTML from binary data)
2. Update constructor:
   - Load HTML content (initially placeholder, will use binary data later)
   - Create `resourceProvider` with HTML content
   - Create `webView` with resource provider options
   - Call `webView->goToURL("/")`
   - Set window size to 450×300
   - Set non-resizable (`setResizable(false, false)`)
3. Update `paint()` to fill black background
4. Update `resized()` to fill WebView with `getLocalBounds()`

**Verification:**

- [ ] Backed up original `PluginEditor.cpp`
- [ ] Added `UIResourceProvider` class
- [ ] Constructor creates `resourceProvider`
- [ ] Constructor creates `webView` with options
- [ ] Constructor navigates to "/"
- [ ] Window size set to 450×300
- [ ] Window set non-resizable
- [ ] `resized()` sets WebView bounds to full window

---

## 4. Update CMakeLists.txt

**Add WebView configuration to build:**

```bash
cd plugins/Spyder2000

# Backup existing CMakeLists.txt
cp CMakeLists.txt CMakeLists.txt.backup

# Open for editing
# Append contents of .ideas/mockups/v5-CMakeLists.txt
```

**Add these sections:**

1. **Binary data target:**
   ```cmake
   juce_add_binary_data(Spyder2000_UIResources
       SOURCES
           Source/ui/public/index.html
   )
   ```

2. **Link libraries:**
   ```cmake
   target_link_libraries(Spyder2000
       PRIVATE
           Spyder2000_UIResources
           juce::juce_gui_extra
   )
   ```

3. **Compile definitions:**
   ```cmake
   target_compile_definitions(Spyder2000
       PUBLIC
           JUCE_WEB_BROWSER=1
           JUCE_USE_CURL=0
   )
   ```

**Verification:**

- [ ] Backed up `CMakeLists.txt`
- [ ] `juce_add_binary_data(Spyder2000_UIResources ...)` added
- [ ] Binary data sources include `Source/ui/public/index.html`
- [ ] `Spyder2000_UIResources` linked to target
- [ ] `juce::juce_gui_extra` linked to target
- [ ] `JUCE_WEB_BROWSER=1` definition added
- [ ] `JUCE_USE_CURL=0` definition added

---

## 5. Build Plugin (Debug Configuration)

**Configure and build:**

```bash
cd plugins/Spyder2000

# Clean previous build
rm -rf build

# Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build --config Debug
```

**Expected output:**

- Binary data target `Spyder2000_UIResources` compiles successfully
- HTML embedded as `BinaryData::index_html` and `BinaryData::index_htmlSize`
- Plugin compiles without errors
- VST3 binary created: `build/Spyder2000_artefacts/Debug/VST3/Spyder2000.vst3`

**Verification:**

- [ ] CMake configure succeeded (no errors)
- [ ] Build completed without errors
- [ ] No warnings about missing `juce_gui_extra` module
- [ ] Binary data compiled (check build logs for `Spyder2000_UIResources`)
- [ ] VST3 binary exists in `build/Spyder2000_artefacts/Debug/VST3/`
- [ ] Binary size reasonable (~1-2MB)

**Common build errors:**

| Error | Solution |
|-------|----------|
| `juce_gui_extra not found` | Add `juce_gui_extra` to `JUCE_MODULES_NEEDED` in CMakeLists.txt |
| `JUCE_WEB_BROWSER undefined` | Add `JUCE_WEB_BROWSER=1` to compile definitions |
| `index.html not found` | Verify file copied to `Source/ui/public/index.html` |
| `WebBrowserComponent not in namespace` | Check `#include <juce_gui_extra/juce_gui_extra.h>` |

---

## 6. Test WebView in Standalone

**Run standalone application:**

```bash
cd plugins/Spyder2000/build

# Run standalone (auto-built by JUCE)
./Spyder2000_artefacts/Debug/Standalone/Spyder2000
```

**Visual verification:**

- [ ] Window opens at 450×300px
- [ ] Window is non-resizable
- [ ] Ferrari Red glossy background visible
- [ ] 5px rounded corners visible
- [ ] Plugin name "SPYDER 2000" displayed at top (flat white, no shadow)
- [ ] 3 black Davies 1510 knobs visible (GAIN, TONE, LEVEL)
- [ ] Knob labels are flat white (NO text-shadow)
- [ ] Knob shine highlights visible (stationary, top-left)
- [ ] 7-segment display shows "INSANE" with red LED glow
- [ ] Display background is pure black
- [ ] No console errors (check Developer Tools if available)

**Interaction testing:**

- [ ] Right-click disabled (context menu doesn't appear)
- [ ] Knobs respond to mouse drag (up = increase, down = decrease)
- [ ] Knob rotation smooth (-135° to +135° range)
- [ ] Knob shine stays stationary (doesn't rotate with knob body)
- [ ] Display updates to show parameter values during adjustment
- [ ] Display shows format: "GAIN 7.5", "TONE 3.2", "LEVEL 9.0"
- [ ] Display returns to "INSANE" after 2 seconds of inactivity

**Developer Tools verification (if available):**

- [ ] Open Developer Tools (right-click → Inspect, if enabled in debug)
- [ ] Console shows no JavaScript errors
- [ ] `window.Juce` object exists (JUCE backend available)
- [ ] `window.Juce.getSliderState("gain")` returns object
- [ ] Network tab shows no failed resource loads

---

## 7. Test Parameter Binding

**Parameter count:** 3 (gain, tone, level)

### 7.1 Verify JUCE Parameter Communication

**In standalone app, with Developer Tools open:**

```javascript
// Test parameter state retrieval
window.Juce.getSliderState("gain")
// Should return: { normalisedValue: 0.5, valueChangedEvent: {...} }

window.Juce.getSliderState("tone")
// Should return: { normalisedValue: 0.5, valueChangedEvent: {...} }

window.Juce.getSliderState("level")
// Should return: { normalisedValue: 0.8, valueChangedEvent: {...} }
```

**Checklist:**

- [ ] `Juce.getSliderState("gain")` returns valid object
- [ ] `Juce.getSliderState("tone")` returns valid object
- [ ] `Juce.getSliderState("level")` returns valid object
- [ ] All state objects have `normalisedValue` property (0.0 to 1.0)
- [ ] All state objects have `valueChangedEvent` object

### 7.2 Test UI → APVTS Communication

**Drag each knob and verify parameter updates:**

1. **Gain knob:**
   - [ ] Drag up to 10.0 (normalized 1.0)
   - [ ] Drag down to 0.0 (normalized 0.0)
   - [ ] Set to 5.0 (normalized 0.5)
   - [ ] Display shows "GAIN X.X" while dragging

2. **Tone knob:**
   - [ ] Drag up to 10.0 (normalized 1.0)
   - [ ] Drag down to 0.0 (normalized 0.0)
   - [ ] Set to 5.0 (normalized 0.5)
   - [ ] Display shows "TONE X.X" while dragging

3. **Level knob:**
   - [ ] Drag up to 10.0 (normalized 1.0)
   - [ ] Drag down to 0.0 (normalized 0.0)
   - [ ] Set to 8.0 (normalized 0.8, default value)
   - [ ] Display shows "LEVEL X.X" while dragging

**Verify parameter persistence:**

- [ ] Close standalone app
- [ ] Reopen standalone app
- [ ] All knobs return to default positions (gain: 5.0, tone: 5.0, level: 8.0)

### 7.3 Test APVTS → UI Communication (Automation)

**If DAW automation is available:**

1. Load plugin in DAW
2. Automate gain parameter (0.0 → 10.0 sweep)
3. Observe knob rotation follows automation
4. Verify display updates during playback

**Checklist:**

- [ ] Automation moves knobs visually
- [ ] Knob rotation synchronized with automation curve
- [ ] Display updates during automation playback
- [ ] No lag or stuttering during automation

---

## 8. Build Plugin (Release Configuration)

**Build release version:**

```bash
cd plugins/Spyder2000

# Build release
cmake --build build --config Release
```

**Release-specific verification:**

- [ ] Release build completes without errors
- [ ] No new warnings in release mode
- [ ] VST3 binary created: `build/Spyder2000_artefacts/Release/VST3/Spyder2000.vst3`
- [ ] Binary size smaller than debug (~500KB-1MB)

**Test release standalone:**

```bash
./build/Spyder2000_artefacts/Release/Standalone/Spyder2000
```

**Release testing:**

- [ ] Standalone opens without crash
- [ ] UI renders identically to debug build
- [ ] All knobs functional
- [ ] No crashes during interaction
- [ ] Reload plugin 10 times (tests stability)
- [ ] No memory leaks (check with Instruments/Valgrind if available)

**⚠️ Critical:** If release build crashes but debug doesn't, this indicates a member order issue. Verify `webView` is destroyed before `resourceProvider` in PluginEditor.h.

---

## 9. WebView-Specific Validation

**CSS validation:**

```bash
# Search for viewport units (FORBIDDEN in JUCE WebView)
grep -n "100vh\|100vw\|100dvh\|100svh" Source/ui/public/index.html
# (should return no results)
```

- [ ] No viewport units in CSS (`100vh`, `100vw`, `100dvh`, `100svh`)
- [ ] `html, body { height: 100%; }` present (REQUIRED)
- [ ] `user-select: none` present (native feel)
- [ ] Context menu disabled in JavaScript

**Resource provider validation:**

- [ ] `UIResourceProvider::getResource("/")` returns HTML
- [ ] MIME type is `text/html; charset=utf-8`
- [ ] HTML content is complete (not truncated)
- [ ] No external CDN dependencies fail to load

**Font loading:**

- [ ] 7-segment font (DSEG7Classic) loads from CDN
- [ ] Display text uses 7-segment font (not fallback)
- [ ] If CDN fails, fallback font chain works ('Digital-7', 'Segment7', monospace)

---

## 10. Final Validation Checklist

**Before marking Stage 3 complete:**

### 10.1 Visual Design Accuracy

- [ ] Window size exactly 450×300px
- [ ] Window non-resizable
- [ ] Ferrari Red glossy background (#DC143C base color)
- [ ] 5px rounded corners (NOT 20px from earlier versions)
- [ ] Hammond 1590BB beveled edges visible
- [ ] Plugin name "SPYDER 2000" in flat white (NO text-shadow)
- [ ] 3 Davies 1510 knobs (black, polished, white marker)
- [ ] Knob labels flat white (NO text-shadow) - **v5 key feature**
- [ ] Knob shine stationary (doesn't rotate) - **v5 key feature**
- [ ] 7-segment display shows "INSANE" with red glow
- [ ] Display background pure black (#000000)

### 10.2 Functional Requirements

- [ ] All 3 parameters (gain, tone, level) functional
- [ ] Knob drag direction correct (up = increase)
- [ ] Knob rotation range correct (-135° to +135°)
- [ ] Display updates show parameter values
- [ ] Display returns to "INSANE" after 2 seconds
- [ ] Parameter values persist across plugin reloads
- [ ] No JavaScript console errors
- [ ] Context menu disabled

### 10.3 Build System

- [ ] Debug build succeeds
- [ ] Release build succeeds
- [ ] HTML embedded via `juce_add_binary_data`
- [ ] `juce_gui_extra` module linked
- [ ] `JUCE_WEB_BROWSER=1` defined
- [ ] No CMake warnings

### 10.4 Stability

- [ ] Standalone runs without crash (debug)
- [ ] Standalone runs without crash (release)
- [ ] Reload test: 10 consecutive reloads without crash
- [ ] No memory leaks detected
- [ ] No resource leaks (check open file descriptors)

---

## 11. v5-Specific Verification

**These checks verify v5 design features are correctly implemented:**

### 11.1 Flat Silkscreen Labels (v5 Key Feature)

**Visual inspection:**

- [ ] Plugin name "SPYDER 2000" has NO text-shadow
- [ ] Knob label "GAIN" has NO text-shadow
- [ ] Knob label "TONE" has NO text-shadow
- [ ] Knob label "LEVEL" has NO text-shadow
- [ ] Labels appear flat white on glossy red background
- [ ] Labels look like authentic pedal silkscreen printing

**CSS verification:**

```bash
# Verify plugin name has no text-shadow
grep -A5 "\.plugin-name" Source/ui/public/index.html | grep "text-shadow"
# (should return no results OR "/* REMOVED v5: text-shadow... */")

# Verify knob labels have no text-shadow
grep -A5 "\.knob-label" Source/ui/public/index.html | grep "text-shadow"
# (should return no results OR "/* REMOVED v5: text-shadow... */")
```

- [ ] `.plugin-name` class has NO `text-shadow` property
- [ ] `.knob-label` class has NO `text-shadow` property
- [ ] Display still has text-shadow (LED glow effect, NOT silkscreen)

### 11.2 Stationary Knob Shine (v5 Key Feature)

**Visual inspection while dragging knobs:**

- [ ] Gain knob: Shine highlight stays top-left while knob rotates
- [ ] Tone knob: Shine highlight stays top-left while knob rotates
- [ ] Level knob: Shine highlight stays top-left while knob rotates
- [ ] Shine layer appears above rotating knob body
- [ ] Shine layer has `pointer-events: none` (doesn't block clicks)

**CSS verification:**

```bash
# Verify .knob-shine has no transform
grep -A10 "\.knob-shine" Source/ui/public/index.html | grep "transform"
# (should only show transform in updateKnobRotation for .knob-body, NOT .knob-shine)
```

- [ ] `.knob-shine` has `position: absolute` and `pointer-events: none`
- [ ] `.knob-shine` has NO `transform` property (stays stationary)
- [ ] `.knob-body` has `transform: rotate(...)` (rotates with knob)
- [ ] JavaScript `updateKnobRotation()` only transforms `.knob-body`

### 11.3 5px Rounded Corners (v5 Refinement)

**Visual inspection:**

- [ ] Container corners are tight and precise (NOT rounded like v1-v3)
- [ ] Corner radius appears to be ~5px (measure with browser tools if available)
- [ ] Beveled edges align with rounded corners

**CSS verification:**

```bash
# Verify border-radius is 5px (NOT 20px)
grep "border-radius" Source/ui/public/index.html | grep -v "50%"
# Should show: border-radius: 5px
```

- [ ] `.container` has `border-radius: 5px`
- [ ] All pseudo-elements (::before, ::after) use matching radius

---

## 12. Known Issues and Limitations

**v5 Design Decisions:**

1. **7-segment font refinement deferred:**
   - Current: DSEG7Classic loaded from CDN
   - Stage 4 polish: Embed font via binary data, optimize character spacing
   - Fallback: 'Digital-7', 'Segment7', monospace

2. **Display text-shadow preserved:**
   - Display retains glowing text-shadow (simulates actual LED glow)
   - This is intentional and different from silkscreen labels
   - Labels have NO shadow (flat silkscreen), display HAS shadow (LED glow)

3. **WebView Developer Tools:**
   - May not be available in release builds
   - Use debug builds for JavaScript debugging
   - Check JUCE documentation for enabling DevTools in release

**Potential Issues:**

- **CDN dependency:** 7-segment font loads from jsdelivr.net (requires internet during first load). Will be embedded in future version.
- **Font fallback:** If CDN unavailable, display uses system monospace font (less authentic but functional)
- **Platform differences:** WebView rendering may vary slightly between macOS/Windows/Linux (test on target platforms)

---

## 13. Next Steps After Stage 3 Complete

**Once all checklist items are verified:**

1. **Run automatic validation:**
   ```bash
   /test Spyder2000
   ```
   - Validation-agent will run compile-time + runtime tests
   - pluginval will verify VST3 compliance
   - All tests must pass before marking Stage 3 complete

2. **Mark Stage 3 complete:**
   - Update `.continue-here.md` with `stage: 3` and `status: complete`
   - Commit all changes with message:
     ```
     feat(Spyder2000): Stage 3 (GUI) complete - WebView v5 integrated
     ```

3. **Optional: Install plugin:**
   ```bash
   /install-plugin Spyder2000
   ```
   - Copies VST3 to system plugin folder
   - Allows testing in real DAWs (Logic Pro, Ableton, etc.)

4. **Optional: Create installer package:**
   ```bash
   /package Spyder2000
   ```
   - Creates PKG installer for distribution
   - Includes VST3, AU (if built), documentation

---

## Parameter List (from parameter-spec.md)

**Total parameters:** 3

| ID | Type | Range | Default | UI Control | Relay Type |
|----|------|-------|---------|------------|------------|
| gain | Float | 0.0-10.0 | 5.0 | Rotary knob | WebSliderRelay |
| tone | Float | 0.0-10.0 | 5.0 | Rotary knob | WebSliderRelay |
| level | Float | 0.0-10.0 | 8.0 | Rotary knob | WebSliderRelay |

**Note:** Parameter IDs are case-sensitive and must match exactly in APVTS and JavaScript.

---

## Summary

This checklist ensures the **finalized v5 design** is correctly integrated:

- ✅ Flat silkscreen labels (authentic pedal aesthetic)
- ✅ 5px rounded corners (Hammond 1590BB style)
- ✅ Davies 1510 knobs with stationary shine
- ✅ Glossy Ferrari Red chassis
- ✅ 7-segment display with LED glow

**Estimated integration time:** 30-60 minutes
**Required expertise:** JUCE 8 WebView, CMake, HTML/CSS/JavaScript
**Dependencies:** juce_gui_extra module, JUCE_WEB_BROWSER=1

**v5 is the FINALIZED design.** All implementation should match v5 specifications exactly.
