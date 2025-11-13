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
| TapeAge | 📦 Installed | 1.0.3 | 2025-11-12 |
| ClapMachine | 💡 Ideated | - | 2025-11-10 |
| DriveVerb | 📦 Installed | 1.0.2 | 2025-11-12 |
| FlutterVerb | 📦 Installed | 1.0.3 | 2025-11-12 |
| LushVerb | 💡 Ideated | - | 2025-11-12 |
| OrganicHats | 📦 Installed | 1.0.0 | 2025-11-12 |
| DrumRoulette | 📦 Installed | 1.0.0 | 2025-11-12 |
| Scatter | 💡 Ideated | - | 2025-11-12 |
| MinimalKick | 🚧 Stage 0 | - | 2025-11-12 |

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
**Version:** 1.0.3
**Created:** 2025-11-10
**Completed:** 2025-11-11
**Installed:** 2025-11-12
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
- **2025-11-12 (v1.0.3):** Fixed knob hover shake - replaced scale transform with border color change

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### DriveVerb

**Status:** 📦 **Installed**
**Version:** 1.0.2
**Created:** 2025-11-11
**Completed:** 2025-11-12
**Installed:** 2025-11-12
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
- **2025-11-12 (Stage 5.3):** Parameter value displays + polish (real-time readouts with units)
- **2025-11-12 (Stage 6):** Validation complete - 5 factory presets, CHANGELOG.md generated

**Validation:**
- ✓ Factory presets: 5 presets created (Default, Subtle Warmth, Driven Ambience, Dark Filtered Hall, Aggressive Saturation)
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Build verification: Compiles successfully (VST3, AU, Standalone)
- ✓ Manual testing: All 6 parameters functional, PRE/POST routing audible, VU meter responsive

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/DriveVerb.vst3` (4.2 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/DriveVerb.component` (4.1 MB)

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
- **2025-11-12 (Stage 5.3):** Parameter value displays + polish (real-time readouts with units)
- **2025-11-12 (Stage 6):** Validation complete - 5 factory presets, CHANGELOG.md generated
- **2025-11-12 (v1.0.0):** Installed to system folders (VST3 + AU)
- **2025-11-12 (v1.0.1):** UI control reordering - dry/wet moved to end for standard mixing workflow
- **2025-11-12 (v1.0.2):** Fixed knob hover shake - replaced scale transform with border color change

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### FlutterVerb

**Status:** 📦 **Installed**
**Version:** 1.0.3
**Created:** 2025-11-11
**Completed:** 2025-11-11
**Installed:** 2025-11-12
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
- **2025-11-11 (v1.0.2):** Bug fixes - VU meter animation, MOD_MODE toggle UI, MOD_MODE functionality, SIZE/DECAY independence
- **2025-11-12 (v1.0.3):** Fixed knob hover shake - replaced scale transform with border color change

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### LushVerb

**Status:** 💡 **Ideated**
**Created:** 2025-11-11
**Type:** Audio Effect (Reverb)

**Description:**
Stunning, lush algorithmic reverb inspired by Strymon BigSky. Four simple controls deliver consistently gorgeous ambient/ethereal reverb with infinite tails, built-in modulation, and subtle shimmer.

**Parameters (4 total):**
- SIZE: 0.5s-20s, default 2.5s (reverb tail length)
- DAMPING: 0-100%, default 30% (high-frequency rolloff)
- SHIMMER: 0-100%, default 30% (+1 octave pitch-shifted signal amount)
- MIX: 0-100%, default 30% (dry/wet blend)

**Preserved Artifacts:**
- Creative brief: `.ideas/creative-brief.md`
- Parameter spec: `.ideas/parameter-spec.md`
- UI mockups: `.ideas/mockups/` (v3 finalized)

**Backup:**
- Implementation archived: `backups/rollbacks/LushVerb_reset_20251112_223045.tar.gz` (57 KB)

**Lifecycle Timeline:**
- **2025-11-11:** Creative brief completed
- **2025-11-11:** UI mockup v3 finalized
- **2025-11-11:** Implementation completed through Stage 6 (v1.0.0-v1.0.3)
- **2025-11-12:** Reset to ideation - implementation removed, concept preserved

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### OrganicHats

**Status:** 📦 **Installed**
**Version:** 1.0.0
**Created:** 2025-11-12
**Completed:** 2025-11-12
**Installed:** 2025-11-12
**Type:** Synth (Instrument)
**Complexity:** 5.0 (Complex)

**Description:**
Warm, organic noise-based hi-hat synthesizer with separate closed and open sounds. Inspired by CR-78/808 drum machines but with warmer, less metallic character. Features instant choking behavior (closed chokes open).

**Parameters (8 total):**

Closed Hi-Hat:
- Tone: 0-100%, default 50% (brightness control)
- Decay: 20-200ms, default 80ms (envelope decay time)
- Noise Color: 0-100%, default 50% (warmth vs brightness filtering)

Open Hi-Hat:
- Tone: 0-100%, default 50% (brightness control, shared character with closed)
- Release: 100ms-1s, default 400ms (sustained decay time)
- Noise Color: 0-100%, default 50% (warmth vs brightness filtering)

Global:
- Velocity > Volume: 0-100%, default 100% (velocity sensitivity to volume)
- Velocity > Tone: 0-100%, default 50% (velocity sensitivity to brightness)

**DSP:** Filtered noise synthesis with fixed resonance peaks for organic body. Static noise generation (no random variation). Closed and open sounds use separate MIDI note triggers. Instant choke behavior (<5ms) when closed chokes open. Velocity affects both volume and tone brightness.

**GUI:** Dual-panel layout with separate sections for closed (left) and open (right) hi-hats. Global controls section. Visual indication of active sound and choke behavior. User preset save/recall capability.

**MIDI:** Different MIDI notes for closed and open (e.g., C1 = closed, D1 = open). Velocity controls volume and tone brightness. Closed hi-hat trigger instantly cuts open hi-hat.

**Presets:** User preset save/recall only (no factory presets).

**Use Cases:**
- Electronic music production requiring warm, synthesized hi-hat sounds
- Layering with acoustic drum samples for synthetic character
- Creating unique percussive textures blending organic warmth with electronic precision
- Alternative to harsh digital hi-hat samples in house, techno, and ambient

**Inspirations:**
- CR-78/808 drum machines (classic electronic character but warmer)
- Organic analog drum machines (Jomox, Vermona)

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/OrganicHats.vst3` (3.5 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/OrganicHats.component` (3.4 MB)

**Lifecycle Timeline:**
- **2025-11-12:** Creative brief completed
- **2025-11-12 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-12 (Stage 1):** Planning - Complexity 5.0 (phased implementation)
- **2025-11-12 (Stage 2):** Foundation - Build system operational, compiles successfully
- **2025-11-12 (Stage 3):** Shell complete - 6 parameters implemented
- **2025-11-12 (Stage 4.1):** Voice architecture + noise generation (C1/D1 MIDI routing, ADSR envelopes)
- **2025-11-12 (Stage 4.2):** Filtering + tone shaping (Tone filter 3-15kHz, Noise Color filter 5-10kHz, velocity modulation)
- **2025-11-12 (Stage 4.3):** Resonators + choke logic (3 fixed peaks 7/10/13kHz, closed chokes open)
- **2025-11-12 (Stage 5.1):** WebView layout integration (600×590px, studio hardware aesthetic)
- **2025-11-12 (Stage 5.2):** Parameter binding complete (6 parameters with ES6 modules and relative drag)
- **2025-11-12 (Stage 5.3):** Power LED and polish (pulse animation, final styling)
- **2025-11-12 (Stage 6):** Validation complete - Build verified, CHANGELOG created
- **2025-11-12 (v1.0.0):** Installed to system folders (VST3 + AU)

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### DrumRoulette

**Status:** 📦 **Installed**
**Version:** 1.0.0
**Created:** 2025-11-12
**Completed:** 2025-11-12
**Installed:** 2025-11-12
**Type:** Instrument (Drum Sampler)
**Complexity:** 5.0 / 5.0 (maximum - phased implementation)

**Description:**
Eight-slot drum sampler with folder-based randomization and mixer-style interface. Each slot connects to a folder for random sample selection, with per-channel lock controls and multi-output routing.

**Parameters (73 total):**

Global:
- Randomize All: Button (randomize all unlocked slots)

Per-Slot (×8):
- Folder Path: File Browser
- Randomize: Button (pick random sample from folder)
- Lock: Toggle (exclude from global randomization)
- Volume: Fader (-inf to +6dB), default 0dB
- Decay: 10ms-2s, default 500ms (envelope decay, sustain=0)
- Attack: 0-50ms, default 1ms (percussive shaping)
- Tilt Filter: -12dB to +12dB, default 0dB (brightness, pivot at 1kHz)
- Pitch: ±12 semitones, default 0
- Solo: Toggle, default Off
- Mute: Toggle, default Off

**DSP:** Sample playback engine using juce::Synthesiser with 8 custom voices. ADSR envelope (sustain=0). Tilt filter (cascaded low/high shelving filters at 1kHz pivot). Pitch shifting via variable-rate resampling with linear interpolation. True random file selection with recursive folder scanning. Multi-output routing (18 channels: 2 main + 16 individual).

**GUI:** Eight vertical channel strips (mixer console style). LED-style trigger indicators. Color-coded lock icons (grey=unlocked, highlighted=locked). Global randomize button. Vintage hardware mixer aesthetic with brushed metal texture (1400×950px).

**MIDI:** C1-G1 (8 chromatic notes), velocity-sensitive.

**Audio Routing:** Automatic multi-output (stereo main + 8 individual stereo outputs = 18 total).

**Folder Handling:**
- Recursive subfolder scanning
- True random selection (any file each time)
- Error message on empty folders
- Manual folder selection each session

**Lifecycle Timeline:**
- **2025-11-12:** Creative brief completed
- **2025-11-12 (Stage 0):** Research completed - DSP architecture documented
- **2025-11-12 (Stage 1):** Planning - Complexity 5.0 (phased implementation: 4 DSP phases + 3 GUI phases)
- **2025-11-12 (Stage 2):** Foundation - Build system operational, compiles successfully
- **2025-11-12 (Stage 3):** Shell complete - 73 parameters implemented (65 APVTS + 8 deferred to Stage 4)
- **2025-11-12 (Stage 4.1):** Voice architecture + sample playback - 8 voices, MIDI mapping C1-G1, multi-output (18 channels)
- **2025-11-12 (Stage 4.2):** Envelope + pitch shifting - ADSR per voice (sustain=0), pitch ±12 semitones, linear interpolation
- **2025-11-12 (Stage 4.3):** Tilt filter + volume control - cascaded shelving filters at 1kHz, per-voice gain (-60dB to +6dB)
- **2025-11-12 (Stage 4.4):** Folder randomization + solo/mute - recursive scanning, random selection, priority routing
- **2025-11-12 (Stage 4 COMPLETE):** All DSP components functional - ready for GUI integration
- **2025-11-12 (Stage 5 Phase 5.1):** WebView layout setup - 8 channel strips, JUCE patterns applied, zero errors
- **2025-11-12 (Stage 5 Phase 5.2):** Parameter binding slots 1-2 - 16 parameters bound, correct member order, zero errors
- **2025-11-12 (Stage 5 Phase 5.3):** Parameter binding slots 3-8 - 48 parameters bound, 100% contract compliance
- **2025-11-12 (Stage 5 COMPLETE):** All GUI integration finished - 64 parameters bound, all JUCE 8 patterns applied
- **2025-11-12 (Stage 6):** Validation complete - 5 factory presets created, CHANGELOG.md generated, ready for installation

**Validation:**
- ✓ Factory presets: 5 presets created (Default Kit, Tight Drums, Dark and Deep, Bright and Crispy, Lo-Fi Character)
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Build verification: Compiles successfully (VST3, AU, Standalone) - 5.5MB Release binary
- ✓ Parameter count: 73 parameters (1 global + 72 per-slot) - matches parameter-spec.md exactly
- ✓ DSP chain: All 4 phases verified (Voice architecture, Envelope+Pitch, Tilt+Volume, Randomization+Solo/Mute)
- ✓ GUI bindings: 64 parameter attachments (8 slots × 8 params) - 100% contract compliance

**Formats:** VST3, AU, Standalone

**Installation Locations:**
- VST3: `~/Library/Audio/Plug-Ins/VST3/DrumRoulette.vst3` (5.5 MB)
- AU: `~/Library/Audio/Plug-Ins/Components/DrumRoulette.component` (5.4 MB)

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### Scatter

**Status:** 💡 **Ideated**
**Created:** 2025-11-12
**Type:** Audio Effect (Delay)

**Description:**
Granular reversed delay with beautiful stuttering grains, randomized pitch (quantized to musical scales), and randomized stereo placement. Creates evolving textural ambience with musical coherence.

**Parameters (9 total):**
- Delay Time: 100ms-2s (synced), default 500ms (buffer length, tempo-synced note values)
- Grain Size: 5-500ms, default 100ms (individual grain length)
- Grain Density: 0-100%, default 50% (overlap amount between grains)
- Pitch Random: 0-100%, default 30% (pitch randomization amount, ±7 semitones max)
- Scale: Chromatic/Major/Minor/etc, default Chromatic (quantization scale)
- Root Note: C-B, default C (scale root)
- Pan Random: 0-100%, default 75% (stereo randomization amount)
- Feedback: 0-100%, default 30% (traditional delay feedback)
- Mix: 0-100%, default 50% (dry/wet blend)

**DSP:** Circular delay buffer with tempo sync. Grain scheduler with overlap-based density control. Per-grain reverse playback with windowing (Hann/Hamming). Pitch-shifting per grain with scale quantization (±7 semitone range). Random pan position generator. Each grain randomly forward or reverse.

**GUI:** Single-page interface with grain cloud visualization (shows position in time/stereo/pitch space). Tempo sync indicator. Scale/root note selector. Visual feedback for density and randomization.

**Use Cases:**
- Creating evolving soundscapes and atmospheric beds
- Textural ambience for melodic loops or pads
- Transforming percussion into stuttering, pitched textures
- Adding ambient depth with quantized harmonic delays
- Sound design for film/games requiring abstract, musical atmospheres

**Inspirations:**
- Granular: GrainScanner, Portal, Granite, Iris
- Ambient: Cosmos, Shimmer, CloudSeed
- Reverse delays: Backmask, H-Delay reverse mode, EchoBoy

**Lifecycle Timeline:**
- **2025-11-12:** Creative brief completed

**Known Issues:**
- None

**Last Updated:** 2025-11-12

### MinimalKick

**Status:** 🚧 Stage 3
**Type:** Synth
**Created:** 2025-11-12
**Complexity:** 5.0 (Complex - phased implementation)

**Description:**
Minimal house kick drum synthesizer with sine wave + pitch envelope architecture for deep, subby kicks that sit perfectly in minimal and tech house tracks.

**Parameters (5 total):**
- Sweep: 0-24 semitones, default 12 st (pitch envelope starting offset)
- Time: 5-500 ms, default 50 ms (pitch envelope decay time)
- Attack: 0-50 ms, default 5 ms (amplitude envelope attack)
- Decay: 50-2000 ms, default 400 ms (amplitude envelope decay)
- Drive: 0-100%, default 20% (saturation/harmonics)

**DSP:** Sine oscillator + exponential pitch envelope + AD amplitude envelope + tanh saturation. Monophonic, retriggerable. Estimated CPU: ~11% single core.

**Implementation Strategy:** Phased (6 phases: 3 DSP + 3 GUI)
- Stage 4.1: Core synthesis (oscillator + MIDI + amplitude)
- Stage 4.2: Pitch envelope (custom exponential - highest risk)
- Stage 4.3: Saturation/drive
- Stage 5.1-5.3: WebView UI (layout, binding, polish)

**Lifecycle Timeline:**
- **2025-11-12:** Creative brief completed
- **2025-11-12:** UI mockup v2 finalized (5 knobs, vintage hardware aesthetic)
- **2025-11-12 (Stage 0):** Research completed - DSP architecture documented (Tier 4, 6 features)
- **2025-11-13 (Stage 1):** Planning completed - Complexity 5.0, 6-phase implementation plan created
- **2025-11-13 (Stage 2):** Foundation complete - Project structure created, JUCE 8 patterns followed
- **2025-11-13 (Stage 3):** Shell complete - 5 APVTS parameters implemented (sweep, time, attack, decay, drive)

**Known Issues:**
- None

**Last Updated:** 2025-11-13

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
