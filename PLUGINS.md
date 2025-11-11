# PLUGIN REGISTRY

## State Legend

- **💡 Ideated** - Creative brief exists, no implementation
- **🚧 Stage N** - In development (specific stage number)
- **✅ Working** - Completed Stage 6, not installed
- **📦 Installed** - Deployed to system folders
- **🐛 Has Issues** - Known problems (combines with other states)
- **🗑️ Archived** - Deprecated

## State Machine Rules

- If status is 🚧: ONLY plugin-workflow can modify (use `/continue` to resume)
- plugin-improve blocks if status is 🚧 (must complete workflow first)

## Build Management

- All plugin builds managed by `build-automation` skill
- Build logs: `logs/[PluginName]/build_TIMESTAMP.log`
- Installed plugins: `~/Library/Audio/Plug-Ins/VST3/` and `~/Library/Audio/Plug-Ins/Components/`

## Plugin Registry

| Plugin Name | Status | Version | Last Updated |
|-------------|--------|---------|--------------|
| GainKnob | 📦 Installed | 1.2.3 | 2025-11-10 |
| TapeAge | 💡 Ideated | - | 2025-11-10 |
| ClapMachine | 💡 Ideated | - | 2025-11-10 |

### GainKnob

**Status:** 📦 **Installed**
**Version:** 1.2.3
**Created:** 2025-11-10
**Completed:** 2025-11-10
**Installed:** 2025-11-10
**Type:** Audio Effect (Utility)

**Description:**
Minimalist gain, pan, and DJ-style filter utility plugin with three knobs for volume attenuation, stereo positioning, and frequency filtering.

**Parameters (3 total):**
- Gain: -∞ to 0dB, default 0dB (volume attenuation)
- Pan: -100% L to +100% R, default 0% (stereo positioning)
- Filter: -100% to +100%, default 0% (DJ-style filter: negative=low-pass, positive=high-pass, 0%=bypass)

**DSP:** 2nd-order Butterworth IIR filters (200Hz-20kHz), gain multiplication, constant power panning. DSP chain: Filter → Gain → Pan.

**GUI:** Three horizontal rotary knobs with value displays. Clean, minimal design. 800x400px.

**Validation:**
- ✓ Factory presets: 5 presets created (Unity, Subtle Cut, Half Volume, Quiet, Silence)
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Parameter spec adherence: GAIN parameter matches spec exactly (-60 to 0 dB)
- ✓ DSP implementation: Decibels::decibelsToGain with silence at minimum
- ✓ WebView UI: Knob binding operational

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/GainKnob.vst3` (4.1 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/GainKnob.component` (4.0 MB)

**Lifecycle Timeline:**
- **2025-11-10:** Creative brief completed
- **2025-11-10 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-10 (Stage 1):** Planning - Complexity 1.2 (single-pass implementation)
- **2025-11-10 (Stage 2):** Foundation - Build system operational, compiles successfully
- **2025-11-10 (Stage 3):** Shell complete - 1 parameter implemented
- **2025-11-10 (Stage 4):** DSP complete - Gain conversion with denormal protection
- **2025-11-10 (Stage 5):** GUI complete - WebView integration with v1 mockup
- **2025-11-10 (Stage 6):** Validation complete - ready for installation
- **2025-11-10 (v1.0.0):** Installed to system folders (VST3 + AU)
- **2025-11-10 (v1.1.0):** Pan parameter added - constant power panning, dual knob UI
- **2025-11-10 (v1.2.0):** DJ-style filter added - Butterworth IIR filters, three knob UI
- **2025-11-10 (v1.2.1):** Filter frequency curve fixed - exponential mapping for musical sweep
- **2025-11-10 (v1.2.2):** Filter direction inverted - center position now bypasses correctly
- **2025-11-10 (v1.2.3):** Filter state burst eliminated - reset delay buffers on type transitions

**Known Issues:**
- None

**Last Updated:** 2025-11-10

### TapeAge

**Status:** 🚧 **Stage 4**
**Created:** 2025-11-10
**Type:** Audio Effect

**Description:**
Vintage tape saturator with warm saturation and musical degradation (wow/flutter/dropout/noise). 60s/70s aesthetic with earth tone palette.

**Parameters (3 total):**
- DRIVE: 0-100%, default 50% (tape saturation)
- AGE: 0-100%, default 25% (tape degradation - wow/flutter/dropout/noise)
- MIX: 0-100%, default 100% (dry/wet blend)

**DSP:** Warm harmonic saturation with controllable tape artifacts (wow, flutter, dropout, noise). Musical degradation even at maximum settings. 2x oversampling for aliasing reduction.

**GUI:** Medium rectangle, vintage VU meter (output peak), 3 brass knobs horizontal, creamy beige textured background, burnt orange/brown accents, clean sans-serif all-caps typography.

**Lifecycle Timeline:**
- **2025-11-10:** Creative brief completed
- **2025-11-10 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-10 (Stage 1):** Planning - Complexity 5.0 (phased implementation)
- **2025-11-10 (Stage 2):** Foundation complete - build system operational
- **2025-11-10 (Stage 3):** Shell complete - 3 parameters implemented
- **2025-11-10 (Stage 4):** DSP complete - all 4 phases (saturation, wow/flutter, dropout/noise, dry/wet)

**Known Issues:**
- None

**Last Updated:** 2025-11-10

### ClapMachine

**Status:** 💡 **Ideated**
**Created:** 2025-11-10
**Type:** Synth (Instrument)

**Description:**
The definitive 808-style clap synthesizer with dual-section architecture (flam + noise body) and independent multimode filtering. Designed to be "the only clap you'd ever need for house music."

**Parameters (18 total):**

Flam Section:
- Timing Spread: 0-100ms, default 15ms (time spread between layers)
- Layer Count: 2-5, default 3 (stepped knob)
- Stereo Width: 0-100%, default 50% (mono to wide spread)
- Filter Cutoff: 20Hz-20kHz, default 8kHz
- Filter Resonance: 0-100%, default 10%
- Filter Mode: LP/BP/HP, default LP (selector)

Body Section:
- Noise Type: White/Pink/Brown, default Pink (selector)
- Tone/Color: 0-100%, default 50% (brightness)
- Decay/Release: 10ms-2s, default 200ms
- Filter Cutoff: 20Hz-20kHz, default 5kHz
- Filter Resonance: 0-100%, default 5%
- Filter Mode: LP/BP/HP, default LP (selector)

Mix/Output:
- Flam/Body Balance: 0-100%, default 50%
- Drive: 0-100%, default 0% (saturation)
- Pitch: -24 to +24 semitones, default 0
- Output Level: -inf to 0dB, default -6dB

**DSP:** Dual-section synthesis - flam generator (2-5 layered transients with timing spread and stereo width) + noise body generator (white/pink/brown with envelope). Independent multimode filters (LP/BP/HP + resonance) per section. Mix stage with drive/saturation and pitch shifting.

**GUI:** Dual-panel layout with visual separation. Left: Flam controls. Right: Body controls. Center/bottom: Mix controls. 808-inspired vintage aesthetic with modern clarity.

**MIDI:** Velocity controls volume only. Wide pitch range (+/- 2 octaves) for creative use.

**Presets:** 3-5 starting points (Tight House, Classic 808, Wide & Loose, Bright & Punchy, Dark & Deep)

**Lifecycle Timeline:**
- **2025-11-10:** Creative brief completed

**Known Issues:**
- None

**Last Updated:** 2025-11-10

## Entry Template

```markdown
### [PluginName]
**Status:** [Emoji] **[State Name]**
**Created:** YYYY-MM-DD
**Version:** X.Y.Z (if applicable)
**Type:** [Audio Effect | MIDI Instrument | Synth]

**Description:**
[Brief description]

**Parameters ([N] total):**
- [Param1]: [range], default [value]
- [Param2]: [range], default [value]

**DSP:** [Architecture description]

**GUI:** [Design description]

**Lifecycle Timeline:**
- **YYYY-MM-DD (Stage 0):** Research completed
- **YYYY-MM-DD (Stage 1):** Planning - Complexity N

**Known Issues:**
- None

**Last Updated:** YYYY-MM-DD
```
