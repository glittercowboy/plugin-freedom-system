# FlutterVerb - Implementation Plan

**Date:** 2025-11-11
**Complexity Score:** 5.0 (Complex)
**Strategy:** Phase-based implementation

## Complexity Factors

- Parameters: 7 parameters (1.4 points, capped at 2.0)
- Algorithms: 7 DSP components (Reverb, Wow LFO, Flutter LFO, DelayLine, Saturation, Filter, DryWetMixer)
- Features: Dual LFO modulation system (1 point)
- Total: 9.4 (capped at 5.0)

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ✓
- Stage 2: Foundation ✓
- Stage 3: Shell ✓
- Stage 4: DSP (phased) - Phase 4.1 in progress
- Stage 5: GUI (phased)
- Stage 6: Validation

## Stage 4: DSP Phases

### Phase 4.1: Core Reverb Processing ✓

**Completed:** 2025-11-11
**Goal:** Implement basic plate reverb with dry/wet mixing
**Components:**
- juce::dsp::Reverb initialization and configuration
- SIZE and DECAY parameter mapping
- juce::dsp::DryWetMixer for MIX parameter
- Basic audio path: Input → Reverb → DryWetMixer → Output

**Test Criteria:**
- [ ] Plugin loads in DAW without crashes
- [ ] Audio passes through (wet and dry signals audible)
- [ ] SIZE parameter affects reverb character
- [ ] DECAY parameter affects tail length
- [ ] MIX parameter blends dry/wet correctly

**Duration:** 15 min

**Implementation Notes:**
- Added juce::dsp::Reverb and juce::dsp::DryWetMixer member variables
- Implemented prepareToPlay() with component initialization
- Implemented processBlock() with SIZE/DECAY/MIX parameter mapping
- Decay mapped to damping using inverse relationship (short decay = high damping)
- Reverb configured for full stereo width, no freeze mode
- DryWetMixer handles dry/wet blend with MIX parameter
- Added juce_dsp module to CMakeLists.txt
- Real-time safety: All allocations in prepareToPlay(), atomic parameter reads
- Multi-channel support: Handles mono, stereo, and multi-channel configurations

### Phase 4.2: Modulation System

**Goal:** Add wow and flutter pitch modulation with delay line
**Components:**
- Dual LFO generators (Wow: 0.5-1.5Hz, Flutter: 4-8Hz)
- juce::dsp::DelayLine with Lagrange3rd interpolation
- AGE parameter controls modulation depth
- Per-channel phase tracking

**Test Criteria:**
- [ ] AGE parameter creates audible pitch warbling
- [ ] Modulation is smooth without clicks/artifacts
- [ ] Wow (slow) and flutter (fast) both contribute
- [ ] Stereo channels maintain independent phase

**Duration:** 20 min

### Phase 4.3: Saturation and Filter

**Goal:** Add tape saturation and DJ-style filter
**Components:**
- Tanh waveshaper with DRIVE parameter (1.0-10.0 gain)
- juce::dsp::IIR::Filter with exponential cutoff mapping
- TONE parameter (-100 to +100): LP/HP crossfade
- State reset on filter type change (prevent bursts)

**Test Criteria:**
- [ ] DRIVE parameter adds warmth/saturation
- [ ] TONE parameter sweeps LP (left) to HP (right)
- [ ] Center position (TONE=0) is full-range bypass
- [ ] Filter type changes don't create burst artifacts

**Duration:** 15 min

### Phase 4.4: MOD_MODE Routing

**Goal:** Implement wet-only vs wet+dry modulation routing
**Components:**
- Signal path branching based on MOD_MODE toggle
- WET ONLY (0): Modulation after reverb, before mixer
- WET+DRY (1): Modulation before reverb (affects both paths)
- Smooth transitions when toggling

**Test Criteria:**
- [ ] WET ONLY mode modulates only reverb tail
- [ ] WET+DRY mode modulates entire signal
- [ ] Toggle switching doesn't create discontinuities
- [ ] Dry signal routing correct in both modes

**Duration:** 10 min

## Stage 5: GUI Phases

### Phase 5.1: Layout and Basic Controls

**Goal:** Integrate v6 mockup HTML and bind all 7 parameters
**Components:**
- Copy v6-ui.html to Source/ui/public/index.html
- Update PluginEditor.h/cpp with WebView setup
- Configure CMakeLists.txt for WebView resources
- Bind all 7 parameters via relay system

**Test Criteria:**
- [ ] WebView window opens with correct size (600×640px)
- [ ] All 6 knobs visible and styled correctly
- [ ] MOD_MODE toggle renders below Tone knob
- [ ] VU meter visible at top
- [ ] Dark gradient background and brass accents render

**Duration:** 15 min

### Phase 5.2: Parameter Binding and Interaction

**Goal:** Two-way parameter communication (UI ↔ DSP)
**Components:**
- JavaScript → C++ relay calls (knob/toggle changes)
- C++ → JavaScript parameter updates (host automation)
- Value formatting and display
- Real-time parameter updates during playback

**Test Criteria:**
- [ ] Knob movements change DSP parameters
- [ ] Toggle switch changes MOD_MODE parameter
- [ ] Host automation updates UI controls
- [ ] Preset changes update all UI elements
- [ ] Parameter values display correctly

**Duration:** 10 min

### Phase 5.3: VU Meter Implementation

**Goal:** Real-time output level visualization
**Components:**
- Peak level calculation in processBlock
- C++ → JavaScript level updates (60fps throttled)
- VU meter needle animation
- Decay/ballistics for smooth movement

**Test Criteria:**
- [ ] VU meter responds to output level
- [ ] Needle movement is smooth (not jittery)
- [ ] Peak levels trigger appropriately
- [ ] Meter doesn't cause CPU spikes

**Duration:** 10 min

## Estimated Duration

Total: ~95 minutes (~1.5 hours)

- Stage 2: 5 min (Foundation - project structure)
- Stage 3: 5 min (Shell - APVTS parameters)
- Stage 4: 60 min (DSP - 4 phases)
  - Phase 4.1: 15 min (Reverb)
  - Phase 4.2: 20 min (Modulation)
  - Phase 4.3: 15 min (Saturation/Filter)
  - Phase 4.4: 10 min (MOD_MODE routing)
- Stage 5: 35 min (GUI - 3 phases)
  - Phase 5.1: 15 min (Layout)
  - Phase 5.2: 10 min (Binding)
  - Phase 5.3: 10 min (VU meter)
- Stage 6: 20 min (Validation - presets, pluginval, changelog)

## Implementation Notes

**Thread Safety:**
- All parameter reads use atomic getRawParameterValue()->load()
- Filter coefficient updates in audio thread (no allocations)
- LFO phase state is per-channel (no shared state)

**Performance:**
- Reverb: ~30% CPU (most expensive)
- Delay line interpolation: ~10% CPU
- Total estimated: ~50% single core at 48kHz

**Latency:**
- Base delay: 50ms (2400 samples at 48kHz)
- Reverb: 20-50ms internal latency
- Report via getLatencySamples() for host compensation

**Denormal Protection:**
- Use juce::ScopedNoDenormals in processBlock()
- All JUCE DSP components handle denormals internally
