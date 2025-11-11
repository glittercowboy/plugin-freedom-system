# ClapMachine - Creative Brief

## Overview

**Type:** Synth (Instrument)
**Core Concept:** The definitive 808-style clap synthesizer with dual-section architecture and independent multimode filtering
**Status:** 💡 Ideated
**Created:** 2025-11-10

## Vision

ClapMachine is designed to be "the only clap you'd ever need for house music." It combines the classic character of 808-style claps with extensive sound-shaping capabilities through a dual-section architecture: an adjustable flam generator with stereo spreading, and a rich noise body with controllable texture and decay. Each section has independent multimode filtering (LP/BP/HP with resonance), allowing precise tonal sculpting of both the initial transient attack and the sustained noise tail. The result is a versatile clap instrument that ranges from tight, punchy house claps to loose, experimental textural hits.

## Architecture

**Dual-Section Design:**

1. **Flam Section:** Multiple layered clap transients with adjustable timing spread (0-100ms), layer count (2-5), and stereo width. This creates the initial attack character.

2. **Body Section:** Noise generator (white/pink/brown) with adjustable tone, decay, and texture. This creates the sustained tail and body of the clap.

Each section has independent multimode filtering for precise tonal control. The sections are blended via a balance control, then processed through a drive stage for saturation and warmth.

## Parameters

### Flam Section (7 controls)

| Parameter | Range/Options | Default | Description |
|-----------|---------------|---------|-------------|
| Timing Spread | 0-100ms | 15ms | Time spread between flam layers (tight to very loose) |
| Layer Count | 2, 3, 4, 5 | 3 | Number of clap layers (stepped knob with detents) |
| Stereo Width | 0-100% | 50% | Stereo spread of layers (mono to wide) |
| Filter Cutoff | 20Hz-20kHz | 8kHz | Flam filter cutoff frequency |
| Filter Resonance | 0-100% | 10% | Flam filter resonance amount |
| Filter Mode | LP/BP/HP | LP | Flam filter type (dropdown/selector) |
| *(Velocity Var)* | *(removed)* | - | *(Not needed - removed per user feedback)* |

### Body Section (7 controls)

| Parameter | Range/Options | Default | Description |
|-----------|---------------|---------|-------------|
| Noise Type | White/Pink/Brown | Pink | Noise generator character (dropdown/selector) |
| Tone/Color | 0-100% | 50% | Noise brightness/darkness (separate from filter) |
| Decay/Release | 10ms-2s | 200ms | Noise tail length |
| Filter Cutoff | 20Hz-20kHz | 5kHz | Body filter cutoff frequency |
| Filter Resonance | 0-100% | 5% | Body filter resonance amount |
| Filter Mode | LP/BP/HP | LP | Body filter type (dropdown/selector) |

### Mix/Output Section (4 controls)

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Flam/Body Balance | 0-100% | 50% | Mix between flam and body sections |
| Drive | 0-100% | 0% | Saturation amount for warmth and grit |
| Pitch | -24 to +24 semitones | 0 | Pitch shift (wide range for creative use) |
| Output Level | -inf to 0dB | -6dB | Master output gain |

**Total Parameter Count:** 18 controls (14 knobs, 2 stepped knobs, 2 selectors)

## MIDI Response

- **Velocity → Volume only:** MIDI velocity controls output level. No velocity affects on tone, filter, or timing.
- **Note range:** Any MIDI note triggers the same clap (pitch parameter shifts it)
- **Polyphony:** Monophonic or simple polyphony (4-8 voices for overlapping hits)

## UI Concept

**Layout:** Dual-panel design with clear visual separation between Flam and Body sections

**Visual Style:** 808-inspired aesthetic with modern clarity. Vintage elements (knob styling, color palette) balanced with contemporary usability.

**Key Elements:**
- Left panel: Flam section controls (timing, layers, width, filter)
- Right panel: Body section controls (noise type, tone, decay, filter)
- Center/bottom: Mix controls (balance, drive, pitch, output)
- Filter mode selectors: Dropdown or three-way switches (LP/BP/HP) per section
- Noise type selector: Dropdown or three-way switch (White/Pink/Brown)
- Layer count: Stepped knob with visible detents at 2, 3, 4, 5

**Color coding:** Visual distinction between Flam (e.g., blue/teal) and Body (e.g., orange/amber) sections for quick navigation

## Use Cases

- **Modern house production:** Tight, punchy claps with subtle flam and controlled body
- **Retro 808 sounds:** Wide flam spread with resonant filtering for classic character
- **Experimental textures:** Extreme timing spreads, pitch shifts, and drive for creative sound design
- **Dynamic layering:** Use pitch and balance controls to create multiple clap variations from one instance
- **Mix-ready versatility:** Independent filtering allows precise fit in dense mixes without external processing

## Inspirations

- **808 drum machines:** Classic flammed clap character with timing spread
- **Modern sample-based claps:** Flexibility and tonal control of contemporary clap libraries
- **Hardware synth claps:** TR-808, TR-909, Simmons electronic percussion
- **Goal:** Combine vintage warmth with modern precision and flexibility

## Technical Notes

### DSP Architecture

1. **Flam Generator:**
   - Generate 2-5 identical clap transients (synthesis or sample-based)
   - Distribute across stereo field based on width parameter
   - Apply timing offsets based on timing spread (0-100ms)
   - Process through multimode filter (LP/BP/HP with resonance)

2. **Body Generator:**
   - Generate noise (white/pink/brown selection)
   - Shape with tone control (pre-filter brightness)
   - Apply envelope (adjustable decay/release)
   - Process through independent multimode filter (LP/BP/HP with resonance)

3. **Mix Stage:**
   - Balance control blends flam and body signals
   - Pitch shift (+/- 2 octaves)
   - Drive/saturation (soft clipping or waveshaping)
   - Output gain with velocity scaling

### Filter Implementation
- Each section needs independent multimode filter (LP/BP/HP)
- Resonance should be musical (self-oscillation optional but not required)
- Filter types should be CPU-efficient (SVF or similar)

### Performance Considerations
- Lightweight DSP (target: <5% CPU on modern systems)
- Fast parameter changes (no zipper noise on filter sweeps)
- Minimal latency (critical for live playing and tight timing)

## Presets

**3-5 Starting Points:**

1. **Tight House:** Minimal flam spread (5ms), 3 layers, moderate body, LP filtering
2. **Classic 808:** Medium flam spread (25ms), 4 layers, pink noise, some resonance
3. **Wide & Loose:** Maximum flam spread (80-100ms), 5 layers, stereo width, long decay
4. **Bright & Punchy:** HP filtering on flam, short decay, white noise, tight timing
5. **Dark & Deep:** LP filtering on both sections, brown noise, resonant body, longer decay

**Default (Init):** Balanced starting point - 3 layers, 15ms spread, 50% width, pink noise, moderate decay, neutral filtering

## Next Steps

- [ ] Create UI mockup (`/dream ClapMachine` → option 3, or invoke `ui-mockup` skill)
- [ ] Define parameter specifications (ranges, curves, defaults) in `parameter-spec.md`
- [ ] Research DSP algorithms (flam timing, noise generation, filter implementation)
- [ ] Start implementation (`/implement ClapMachine`)
