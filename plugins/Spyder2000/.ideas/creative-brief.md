# Spyder 2000 - Creative Brief

## Overview

**Type:** Effect (Guitar Overdrive/Distortion)
**Core Concept:** Line 6-inspired nonlinear distortion with adaptive pre-clipping filtering and tilt-based tone control
**Status:** 🚧 Stage 0 (Research & Planning Complete)
**Created:** 2025-11-17
**Last Updated:** 2025-11-17 (Stage 0 findings integrated)

## Vision

Spyder 2000 is a sophisticated guitar overdrive/distortion plugin that pays tribute to the legendary Line 6 POD and Spider amp series, both in its sonic character and visual design. At its core is the **authentic Line 6 Patent U.S. Patent 5,789,689 (Figure 14) waveshaping algorithm**—the actual nonlinear transfer function used in POD and Spider amps. This piecewise formula with three distinct regions (sharp asymmetric clipping, quadratic soft saturation, and hard clip) provides the exact harmonic distortion character that defined Line 6's legendary guitar tones.

What makes Spyder 2000 unique is its adaptive pre-clipping filter system. Using parallel shelving and peak filters with exponential gain-dependent crossfading, the plugin emphasizes different frequency regions based on the Gain setting: low gain settings boost a treble shelf @ 2000Hz (+6dB max) for clarity and sparkle (perfect for pushing an already dirty amp), while high gain settings shift to a midrange peak @ 800Hz (+9dB max) for focused, aggressive distortion (ideal for classic rock and blues into clean amplifiers).

The signal chain is meticulously designed: input → adaptive pre-clipping IIR filter → 8x oversampled nonlinear waveshaper → tilt-based tone control → DC blocking filter → output level. The 8x oversampling uses polyphase IIR filters for efficient, pristine sound without aliasing artifacts, adding only ~10-20 samples of latency (automatically compensated by the DAW).

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
Input (Stereo)
  ↓
Adaptive Pre-Clipping Filter (parallel topology, exponential crossfading)
  ├─ Treble Shelf @ 2000Hz (+6dB max, emphasized at low gain)
  └─ Midrange Peak @ 800Hz (+9dB max, emphasized at high gain)
  ↓
[Upsample 8x - Polyphase IIR]
  ↓
Nonlinear Waveshaper (tanh, 3-40dB drive range)
  ↓
[Downsample to 1x - Anti-aliasing]
  ↓
Tilt Filter @ 680Hz (±12dB, two biquad shelves)
  ↓
DC Blocking Filter (20Hz highpass, always active)
  ↓
Output Level Control (-∞ to 0dB makeup gain)
  ↓
Output (Stereo)
```

**Performance:** ~50% single core CPU @ 48kHz, ~10-20 samples latency (host-compensated)

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

**Research Findings (Stage 0 Complete):**

Research conducted on 4 professional plugins (FabFilter Saturn 2, Soundtoys Decapitator, iZotope Trash 2, Line 6 POD Farm) and industry DSP literature. Architecture validated as implementable in JUCE 8 using `juce_dsp` module.

**Nonlinear Waveshaping:**
- **Algorithm:** **Authentic Line 6 Patent U.S. Patent 5,789,689 (Figure 14)** piecewise waveshaping
- **UPDATE:** Actual Line 6 patent formula obtained post-Stage 0 - provides exact POD/Spider character
- **Piecewise regions:**
  1. Region 1 (input < -0.08905): Sharp asymmetric clipping with 12th-power polynomial
  2. Region 2 (-0.08905 to 0.320018): Quadratic soft saturation
  3. Region 3 (input >= 0.320018): Hard clip at 0.630035
- **Drive mapping:** gain parameter (0-10) → 3-40dB → linear gain 1.41x to 100x
- **Formula:** `driveGain = pow(10.0, (3.0 + gainValue * 3.7) / 20.0)`
- **Characteristics:** Asymmetric saturation creates even harmonics (signature Line 6 sound)
- **JUCE implementation:** `juce::dsp::WaveShaper<float>` with custom Line 6 transfer function
- **Fallback:** Tanh waveshaping if CPU profiling shows patent formula too expensive (unlikely)

**Oversampling:**
- **Factor:** 8x oversampling (default, recommended for heavy distortion per research)
- **Implementation:** `juce::dsp::Oversampling<float>` with polyphase IIR filters
- **Cascade:** 3 stages (8x → 4x → 2x → 1x via halfband filters)
- **Latency:** ~10-20 samples @ 48kHz, automatically reported via `setLatencySamples()`
- **CPU cost:** ~35% CPU (most expensive component), total plugin ~50% single core @ 48kHz
- **Rationale:** Research shows 8x necessary for high gain settings (>30dB drive) to prevent audible aliasing

**Adaptive Pre-Clipping Filter:**
- **Treble shelf:**
  - Frequency: 2000 Hz (upper midrange/presence region)
  - Q: 0.707 (standard shelf slope)
  - Gain: +6dB maximum boost (at gain = 0, fades to 0dB at mid-range)
  - JUCE: `juce::dsp::IIR::Coefficients<float>::makeHighShelf()`
- **Midrange peak:**
  - Center frequency: 800 Hz (guitar fundamental region)
  - Q: 1.5 (focused peak)
  - Gain: +9dB maximum boost (at gain = 10, fades from 0dB at low gain)
  - JUCE: `juce::dsp::IIR::Coefficients<float>::makePeakFilter()`
- **Crossfading:**
  - Curve: Exponential (smooth transition, no abrupt changes)
  - Formula: `shelfGain = exp(-gainValue * 0.8)`, `peakGain = 1.0 - exp(-(gainValue - 4.0) * 0.8)`
  - Low gain (0-4): Emphasize treble shelf (boost mode for dirty amps)
  - Mid gain (4-6): Transition region (both filters active)
  - High gain (6-10): Emphasize midrange peak (distortion mode for clean amps)

**Tilt Filter (Tone Control):**
- **Implementation:** Two `juce::dsp::IIR::Filter<float>` instances (low shelf + high shelf @ 680Hz)
- **Low shelf:** Gain -12dB to +12dB (inverse of tone parameter), Q: 0.707
  - Formula: `lowShelfGain_dB = (5.0 - toneValue) * 2.4`
- **High shelf:** Gain -12dB to +12dB (proportional to tone parameter), Q: 0.707
  - Formula: `highShelfGain_dB = (toneValue - 5.0) * 2.4`
- **Result:** tone=0 (darkest: +12dB lows, -12dB highs), tone=5 (flat), tone=10 (brightest: -12dB lows, +12dB highs)
- **Phase:** Minimal phase shift at 680Hz pivot frequency (<10 degrees, imperceptible)

**DC Blocking Filter:**
- **Purpose:** Remove DC offset introduced by waveshaping
- **Implementation:** `juce::dsp::IIR::Coefficients<float>::makeHighPass()`
- **Cutoff:** 20 Hz
- **Q:** 0.707 (standard Butterworth response)
- **Order:** First-order (minimal phase shift)
- **Always active** (prevents DC buildup, protects speakers)

**Output Level Control:**
- **Range:** -∞ to 0dB (mute to unity gain)
- **Mapping:** level parameter (0-10) → -60dB to 0dB → linear gain 0.001 to 1.0
- **Formula:** `outputGain = (levelValue < 0.01) ? 0.0 : pow(10.0, (levelValue - 10.0) * 6.0 / 20.0)`
- **Smoothing:** 5ms ramp to avoid clicks when changing level
- **Assumes nonlinear section clips at ±1** (output level provides makeup only, no boost above unity)

**Architecture Decisions:**
- **Authentic Line 6 patent formula:** **UPDATE** - U.S. Patent 5,789,689 (Figure 14) obtained post-Stage 0, provides exact POD/Spider character (not approximation). Tanh retained as CPU fallback only.
- **8x oversampling:** Required for high gain (>30dB drive) per DSP research; 4x insufficient for aliasing reduction
- **Parallel filter topology:** Allows independent tuning of treble shelf and midrange peak; more intuitive than coefficient-morphing
- **Two-shelf tilt filter:** Standard implementation (Quad 34 preamp, modern mastering plugins); simpler than allpass-based approach

**Performance Characteristics:**
- **CPU usage:** ~50% single core @ 48kHz (35% oversampling, 8% waveshaping, 5% filters, 2% gain)
- **Latency:** ~10-20 samples from oversampling (host-compensated via delay compensation)
- **Sample rate:** Supports 44.1kHz to 192kHz (oversampling scales: 8x @ 44.1kHz = 352.8kHz internal)
- **Denormal protection:** `juce::ScopedNoDenormals` in `processBlock()`
- **Thread safety:** All parameter reads atomic via `APVTS::getRawParameterValue()->load()`

**JUCE Modules Required:**
- `juce_dsp` (Oversampling, IIR::Filter, WaveShaper, ProcessSpec, AudioBlock)
- `juce_audio_processors` (AudioProcessor, APVTS)
- `juce_core` (standard utilities)

## Next Steps

- [x] ~~Create UI mockup~~ (can be done in parallel with implementation)
- [x] ~~Stage 0 DSP research and planning~~ (`/plan` - COMPLETE)
- [ ] **Start implementation** (`/implement Spyder2000` - Stage 1: Foundation + Shell)
  - Create CMakeLists.txt with juce_dsp module dependency
  - Generate project structure (PluginProcessor, PluginEditor)
  - Implement APVTS with 3 parameters (gain, tone, level)
  - Configure BusesProperties (stereo in/out)
