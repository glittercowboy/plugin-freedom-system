# Parameter Specification

**Plugin:** AutoClip
**Version:** 1.0
**Created:** 2025-11-13
**Source:** UI Mockup v5 (finalized)

This specification defines all parameters for JUCE implementation (APVTS). This is the immutable contract for implementation stages.

---

## Total Parameter Count

**Total:** 2 parameters

---

## Parameter Definitions

### clipThreshold

- **Parameter ID:** `clipThreshold`
- **Type:** Float
- **Range:** 0.0 to 100.0
- **Default:** 0.0
- **Units:** %
- **Skew Factor:** 1.0 (linear)
- **UI Control:** Rotary knob (190×190px)
- **UI Position:** Center (x:55, y:155)
- **UI Behavior:** Physical rotation model - texture rotates with knob, lighting fixed
- **Value Display:** None (no text readout below knob)
- **Label:** None
- **DSP Usage:** Controls the clipping threshold level (0% = no clipping, 100% = maximum clipping)

**JUCE Implementation:**
```cpp
auto clipThreshold = std::make_unique<juce::AudioParameterFloat>(
    juce::ParameterID("clipThreshold", 1),
    "Clip Threshold",
    juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f),
    0.0f,
    "%"
);
```

---

### soloClipped

- **Parameter ID:** `soloClipped`
- **Type:** Bool
- **Default:** false (Off)
- **UI Control:** Toggle switch (80×38px)
- **UI Position:** Lower center (x:110, y:420)
- **UI Behavior:** Horizontal slide toggle (Off = left, On = right)
- **Label:** "CLIP SOLO" (below toggle)
- **Label Style:** 11px uppercase, letter-spacing 0.25em
- **DSP Usage:** When true, outputs only the clipped portion of signal (delta monitoring)

**JUCE Implementation:**
```cpp
auto soloClipped = std::make_unique<juce::AudioParameterBool>(
    juce::ParameterID("soloClipped", 1),
    "Clip Solo",
    false
);
```

---

## UI Layout Summary

**Window:** 300×500px (portrait, fixed size)

**Visual Theme:** Vintage Bakelite
- Background: Radial gradient (#2a1808 → #1a0800)
- Border: 2px solid #4a3020 with stitched inner border
- Title: "AUTOCLIP" (20px bold, top: 40px)

**Controls Layout:**
1. **Large rotary knob** (dominant element, y-centered)
   - 190px diameter
   - Vintage Bakelite aesthetic with noise texture
   - Physical rotation behavior
   - Hover effect: border color changes to #c49564

2. **Toggle switch** (below knob)
   - 80×38px horizontal pill
   - Slide thumb animation (0.2s ease)
   - Label below switch

**Interaction Model:**
- Knob: Vertical drag to adjust (ns-resize cursor)
- Rotation range: ±135° (270° total travel)
- Toggle: Click to toggle state
- Hover feedback: Subtle border illumination (TapeAge-style)

---

## Parameter Groups

*No parameter groups - both parameters are global/ungrouped*

---

## WebView Integration Notes

**UI Files:**
- Production HTML: `Source/ui/public/index.html`
- JUCE frontend library: `Source/ui/public/js/juce/index.js`

**Parameter Bindings:**
- `clipThreshold`: Slider relay + attachment (rotary knob behavior)
- `soloClipped`: Toggle relay + attachment (boolean state)

**Critical Implementation Details:**
- Member order in PluginEditor.h: relays → webView → attachments
- Knob rotation: Apply transform to entire body element (not just indicator)
- Shadow container: Fixed (doesn't rotate with knob)
- No viewport units (100vh/100vw) - use `html, body { height: 100%; }`
- Context menu disabled via JavaScript
- User-select: none (native feel)

---

## Validation Checklist

Before implementation:
- [ ] 2 parameters defined (clipThreshold, soloClipped)
- [ ] Parameter IDs match YAML spec exactly
- [ ] Default values match YAML defaults
- [ ] UI control types match (rotary_knob → slider relay, toggle → toggle relay)
- [ ] Window dimensions: 300×500px
- [ ] WebView constraints validated (no viewport units, proper height)

---

## Change Log

- **2025-11-13:** Initial specification from mockup v5
