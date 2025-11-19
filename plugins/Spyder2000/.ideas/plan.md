# Spyder2000 - Implementation Plan

**Date:** 2025-11-17
**Complexity Score:** 5.0 (Complex)
**Strategy:** Phase-based implementation

---

## Complexity Factors

- **Parameters:** 3 parameters (3/5 = 0.6 points, capped at 2.0) = 0.6
- **Algorithms:** 5 DSP components = 5.0
  - Adaptive Pre-Clipping Filter System (parallel treble shelf + midrange peak + crossfading)
  - Nonlinear Distortion Stage (tanh waveshaper + oversampling)
  - Tilt Filter (two biquad shelves with opposite gains)
  - DC Blocking Filter (first-order highpass)
  - Output Level Control (simple gain)
- **Features:** 0 points
  - No feedback loops
  - No FFT/frequency domain processing
  - No multiband processing
  - No modulation systems
  - No external MIDI control
- **Total:** 0.6 + 5.0 + 0 = 5.6 (capped at 5.0)

**Justification for high complexity:**
Despite only 3 parameters, the DSP implementation is sophisticated:
- Oversampling infrastructure (CPU-intensive, latency compensation required)
- Parallel filter topology with gain-dependent crossfading (requires tuning)
- Multiple IIR filters requiring careful coefficient management
- Waveshaping with anti-aliasing requirements
- Signal chain ordering is critical (pre-filter → waveshaper → post-filter)

---

## Stages

- Stage 0: Research ✓
- Stage 1: Foundation + Shell ← Next
- Stage 2: DSP (phased: 3 phases)
  - Phase 2.1: Core Processing
  - Phase 2.2: Filtering System
  - Phase 2.3: Oversampling Integration
- Stage 3: GUI (single-pass: 3 knobs only)
- Stage 3: Validation (automatic)

---

## Complex Implementation (Score = 5.0)

### Stage 2: DSP Phases

#### Phase 2.1: Core Processing

**Goal:** Implement basic signal path with waveshaping and output control

**Components:**
- Output level control (simple gain multiplication)
  - Parameter: level (0-10 → -60dB to 0dB)
  - Formula: `outputGain = (levelValue < 0.01) ? 0.0 : pow(10.0, (levelValue - 10.0) * 6.0 / 20.0)`
  - Parameter smoothing to avoid clicks
- DC blocking filter (first-order IIR highpass @ 20Hz)
  - JUCE class: `juce::dsp::IIR::Filter<float>` with `makeHighPass()`
  - Always active (no bypass)
  - Purpose: Remove DC offset from waveshaper
- Basic waveshaper (tanh, no oversampling yet)
  - Transfer function: `output = tanh(driveGain * input)`
  - Drive gain mapping: gain parameter (0-10) → 3dB to 40dB
  - Formula: `driveGain = pow(10.0, (3.0 + (gainValue * 3.7)) / 20.0)`
- Basic signal flow: Input → Waveshaper → DC Blocking → Output Level → Output

**Test Criteria:**
- [ ] Plugin loads in DAW without crashes
- [ ] Audio passes through (dry signal audible at level=10, gain=0)
- [ ] gain parameter produces audible distortion (increase from 0 to 10)
- [ ] tone parameter does nothing yet (not implemented, ignored)
- [ ] level parameter controls output volume correctly (0=mute, 10=unity)
- [ ] No clicks when changing level parameter
- [ ] No DC offset in output (verified with spectrum analyzer)
- [ ] processBlock() runs without crashes
- [ ] Parameter smoothing prevents clicks

**Implementation notes:**
- Use `juce::ScopedNoDenormals` in processBlock()
- Calculate coefficients in prepareToPlay() and when parameters change
- Use `juce::dsp::ProcessSpec` for filter initialization
- Use `juce::dsp::AudioBlock` and `ProcessContextReplacing` for processing

---

#### Phase 2.2: Filtering System

**Goal:** Add pre-clipping adaptive filter and post-distortion tilt filter

**Components:**
- Tilt filter (two biquad shelves @ 680Hz)
  - Low shelf: 680Hz, Q=0.707, gain = (5.0 - toneValue) * 2.4 dB
  - High shelf: 680Hz, Q=0.707, gain = (toneValue - 5.0) * 2.4 dB
  - JUCE classes: Two `juce::dsp::IIR::Filter<float>` instances
  - Coefficients: `makeLowShelf()` and `makeHighShelf()`
- Treble shelf filter (pre-clipping)
  - Frequency: 2000Hz, Q=0.707
  - Gain: +6dB at gain=0, smooth transition to 0dB at mid-gain
  - JUCE class: `juce::dsp::IIR::Filter<float>` with `makeHighShelf()`
- Midrange peak filter (pre-clipping)
  - Center frequency: 800Hz, Q=1.5
  - Gain: +9dB at gain=10, smooth transition to 0dB at low gain
  - JUCE class: `juce::dsp::IIR::Filter<float>` with `makePeakFilter()`
- Crossfading logic (gain-dependent mixing)
  - shelfWeight = exp(-gainValue * 0.8)
  - peakWeight = 1.0 - exp(-(gainValue - 4.0) * 0.8)
  - output = (shelfOutput * shelfWeight) + (peakOutput * peakWeight)
- Updated signal flow: Input → Adaptive Pre-Clipping Filter → Waveshaper → Tilt Filter → DC Blocking → Output Level → Output

**Test Criteria:**
- [ ] tone parameter now affects output (darker at 0, brighter at 10)
- [ ] Tilt filter pivot at 680Hz (verify with spectrum analyzer: lows/highs move inversely)
- [ ] gain parameter affects both pre-clipping filter AND distortion amount
- [ ] Low gain (0-4): Treble boost audible (brighter distortion character)
- [ ] High gain (6-10): Midrange boost audible (focused, aggressive distortion)
- [ ] Mid gain (4-6): Smooth transition between filter characters
- [ ] Crossfading is smooth (no abrupt tonal changes when sweeping gain)
- [ ] All filters recalculate coefficients when parameters change
- [ ] No artifacts or discontinuities when changing tone or gain

**Implementation notes:**
- Calculate filter coefficients once per block (not per-sample) for efficiency
- Update coefficients when gain or tone parameters change
- Use separate coefficient calculation functions for clarity
- Test filter frequencies with spectrum analyzer (verify 680Hz, 800Hz, 2000Hz)
- Tune crossfading curve if transition feels too abrupt or too slow

---

#### Phase 2.3: Oversampling Integration

**Goal:** Add oversampling around waveshaper to reduce aliasing artifacts

**Components:**
- juce::dsp::Oversampling<float> system
  - Configuration: 8x oversampling, polyphase IIR filters
  - Initialization: `oversampler.initProcessing(maximumBlockSize)`
  - Processing: `processSamplesUp()` → waveshaping → `processSamplesDown()`
- Latency compensation
  - Get latency: `oversampler.getLatencyInSamples()`
  - Report to host: `setLatencySamples(latency)` in prepareToPlay()
- Updated signal flow: Input → Adaptive Pre-Clipping Filter → [Upsample 8x] → Waveshaper @ 8x rate → [Downsample with anti-aliasing] → Tilt Filter → DC Blocking → Output Level → Output

**Test Criteria:**
- [ ] Oversampling reduces high-frequency aliasing (verify with spectrum analyzer)
- [ ] Compare aliasing at high gain (10) with/without oversampling (temporary bypass for testing)
- [ ] Latency reported correctly via getLatencySamples()
- [ ] DAW compensates latency (test with click track, verify alignment)
- [ ] CPU usage acceptable (~50% single core @ 48kHz, 512 sample buffer)
- [ ] No crashes or audio glitches with oversampling active
- [ ] processBlock() handles variable buffer sizes correctly
- [ ] Oversampling resets properly in prepareToPlay()

**Implementation notes:**
- Profile CPU usage (target < 60% single core)
- If CPU exceeds budget, consider implementing quality modes (2x/4x/8x selectable)
- Oversampling wraps ONLY the waveshaper (not filters - they're sample-rate independent)
- Use `juce::dsp::ProcessSpec` with original sample rate for filters, 8x rate for waveshaper
- Test with different buffer sizes (64, 256, 512, 1024, 2048 samples)
- Document latency value in manual (~10-20 samples @ 48kHz)

---

### Stage 3: GUI (Single-Pass)

**Goal:** Integrate UI mockup (when created) with 3-knob layout

**Why single-pass:** Only 3 knobs, no advanced UI elements (no VU meters, waveform displays, or animations). Standard WebView parameter binding is sufficient.

**Components:**
- Copy v[N]-ui.html to Source/ui/public/index.html (when mockup finalized)
- Update PluginEditor.h/cpp with WebView setup
- Configure CMakeLists.txt for WebView resources
- Bind 3 parameters via relay system:
  - gain (rotary knob, 0-10)
  - tone (rotary knob, 0-10)
  - level (rotary knob, 0-10)

**Test Criteria:**
- [ ] WebView window opens with correct size
- [ ] All 3 knobs visible and styled correctly
- [ ] Knob rotation matches parameter values (0° at min, 270° at max typical)
- [ ] Knobs respond to mouse drag (relative drag, not absolute positioning)
- [ ] Host automation updates knob positions
- [ ] Preset changes update all knob positions
- [ ] Parameter values display correctly (formatted text: "Gain: 5.0", etc.)
- [ ] No lag or visual glitches during parameter changes

**Implementation notes:**
- Use `std::unique_ptr` for WebView members (see juce8-critical-patterns.md Pattern 11)
- Use 3-parameter constructor for WebSliderParameterAttachment (see Pattern 12)
- Use ES6 module imports for getSliderState (see Pattern 21)
- Implement relative drag for knobs (see Pattern 16)
- Use explicit URL mapping in getResource() (see Pattern 8)
- Set NEEDS_WEB_BROWSER TRUE in CMakeLists.txt (see Pattern 9)

---

### Implementation Flow

- **Stage 0:** Research & Planning ✓ (this document)
- **Stage 1:** Foundation + Shell
  - CMakeLists.txt generation
  - Project structure (Source/, PluginProcessor, PluginEditor)
  - APVTS setup with 3 parameters (gain, tone, level)
  - BusesProperties: Stereo input + stereo output (audio effect)
- **Stage 2:** DSP (3 phases)
  - Phase 2.1: Core Processing (waveshaper + output level + DC blocking)
  - Phase 2.2: Filtering System (pre-clipping adaptive filter + tilt filter)
  - Phase 2.3: Oversampling Integration (8x oversampling + latency compensation)
- **Stage 3:** GUI (single-pass)
  - WebView integration with 3 knobs
  - Parameter binding
  - Testing and polish
- **Stage 3:** Validation (automatic)
  - Presets creation
  - pluginval tests
  - Changelog generation

**Total estimated time:** ~4-5 hours
- Stage 1: 30 minutes (foundation + shell)
- Stage 2: 2-3 hours (DSP in 3 phases)
- Stage 3: 45 minutes (GUI single-pass)
- Validation: 30 minutes (automatic + manual testing)

---

## Implementation Notes

### Thread Safety

- All parameter reads use atomic `APVTS::getRawParameterValue()->load()`
- Filter coefficient updates calculated in audio thread (no allocations)
- Crossfading gains calculated once per block (not per-sample)
- Oversampling state is per-channel (no shared state between left/right)
- Parameter smoothing handled by JUCE APVTS (atomic writes from message thread, smooth reads in audio thread)

**CRITICAL:** No locks in processBlock(), no file I/O, no allocations.

### Performance

**Estimated CPU usage (per component @ 48kHz, 512 sample buffer):**
- Oversampling (8x): ~35% CPU (most expensive: 3 stages of filtering + 8x processing rate)
- Waveshaping (tanh): ~8% CPU (transcendental function per-sample at 8x rate)
- IIR Filters (5 biquads): ~5% CPU (treble shelf, midrange peak, tilt low/high, DC block)
- Crossfading/Gain: ~2% CPU (simple multiplications)
- **Total estimated: ~50% single core** (acceptable for professional guitar effect)

**Hot paths:**
- Waveshaper loop within oversampled domain (process at 8x rate = most samples)
- Parallel filter processing (treble shelf + midrange peak both process full block)

**Optimization opportunities:**
- SIMD vectorization for waveshaper loop (if profiling shows bottleneck)
- Quality mode selection (2x/4x/8x oversampling user-selectable)
- Coefficient update throttling (only recalculate when parameters change significantly)

**Performance targets:**
- < 60% single core @ 48kHz (allows headroom for DAW and other plugins)
- < 50ms latency total (including oversampling)
- No audio dropouts or glitches at 64-sample buffer size

### Latency

- **Oversampling latency:** ~10-20 samples @ 48kHz (exact value from `oversampler.getLatencyInSamples()`)
  - At 48kHz: 10 samples = 0.21ms, 20 samples = 0.42ms (imperceptible)
- **IIR filters:** Negligible latency (phase shift, not group delay)
- **Total reported latency:** Oversampling latency only
- **Host compensation:** Report via `setLatencySamples()` in prepareToPlay()
- **DAW handling:** Modern DAWs compensate automatically (delay compensation feature)

**Testing:** Verify latency compensation with click track (input click aligns with output after compensation)

### Denormal Protection

- Use `juce::ScopedNoDenormals` in processBlock() (JUCE macro handles FTZ/DAZ flags)
- All JUCE DSP components handle denormals internally (IIR::Filter, Oversampling, etc.)
- Waveshaper tanh output never reaches true zero (asymptotic behavior, no denormal risk)
- Filter state variables protected by JUCE IIR implementation (flush-to-zero in coefficients)

**No custom denormal handling needed** (JUCE handles it).

### Known Challenges

**1. Crossfading curve tuning:**
- Challenge: Exponential curve may feel too fast/slow during gain sweep
- Solution: Make curve constant parameterizable during development (`exp(-gainValue * k)` where `k` is tunable)
- Reference: Professional overdrive pedals often use exponential taper for gain pots
- Test: Subjective listening with slow gain sweeps (0 → 10 over 5 seconds)

**2. Filter frequency selection:**
- Challenge: 800Hz (peak) and 2000Hz (shelf) are starting points, may need tuning
- Solution: A/B test with ±200Hz variations (e.g., 600Hz vs 800Hz vs 1000Hz for peak)
- Reference: Tube Screamer uses 720Hz peak, Klon Centaur uses ~1.5kHz boost
- Test: Spectrum analyzer + subjective listening with guitar input

**3. Oversampling CPU cost:**
- Challenge: 8x oversampling may exceed CPU budget on older hardware
- Solution: Implement quality mode selection if profiling shows >60% usage
- Reference: Professional plugins (FabFilter Saturn) offer quality modes (Draft/Good/Best)
- Test: Profile on multiple systems (M1 Mac, Intel Mac, Windows)

**4. Line 6 patent formula:**
- Challenge: Exact Line 6 patent transfer function unknown
- Solution: Use tanh as industry-standard approximation, document that this is "inspired by" not "exact clone"
- Reference: tanh is used by FabFilter Saturn, Soundtoys Decapitator, UAD tube emulations
- Test: Subjective listening vs POD recordings (if available)

**5. Parameter smoothing for level:**
- Challenge: Level parameter controls output volume, abrupt changes cause clicks
- Solution: Use JUCE APVTS smoothing (5-10ms ramp typical)
- Reference: GainKnob example shows parameter smoothing usage
- Test: Automate level parameter with fast changes (verify no clicks)

---

## References

**Contract files:**
- Creative brief: `plugins/Spyder2000/.ideas/creative-brief.md`
- Parameter spec: `plugins/Spyder2000/.ideas/parameter-spec-draft.md`
- DSP architecture: `plugins/Spyder2000/.ideas/architecture.md`
- UI mockup: `plugins/Spyder2000/.ideas/mockups/` (not yet created)

**Similar plugins for reference:**
- **GainKnob:** Parameter smoothing, WebView knob binding, basic DSP pipeline
- **TapeAge:** IIR filtering, waveshaping, parameter-to-DSP mapping
- **FlutterVerb:** juce::dsp::Reverb usage (ProcessSpec, AudioBlock, ProcessContextReplacing pattern)
- **LushPad:** WebView integration, ES6 module imports, 3-parameter WebSliderParameterAttachment

**JUCE critical patterns:**
- Read `troubleshooting/patterns/juce8-critical-patterns.md` BEFORE implementation
- Pay special attention to:
  - Pattern 17: juce::dsp API (ProcessSpec, AudioBlock, ProcessContextReplacing)
  - Pattern 11: WebView member initialization (std::unique_ptr)
  - Pattern 12: WebSliderParameterAttachment 3-parameter constructor
  - Pattern 21: ES6 module loading (type="module" required)

**Research references:**
- DSP literature: Music-DSP mailing list, David Yeh thesis (Stanford CCRMA)
- Professional plugins: FabFilter Saturn, Soundtoys Decapitator, iZotope Trash 2
- JUCE documentation: juce::dsp::Oversampling, juce::dsp::IIR::Filter, juce::dsp::WaveShaper
