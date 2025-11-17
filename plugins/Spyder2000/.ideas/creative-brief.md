# Spyder 2000 - Creative Brief

## Overview

**Type:** Effect (Guitar Overdrive/Distortion)
**Core Concept:** Line 6 patent-based nonlinear distortion with adaptive pre-clipping filtering and tilt-based tone control
**Status:** 💡 Ideated
**Created:** 2025-11-17

## Vision

Spyder 2000 is a sophisticated guitar overdrive/distortion plugin that pays tribute to the legendary Line 6 POD and Spider amp series, both in its sonic character and visual design. At its core is the famous Line 6 patent nonlinear function, providing that coveted distortion character that defined countless modern guitar tones.

What makes Spyder 2000 unique is its adaptive pre-clipping filter system. Using parallel shelving and peak filters with gain-dependent crossfading, the plugin emphasizes different frequency regions based on the Gain setting: low gain settings boost treble shelf frequencies for clarity and sparkle (perfect for pushing an already dirty amp), while high gain settings shift to midrange emphasis for focused, aggressive distortion (ideal for classic rock and blues into clean amplifiers).

The signal chain is meticulously designed: input → adaptive pre-clipping IIR filter → oversampled Line 6 nonlinear section → tilt-based tone control → output level. Oversampling is user-selectable to balance quality and CPU usage, ensuring pristine sound without aliasing artifacts.

The Tone control uses a tilt filter centered at 680Hz with ±12dB range, allowing players to dial in everything from bright, cutting leads to warm, smooth rhythm tones. The Level control provides clean output gain ranging from mute to unity, acting as makeup gain after the clipping stage.

Visually, Spyder 2000 channels the iconic POD aesthetic with a glossy bright red finish, reimagined in a compact 1590b stompbox enclosure. Lots of shine and gloss create an eye-catching interface that looks as good as it sounds.

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Gain | 0-10 | 5.0 | Drive amount (maps to 3-40dB internally). Low settings = boost mode, high settings = distortion mode |
| Tone | 0-10 | 5.0 | Tilt filter @ 680Hz (0 = darker/-12dB highs, 10 = brighter/+12dB highs) |
| Level | 0-10 | 8.0 | Output level (maps to -∞ to 0dB, unity at 10) |

## Signal Flow

```
Input
  ↓
Pre-Clipping Adaptive IIR Filter (parallel topology)
  ├─ Treble Shelf Filter (emphasized at low gain)
  └─ Midrange Peak Filter (emphasized at high gain)
  ↓
Line 6 Patent Nonlinear Function (oversampled, user-selectable quality)
  ↓
Tilt Filter @ 680Hz (±12dB range)
  ↓
Output Level Control (-∞ to 0dB)
  ↓
Output
```

## UI Concept

**Layout:** Three-knob horizontal layout (Gain → Tone → Level)
**Visual Style:** Glossy bright red finish inspired by the iconic Line 6 POD, reimagined in a compact 1590b stompbox enclosure
**Key Elements:**
- High-gloss red surface with lots of shine
- Clean, professional knob design
- POD-inspired color palette and aesthetic
- Compact form factor (stompbox proportions)

## Use Cases

- **Modern high-gain guitarists:** Use low gain settings (1-4) to boost an already overdriven amplifier, adding clarity and edge without excessive distortion
- **Classic rock & blues guitarists:** Use higher gain settings (6-10) as primary distortion into clean amplifiers, achieving vintage saturated tones
- **Versatile gain staging:** Works as both a boost pedal and standalone distortion depending on gain setting
- **Studio production:** Creative distortion on guitars, bass, synths, or other sources requiring controlled harmonic saturation

## Inspirations

- **Line 6 POD:** Iconic glossy red aesthetic and nonlinear distortion algorithm (patent-based implementation)
- **Line 6 Spider amps:** Name tribute and sonic lineage
- **Modern high-gain pedals:** Adaptive filtering approach for gain-dependent frequency shaping
- **Classic stompbox design:** 1590b enclosure form factor for familiar, intuitive layout

## Technical Notes

**Nonlinear Section:**
- Implementation of famous Line 6 patent nonlinear function
- Oversampling: user-selectable quality mode (2x, 4x, 8x, or adaptive)
- Clips at ±1 for consistent headroom management

**Pre-Clipping Filter:**
- Parallel topology: treble shelf + midrange peak
- Gain-dependent crossfading: low gain emphasizes shelf, high gain emphasizes peak
- Allows independent tuning of each filter stage
- More intuitive behavior than coefficient-morphing approach

**Tone Control:**
- Tilt filter centered @ 680Hz
- ±12dB range (boost highs/cut lows ↔ cut highs/boost lows)
- May require tuning during Stage 0 DSP research

**Additional Considerations:**
- Anti-aliasing approach to be determined in Stage 0
- DC blocking filter recommended
- Bypass behavior (true bypass vs buffered) TBD

## Next Steps

- [ ] Create UI mockup (`/dream Spyder2000` → option 3)
- [ ] Start Stage 0 DSP research and planning (`/plan`)
- [ ] Start implementation (`/implement Spyder2000`)
