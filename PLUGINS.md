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
| TapeAge | 💡 Ideated (Paused at Stage 2) | - | 2025-11-10 |
| RadioMusic | 🚧 Stage 1 | - | 2025-01-10 |

### TapeAge

**Status:** 💡 **Ideated (Paused at Stage 2)**
**Created:** 2025-11-10
**Type:** Audio Effect

**Description:**
Vintage tape saturator with warm saturation and musical degradation (wow/flutter/dropout/noise). 60s/70s aesthetic with earth tone palette.

**Parameters (3 total):**
- DRIVE: 0-100%, default 50% (tape saturation)
- AGE: 0-100%, default 25% (tape degradation - wow/flutter/dropout/noise)
- MIX: 0-100%, default 100% (dry/wet blend)

**DSP:** Warm harmonic saturation with controllable tape artifacts (wow, flutter, dropout, noise). Musical degradation even at maximum settings.

**GUI:** Medium rectangle, vintage VU meter (output peak), 3 brass knobs horizontal, creamy beige textured background, burnt orange/brown accents, clean sans-serif all-caps typography.

**Lifecycle Timeline:**
- **2025-11-10:** Creative brief completed
- **2025-11-10 (Stage 0):** Research completed - DSP approach defined
- **2025-11-10 (Stage 1):** Planning - Complexity 3.8 (phased implementation)
- **2025-11-10 (Stage 2):** Foundation created - CMakeLists.txt, basic structure

**Known Issues:**
- None

**Last Updated:** 2025-11-10

### RadioMusic

**Status:** 🚧 **Stage 2**
**Created:** 2025-01-10
**Type:** MIDI-triggered Sampler

**Description:**
Creative audio file browser inspired by the Eurorack Radio Music module. Continuously plays files from a folder with MIDI-triggered re-sampling at user-defined start points. Discovery-focused sampling workflow.

**Parameters (4 total):**
- Station: 0-100% (file selector), default 0%
- Start: 0-100% (re-trigger position), default 0%
- Volume: -60dB to +12dB, default 0dB
- Speed/Pitch: 0.25x-4.0x, default 1.0x

**DSP:** Continuous file playback with MIDI re-triggering, tape-style speed/pitch control, seamless looping, support for 100+ files per folder.

**GUI:** Large waveform display with adaptive start position masking, folder browser, file info display, four knobs (Station/Start/Volume/Speed).

**Lifecycle Timeline:**
- **2025-01-10:** Creative brief completed
- **2025-01-10 (Stage 0):** Research completed - JUCE modules identified, professional examples analyzed
- **2025-01-10 (Stage 1):** Planning - Complexity 5.0 (phased implementation)
- **2025-01-10 (Stage 2):** Foundation - JUCE project builds successfully

**Known Issues:**
- None

**Last Updated:** 2025-01-10

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
