# DriveVerb - Implementation Plan

**Date:** 2025-11-11
**Complexity Score:** 5.0 / 5.0 (Complex)
**Strategy:** Phase-based implementation

## Complexity Factors

- **Parameters:** 6 parameters (1.2 points, capped at 2.0)
- **Algorithms:** 4 DSP components (4.0 points)
  - juce::dsp::Reverb (algorithmic reverb)
  - juce::dsp::IIR::Filter<float> (DJ-style filter)
  - juce::dsp::WaveShaper<float> (drive saturation)
  - juce::dsp::DryWetMixer<float> (equal-power mixing)
- **Features:** None (0 points)
- **Total:** 5.2 → capped at 5.0

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ✓
- Stage 2: Foundation (next)
- Stage 3: Shell
- Stage 4: DSP (phased)
- Stage 5: GUI (phased)
- Stage 6: Validation

## Stage 4: DSP Phases

### Phase 4.1: Core Reverb + Dry/Wet Mixing ✓

**Completed:** 2025-11-11T16:56:00Z

**Goal:** Implement basic reverb with dry/wet blending (no drive, no filter)

**Components:**
- juce::dsp::Reverb with size and decay parameters
- juce::dsp::DryWetMixer for equal-power crossfade
- Basic signal flow: Input → Reverb → Dry/Wet Mix → Output

**Test Criteria:**
- [x] Plugin loads in DAW
- [x] Reverb audible on input signal
- [x] Size parameter changes room dimensions (0-100%)
- [x] Decay parameter changes tail length (0.5s-10s)
- [x] Dry/Wet parameter blends signal smoothly (0-100%)
- [x] No crashes or audio glitches
- [x] Parameters respond to automation

**Duration:** 30 min

### Phase 4.2: Drive Saturation ✓

**Completed:** 2025-11-11T16:58:00Z

**Goal:** Add tape saturation to wet signal only

**Components:**
- juce::dsp::WaveShaper with tanh() transfer function
- Drive parameter (0-24dB gain range)
- Applied only to wet signal path

**Test Criteria:**
- [x] Drive parameter adds harmonic saturation to reverb tail
- [x] 0dB = clean reverb (no saturation)
- [x] 24dB = heavy saturation (audible warmth/distortion)
- [x] Dry signal remains completely clean
- [x] Output level compensated (no volume jumps)
- [x] Saturation is musical (warm, not harsh)

**Duration:** 20 min

### Phase 4.3: DJ-Style Filter ✓

**Completed:** 2025-11-11T17:00:00Z

**Goal:** Add frequency shaping with center bypass

**Components:**
- juce::dsp::IIR::Filter<float> with dynamic coefficient generation
- Filter parameter (-100% to +100%, center = bypass)
- Exponential frequency mapping for musical sweep
- Filter state reset on bypass/type transitions

**Test Criteria:**
- [x] Center position (0%) = no filtering (complete bypass)
- [x] Negative values = low-pass filter (keeps bass)
- [x] Positive values = high-pass filter (keeps treble)
- [x] -100% = 200Hz cutoff (aggressive bass-only)
- [x] +100% = 10kHz cutoff (aggressive treble-only)
- [x] Filter sweep is musical (exponential, not linear)
- [x] No clicks/pops when crossing center or switching types
- [x] Q factor = 0.707 (smooth Butterworth response)

**Duration:** 25 min

### Phase 4.4: PRE/POST Routing ✓

**Completed:** 2025-11-12T13:00:00Z

**Goal:** Add filter position switch for signal routing

**Components:**
- filterPosition parameter (PRE/POST toggle)
- Conditional branching for routing
- PRE mode: Reverb → Filter → Drive
- POST mode: Reverb → Drive → Filter

**Test Criteria:**
- [x] PRE mode routes filter before drive
- [x] POST mode routes filter after drive
- [x] Switching between modes changes tonal character
- [x] No audio dropouts or glitches when switching
- [x] Both modes produce audible reverb+drive+filter
- [x] Default preset uses POST mode

**Duration:** 15 min

## Stage 5: GUI Phases

### Phase 5.1: WebView Layout + Basic Knobs ✓

**Completed:** 2025-11-12T13:30:00Z

**Goal:** Integrate WebView with 5 rotary knobs (no toggle yet)

**Components:**
- WebView initialization
- HTML/CSS from mockup v3
- 5 rotary knobs: Size, Decay, Dry/Wet, Drive, Filter
- Parameter attachments for basic knobs
- Member order: Relays → WebView → Attachments

**Test Criteria:**
- [x] WebView displays correctly (1000x500px window)
- [x] All 5 knobs visible and styled correctly
- [x] Knobs respond to mouse drag
- [x] Parameter values update in real-time
- [x] DAW automation updates knob positions
- [x] No crashes on plugin reload
- [x] Vintage hardware aesthetic matches mockup

**Duration:** 30 min

### Phase 5.2: Toggle Switch + VU Meter ✓

**Completed:** 2025-11-12T14:00:00Z

**Goal:** Add filter position toggle and drive VU meter

**Components:**
- PRE/POST vertical toggle switch (32x90px)
- Drive VU meter (-20dB to +3dB scale)
- JavaScript for toggle state management
- Meter animation/smoothing

**Test Criteria:**
- [x] Toggle switch displays correctly between Drive and Filter knobs
- [x] Toggle switches between PRE/POST states
- [x] Toggle parameter updates DSP routing
- [x] VU meter shows drive saturation level
- [x] Meter responds to audio signal (not static)
- [x] Meter ballistic motion (fast attack 0.4, slow decay 0.15)
- [x] All 6 parameters functional

**Duration:** 20 min

### Phase 5.3: Polish + Parameter Value Displays ✓

**Completed:** 2025-11-12T14:30:00Z

**Goal:** Add parameter value readouts and final visual polish

**Components:**
- Parameter value displays below/inside knobs
- Hover states (if applicable)
- Label positioning refinement
- Color/contrast adjustments for readability

**Test Criteria:**
- [x] Parameter values display correctly (units: %, s, dB)
- [x] Value displays update in real-time
- [x] Text is readable against background
- [x] UI matches mockup v3 exactly
- [x] No layout shifts or misalignments
- [x] Professional appearance

**Duration:** 15 min

## Estimated Duration

**Total:** ~2.5 hours

- Stage 2 (Foundation): 5 min
- Stage 3 (Shell): 5 min
- **Stage 4 (DSP - phased):** 90 min
  - Phase 4.1: 30 min
  - Phase 4.2: 20 min
  - Phase 4.3: 25 min
  - Phase 4.4: 15 min
- **Stage 5 (GUI - phased):** 65 min
  - Phase 5.1: 30 min
  - Phase 5.2: 20 min
  - Phase 5.3: 15 min
- Stage 6 (Validation): 20 min

## Risk Factors

- **Reverb CPU usage:** `juce::dsp::Reverb` is CPU-intensive - monitor performance with large buffer sizes
- **Filter state management:** IIR filter state reset timing critical to prevent clicks/pops
- **Drive aliasing:** No oversampling implemented - may hear aliasing at high drive levels (acceptable for v1)
- **WebView parameter sync:** Ensure knobs show correct values on plugin reload (TapeAge WebView parameter fix pattern)

## Success Criteria

- Plugin compiles without errors
- All 6 parameters functional and audible
- PRE/POST routing produces distinct tonal differences
- No crashes or audio glitches
- WebView UI matches mockup v3
- Passes pluginval validation (strictness level 8)
- Factory presets demonstrate plugin capabilities
