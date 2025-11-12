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
| TapeAge | 📦 Installed | 1.0.2 | 2025-11-11 |
| ClapMachine | 💡 Ideated | - | 2025-11-10 |
| DriveVerb | 🚧 Stage 2 | - | 2025-11-11 |
| FlutterVerb | 📦 Installed | 1.0.0 | 2025-11-11 |
| LushVerb | 🚧 Stage 1 | - | 2025-11-11 |

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

**Status:** 📦 **Installed**
**Version:** 1.0.2
**Created:** 2025-11-10
**Completed:** 2025-11-11
**Installed:** 2025-11-11
**Type:** Audio Effect

**Description:**
Vintage tape saturator with warm saturation and musical degradation (wow/flutter/dropout/noise). 60s/70s aesthetic with earth tone palette.

**Parameters (3 total):**
- DRIVE: 0-100%, default 50% (tape saturation)
- AGE: 0-100%, default 25% (tape degradation - wow/flutter/dropout/noise)
- MIX: 0-100%, default 100% (dry/wet blend)

**DSP:** Warm harmonic saturation with controllable tape artifacts (wow, flutter, dropout, noise). Musical degradation even at maximum settings. 2x oversampling for aliasing reduction.

**GUI:** Medium rectangle, vintage VU meter (output peak), 3 brass knobs horizontal, creamy beige textured background, burnt orange/brown accents, clean sans-serif all-caps typography. WebView UI (500x450px).

**Validation:**
- ✓ Factory presets: 5 presets created (Unity, Subtle Warmth, Classic Tape, Worn Out, Destroyed)
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Pluginval: Passed strictness level 10 (VST3 + AU)
- ✓ Manual testing: Verified in Standalone and DAW
- ✓ WebView integration: All parameters bound correctly, VU meter animated

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/TapeAge.vst3` (4.1 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/TapeAge.component` (4.1 MB)

**Lifecycle Timeline:**
- **2025-11-10:** Creative brief completed
- **2025-11-10 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-10 (Stage 1):** Planning - Complexity 5.0 (phased implementation)
- **2025-11-10 (Stage 2):** Foundation complete - build system operational
- **2025-11-10 (Stage 3):** Shell complete - 3 parameters implemented
- **2025-11-10 (Stage 4):** DSP complete - all 4 phases (saturation, wow/flutter, dropout/noise, dry/wet)
- **2025-11-10 (Stage 5):** GUI complete - WebView integrated with 3 parameter bindings and VU meter animation
- **2025-11-11 (Stage 6):** Validation complete - pluginval passed, factory presets created, ready for installation
- **2025-11-11 (v1.0.0):** Installed to system folders (VST3 + AU)
- **2025-11-11 (v1.0.1):** Fixed parameter state persistence in Ableton (getNumPrograms() interference)
- **2025-11-11 (v1.0.2):** Fixed WebView parameter initialization (valueChangedEvent undefined callback parameters)

**Known Issues:**
- None

**Last Updated:** 2025-11-11

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

### DriveVerb

**Status:** 🚧 **Stage 4**
**Created:** 2025-11-11
**Type:** Audio Effect (Reverb)
**Complexity:** 5.0 / 5.0 (maximum - phased implementation)

**Description:**
Algorithmic reverb with warm tape saturation applied only to the wet signal, plus DJ-style filter with pre/post drive routing.

**Parameters (6 total):**
- Dry/Wet: 0-100%, default 30% (signal blend)
- Drive: 0-24dB, default 6dB (tape saturation, wet signal only, even harmonics)
- Decay: 0.5s-10s, default 2s (reverb tail length)
- Size: 0-100%, default 40% (room dimensions)
- Filter: -100% to +100%, default 0% (DJ-style: negative=low-pass, positive=high-pass, 0%=bypass)
- Filter Position: Pre/Post, default Post (switch: filter before or after drive)

**DSP:** Algorithmic reverb engine with independent size and decay control. Warm tape saturation (even harmonics) on wet signal only. DJ-style filter (Butterworth IIR, 200Hz-20kHz sweep) with pre/post drive routing. Signal flow configurable via switch.

**GUI:** 5 rotary knobs + 1 toggle switch. Modern/clean design with vintage warmth. Parameter value displays.

**Use Cases:**
- Driven ambience for drums, synths, sound design
- Creative reverb where clean reverb is too polite
- Filtered, saturated reverb tails

**Lifecycle Timeline:**
- **2025-11-11:** Creative brief completed
- **2025-11-11 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-11 (Stage 1):** Planning - Complexity 5.0 (phased implementation)
- **2025-11-11 (Stage 2):** Foundation - Build system operational, compiles successfully
- **2025-11-11 (Stage 3):** Shell complete - 6 parameters implemented
- **2025-11-11 (Stage 4.1):** Core reverb + dry/wet mixing (juce::dsp::Reverb + DryWetMixer)
- **2025-11-11 (Stage 4.2):** Drive saturation (tanh waveshaping, 0-24dB, wet only)
- **2025-11-11 (Stage 4.3):** DJ-style filter (IIR, exponential mapping, center bypass)
- **2025-11-12 (Stage 4.4):** PRE/POST routing (conditional branching for filter placement)
- **2025-11-12 (Stage 5.1):** WebView layout + 5 interactive knobs (size, decay, dryWet, drive, filter)
- **2025-11-12 (Stage 5.2):** Toggle switch + VU meter animation (filterPosition PRE/POST, ballistic motion)

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### FlutterVerb

**Status:** 📦 **Installed**
**Version:** 1.0.1
**Created:** 2025-11-11
**Completed:** 2025-11-11
**Installed:** 2025-11-11
**Type:** Audio Effect (Reverb)
**Complexity:** 5.0 (Complex)

**Description:**
Tape-driven plate reverb with extreme wow and flutter modulation for textured, analog-sounding spaces. Combines modern algorithmic clarity with vintage tape character.

**Parameters (7 total):**
- SIZE: 0-100%, default 50% (room dimensions)
- DECAY: 0.1s-10s, default 2.5s (reverb tail length)
- MIX: 0-100%, default 25% (dry/wet blend)
- AGE: 0-100%, default 20% (tape character intensity - combines wow and flutter modulation depth)
- DRIVE: 0-100%, default 20% (tape saturation/warmth)
- TONE: -100% to +100%, default 0% (DJ-style filter: negative=low-pass, positive=high-pass, 0%=bypass)
- MOD_MODE: Toggle (Wet Only / Wet + Dry) - applies modulation to dry signal when enabled

**DSP:** Plate reverb algorithm with size-controlled early reflections. Dual LFO wow/flutter modulation (different rates) controlled by Age parameter. Soft-clipping tape saturation with adjustable drive. DJ-style exponential filter (same implementation as GainKnob). User-selectable modulation routing (wet-only or wet+dry).

**GUI:** WebView UI (600×640px). TapeAge-inspired dark radial gradient with brass accents. VU meter at top (output peak with ballistics), two rows of knobs (Size/Decay/Mix, Age/Drive/Tone), horizontal toggle below Tone knob. Typewriter/monospace typography. All 7 parameters bound with bidirectional sync (UI ↔ DSP).

**Features:**
- Dual LFO modulation (wow: 1Hz, flutter: 6Hz)
- Lagrange 3rd-order interpolation for smooth pitch modulation
- Exponential filter cutoff mapping for musical response
- VU meter ballistics (fast attack, slow release)
- Bypass zone for center filter position (transparent)
- State reset on filter type transitions (no burst artifacts)
- 7 factory presets showcasing different use cases

**Validation:**
- ✓ Factory presets: 7 presets created (Default, Small Room, Large Hall, Tape Warble, Dark Ambient, Bright Plate, Lo-Fi Tape)
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Manual testing: Plugin loads and processes audio correctly
- ✓ All parameters functional with proper ranges
- ✓ VU meter animates correctly
- ⚠ pluginval: Skipped (not installed on system)

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/FlutterVerb.vst3` (4.2 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/FlutterVerb.component` (4.1 MB)

**UI Mockup:** v6 (finalized 2025-11-11)
- Design validated against creative brief (no drift)
- 7 implementation files generated (HTML, C++ header/impl, CMake, checklist)
- Fully integrated in Stage 5 (WebView + parameter binding + VU meter)

**Use Cases:**
- Textured lead vocals with evolving, warbling space
- Piano and keys with lush, moving reverb tails
- Guitar ambience with vintage spring-like wobble
- Mix bus texture for cohesive analog glue
- Creative sound design with extreme pitch-shifted textures

**Lifecycle Timeline:**
- **2025-11-11:** Creative brief completed
- **2025-11-11:** UI mockup v6 finalized (600×640px, TapeAge-inspired design)
- **2025-11-11:** design-sync validation passed (no drift detected)
- **2025-11-11:** Implementation files generated (ready for `/implement FlutterVerb`)
- **2025-11-11 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-11 (Stage 1):** Planning - Complexity 5.0 (phased implementation)
- **2025-11-11 (Stage 2):** Foundation complete - build system operational
- **2025-11-11 (Stage 3):** Shell complete - 7 parameters implemented
- **2025-11-11 (Stage 4.1):** Core reverb complete - juce::dsp::Reverb + DryWetMixer operational (SIZE, DECAY, MIX parameters)
- **2025-11-11 (Stage 4.2):** Modulation system complete - dual LFO (wow + flutter) with delay-based pitch shifting (AGE parameter)
- **2025-11-11 (Stage 4.3):** Saturation and filter complete - tape warmth via tanh() + DJ-style IIR filter (DRIVE, TONE parameters)
- **2025-11-11 (Stage 4.4):** MOD_MODE routing complete - wet-only vs wet+dry modulation routing
- **2025-11-11 (Stage 5.1):** GUI layout complete - WebView infrastructure with 7 relays + attachments
- **2025-11-11 (Stage 5.2):** Parameter binding complete - all 7 parameters bound to UI controls (Pattern #15, #16)
- **2025-11-11 (Stage 5.3):** VU meter complete - real-time output level display with ballistics (fast attack, slow release)
- **2025-11-11 (Stage 6):** Validation complete - 7 factory presets created, CHANGELOG.md generated, ready for installation
- **2025-11-11 (v1.0.0):** Installed to system folders (VST3 + AU)
- **2025-11-11 (v1.0.1):** Bug fixes - decay independence, drive latency compensation, age scaling, MOD_MODE toggle, VU meter

**Known Issues:**
- None

**Last Updated:** 2025-11-11

### LushVerb

**Status:** 📦 **Installed**
**Version:** 1.0.0
**Created:** 2025-11-11
**Completed:** 2025-11-11
**Installed:** 2025-11-11
**Type:** Audio Effect (Reverb)
**Complexity:** 5.0 (maximum - phased implementation)

**Description:**
Stunning, lush algorithmic reverb inspired by Strymon BigSky. Four simple controls deliver consistently gorgeous ambient/ethereal reverb with infinite tails, built-in modulation, and subtle shimmer.

**Parameters (4 total):**
- SIZE: 0.5s-20s, default 2.5s (reverb tail length)
- DAMPING: 0-100%, default 30% (high-frequency rolloff)
- SHIMMER: 0-100%, default 30% (+1 octave pitch-shifted signal amount)
- MIX: 0-100%, default 30% (dry/wet blend)

**DSP:**
- Core: juce::dsp::Reverb engine with SIZE and DAMPING control
- Shimmer: FFT-based phase vocoder (+1 octave, 2048-point FFT with 4x overlap)
- Modulation: Dual LFO (0.3Hz + 0.5Hz) with Lagrange interpolation
- Routing: Input → Shimmer → Reverb → Modulation → Dry/Wet Mix
- Latency: ~46ms (2048 samples at 44.1kHz)

**GUI:** Industrial 19" rack unit aesthetic (500×300px). WebView UI with 4 interactive rotary knobs, real-time LED output meter with ballistic motion, gold accents on brushed metal texture.

**Validation:**
- ✓ Factory presets: 7 presets (Default, Small Room, Large Hall, Shimmer Pad, Dark Ambient, Bright Plate, Instant Inspiration)
- ✓ CHANGELOG.md: Generated in Keep a Changelog format (v1.0.0)
- ✓ Build verification: Compiles with zero errors
- ⚠ Pluginval: Skipped (not installed on system)

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/LushVerb.vst3` (4.2 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/LushVerb.component` (4.1 MB)

**Use Cases:**
- Versatile mixing tool (vocals, instruments)
- Creative ambient soundscapes
- Instant inspiration—always sounds gorgeous

**Inspirations:**
- Strymon BigSky (lush algorithmic reverb)
- Valhalla VintageVerb (smooth, musical)
- Eventide Blackhole (infinite tails)

**Lifecycle Timeline:**
- **2025-11-11:** Creative brief completed
- **2025-11-11:** UI mockup v3 finalized
- **2025-11-11 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-11 (Stage 1):** Planning complete - Complexity 5.0 (phased implementation, 3 DSP phases + 3 GUI phases)
- **2025-11-11 (Stage 2):** Foundation complete - Build system operational, compiles successfully
- **2025-11-11 (Stage 3):** Shell complete - 4 parameters implemented (SIZE, DAMPING, SHIMMER, MIX)
- **2025-11-11 (Stage 4.1):** Core reverb complete - juce::dsp::Reverb + DryWetMixer operational
- **2025-11-11 (Stage 4.2):** Modulation complete - Dual LFO system with delay line
- **2025-11-11 (Stage 4.3):** Shimmer complete - FFT phase vocoder (+1 octave)
- **2025-11-11 (Stage 5.1):** GUI layout complete - WebView integration with v3 mockup
- **2025-11-11 (Stage 5.2):** Parameter binding complete - 4 interactive knobs with relative drag
- **2025-11-11 (Stage 5.3):** LED meter complete - Real-time output visualization with ballistics
- **2025-11-11 (Stage 6):** Validation complete - 7 factory presets, CHANGELOG.md
- **2025-11-11 (v1.0.0):** Installed to system folders (VST3 + AU)

**Known Issues:**
- None

**Last Updated:** 2025-11-11

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
