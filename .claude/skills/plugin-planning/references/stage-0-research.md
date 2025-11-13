# Stage 0: Research

**Context:** This file is part of the plugin-planning skill.
**Invoked by:** Main orchestrator in `SKILL.md` when starting planning workflow
**Purpose:** Understand what we're building before writing code through research and DSP architecture specification

---

**Goal:** Create comprehensive DSP architecture specification (architecture.md)

**Duration:** 5-30 minutes (graduated by complexity - Tier 1: 5 min, Tier 6: 30 min)

**Model Configuration:**
- Extended thinking: ENABLED
- Budget: 10000 tokens

---

## Prerequisites

1. Check for creative brief:
```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" ]; then
    echo "✗ creative-brief.md not found"
    echo "Run /dream ${PLUGIN_NAME} first"
    exit 1
fi
```

2. Check plugin status (must not be past Stage 1):
```bash
STATUS=$(grep -A 2 "^### ${PLUGIN_NAME}$" PLUGINS.md | grep "Status:" | awk '{print $2}')
if [[ "$STATUS" =~ Stage\ [2-6] ]]; then
    echo "✗ Plugin already past planning stage"
    echo "Use /continue or /improve instead"
    exit 1
fi
```

---

## Research Process

### 1. Read Creative Brief

```bash
cat plugins/${PLUGIN_NAME}/.ideas/creative-brief.md
```

Extract key information:
- Plugin type (effect, instrument, utility)
- Core audio functionality
- Target use case
- Key features
- Sonic character

### 2. Identify Technical Approach

Determine:
- **Input/Output:** Mono, stereo, sidechain, multi-channel?
- **Processing Domain:** Time-domain, frequency-domain (FFT), granular, sample-based?
- **Real-time Requirements:** Low latency critical? Lookahead acceptable?
- **State Management:** Stateless or stateful processing?

### 3. Deep Architecture Research (Graduated Complexity)

This step implements per-feature research with depth based on plugin complexity.

#### 3.0: Complexity Detection

**Tool:** Extended thinking

**Analyze creative brief to detect complexity tier:**

| Tier | Indicators | Research Depth | Time |
|------|-----------|---------------|------|
| 1 | 1-3 parameters, simple DSP (gain, pan, basic filter) | QUICK | 5 min |
| 2 | 4-7 parameters, standard DSP (reverb, delay, saturation) | QUICK | 10 min |
| 3 | Complex DSP algorithms (shimmer = pitch shift + reverb) | MODERATE | 15 min |
| 4 | Synthesizers with MIDI input, oscillators | MODERATE | 20 min |
| 5 | File I/O, multi-output routing (>2 channels), folder scanning | DEEP | 30 min |
| 6 | Real-time analysis, visualization, FFT processing | DEEP | 30 min |

**Examples:**
- **Tier 1:** GainKnob (1 parameter, simple gain)
- **Tier 2:** TapeAge (6 parameters, standard reverb + saturation)
- **Tier 3:** LushVerb (shimmer reverb with complex pitch shifting)
- **Tier 4:** OrganicHats (MIDI-triggered synthesizer)
- **Tier 5:** DrumRoulette (file I/O, folder scanning, 18 outputs)
- **Tier 6:** SpectrumAnalyzer (FFT analysis, real-time visualization)

**Action:**
```xml
<extended_thinking>
Thoroughly analyze the creative brief to detect complexity tier based on:
- Parameter count
- DSP algorithm complexity
- Non-DSP features (file I/O, multi-output, MIDI routing)
- UI complexity (visualization, action buttons)
- State management needs

Set research depth: QUICK (Tier 1-2) | MODERATE (Tier 3-4) | DEEP (Tier 5-6)
</extended_thinking>
```

**Output:** Detected tier and assigned research depth

---

#### 3.1: Meta-Research - Feature Identification

**Tool:** Extended thinking

**BEFORE researching anything, identify what needs researching.**

Extract features from creative brief across ALL plugin systems:
- **DSP features:** reverb, saturation, filtering, pitch shifting, synthesis, compression, delay
- **Non-DSP features:** file I/O, folder scanning, multi-output routing (>2 channels), MIDI routing, randomization
- **UI features:** parameter controls, action buttons (randomize, lock), visualization, displays
- **State features:** folder paths, lock states, user preferences, preset management

**Action:**
```xml
<extended_thinking trigger="thoroughly analyze">
Thoroughly analyze the creative brief to extract ALL features across DSP, file I/O, MIDI, UI actions, and state management.

Consider:
- What does the plugin PROCESS? (DSP features)
- What does the plugin LOAD/SAVE? (file I/O, state features)
- What does the plugin ROUTE? (multi-output, MIDI routing)
- What does the plugin DISPLAY? (visualization, UI features)
- What actions can users trigger? (buttons, randomization, UI actions)

Output a numbered list of features requiring research.
</extended_thinking>
```

**Example output (LushVerb - Shimmer Reverb):**
1. Pitch shifting (+1 octave)
2. Reverb engine
3. Feedback loop
4. Dry/wet mixing
5. Parameter controls (shimmer amount, decay, mix)

**Example output (DrumRoulette):**
1. Sample playback
2. File loading (WAV/AIFF)
3. Folder scanning (recursive)
4. Randomization algorithm
5. Multi-output routing (18 outputs)
6. MIDI routing (8 slots)
7. UI-triggered actions (randomize, lock buttons)
8. State persistence (folder paths, lock states)

**Output:** Numbered list of features (typically 3-10 features depending on tier)

---

#### 3.2: Per-Feature Deep Research (ITERATE)

**FOR EACH feature identified in Step 3.1, execute steps 3.2.1 through 3.2.6:**

---

##### 3.2.1: Algorithmic Understanding

**Tool:** Extended thinking (or sequential-thinking MCP if available)

**Questions to answer:**
- What is this feature conceptually?
- How is it implemented algorithmically?
- What are the mathematical/programming primitives?

**Action:**
```xml
<extended_thinking>
Analyze [FeatureName] algorithmically:
- Conceptual understanding: What is this?
- Algorithmic implementation: How is this done?
- Mathematical/programming primitives: What building blocks are needed?

Consider multiple approaches and tradeoffs.
</extended_thinking>
```

**Example (Shimmer Reverb - Pitch Shifting):**

**Conceptual:** Shimmer reverb = pitch shifting (+1 octave) + reverb + feedback loop. The pitch-shifted signal feeds back into the reverb to create cascading harmonics.

**Algorithmic approaches:**
1. **Phase vocoder (FFT-based):**
   - FFT → Frequency-domain pitch shift → IFFT
   - High quality, high CPU cost
   - Industry standard (Strymon BigSky, Valhalla Shimmer)

2. **Granular synthesis:**
   - Break audio into grains → Resample → Overlap-add
   - Lower quality than phase vocoder
   - Lower CPU cost

3. **Delay-based:**
   - Variable delay with interpolation
   - Lowest quality, only works for small shifts
   - Very low CPU cost

**Primitives needed:**
- FFT engine (for phase vocoder approach)
- Reverb engine
- Feedback mixer
- Dry/wet mixer

**Sources:** DSP textbooks (DAFX), audio algorithm documentation

**Output:** Plain-language algorithmic explanation with approaches and primitives

---

##### 3.2.2: Professional Research

**Tool:** WebSearch (for industry plugins, NOT for JUCE documentation)

**Questions to answer:**
- How do professional plugins implement this?
- What approaches do they use?
- What are quality vs performance tradeoffs?

**Action:**
```bash
# Search for professional plugin implementations
WebSearch: "[feature name] professional audio plugins implementation"
WebSearch: "[feature name] FabFilter Waves UAD Valhalla Strymon"
```

**Search targets:**
- FabFilter (modern, clean)
- Waves (industry standard)
- UAD (hardware emulation)
- Valhalla (reverb/modulation)
- iZotope (intelligent processing)
- Soundtoys (creative effects)
- Strymon (high-end effects)

**Example (Shimmer Reverb):**

**Professional implementations found:**

1. **Strymon BigSky - Shimmer Algorithm**
   - Uses phase vocoder for pitch shifting
   - High CPU usage (studio quality)
   - Pitch shift range: +1 octave typical
   - Observation: Heavy processing, not real-time optimized

2. **Valhalla Shimmer**
   - Optimized phase vocoder algorithm
   - Lower CPU than BigSky
   - Still professional quality
   - Observation: Algorithmic optimizations reduce CPU ~30%

3. **Eventide H9 - Shimmer**
   - Hardware implementation (DSP chip)
   - Phase vocoder-based
   - Observation: Industry consensus on phase vocoder approach

**Conclusion:** Phase vocoder is industry standard for shimmer. Granular synthesis is fallback for CPU-constrained systems.

**Output:** 3-5 professional plugin examples with implementation approaches and observations

---

##### 3.2.3: Primitive Decomposition

**Tool:** Extended thinking

**Questions to answer:**
- What are the building blocks?
- What primitives are needed to implement this feature?

**Action:**
```xml
<extended_thinking>
Break [FeatureName] into primitives:
- What are the fundamental components?
- What data structures are needed?
- What algorithms/operations are required?

List each primitive with a brief description.
</extended_thinking>
```

**Example (Shimmer Reverb):**

Breaking shimmer into primitives:

1. **FFT Engine**
   - Purpose: Convert time-domain audio to frequency domain
   - Requirement: Power-of-2 FFT sizes (1024, 2048, 4096)
   - Operations: Forward FFT, inverse FFT

2. **Phase Vocoder**
   - Purpose: Pitch shift in frequency domain
   - Requirement: Phase adjustment for frequency bins
   - Operations: Bin resampling, phase correction

3. **Reverb Engine**
   - Purpose: Generate reverb effect
   - Requirement: Configurable room size, decay, damping
   - Operations: Comb filters, all-pass filters, diffusion

4. **Feedback Mixer**
   - Purpose: Route pitch-shifted reverb back to input
   - Requirement: Gain control for feedback amount
   - Operations: Buffer mixing, gain multiplication

5. **Dry/Wet Mixer**
   - Purpose: Blend processed and unprocessed signals
   - Requirement: Latency compensation for dry signal
   - Operations: Crossfade mixing

**Example (DrumRoulette - Folder Scanning):**

Breaking folder scanning into primitives:

1. **Directory Traversal**
   - Purpose: Recursively scan folder for audio files
   - Requirement: Filter by extension (.wav, .aiff, .mp3)
   - Operations: Recursive file enumeration

2. **File Validation**
   - Purpose: Verify files are valid audio
   - Requirement: Check format, sample rate, channels
   - Operations: File header parsing

3. **File List Storage**
   - Purpose: Cache discovered files for randomization
   - Requirement: Thread-safe data structure
   - Operations: Array/vector storage, concurrent access

4. **Random Selection**
   - Purpose: Pick random file from list
   - Requirement: Uniform distribution
   - Operations: Random number generation, array indexing

**Output:** List of primitives with descriptions (DSP algorithms, file operations, data structures, etc.)

---

##### 3.2.4: JUCE API Mapping

**Tool:** Context7 MCP (authoritative JUCE 8 documentation) - NOT WebSearch

**WHY Context7-MCP and NOT WebSearch:**
- WebSearch returns outdated JUCE 6 documentation
- JUCE 8 has breaking changes from JUCE 6
- Context7-MCP provides authoritative JUCE 8 API documentation
- Using wrong docs causes build failures

**For each primitive from 3.2.3:**

1. **Query Context7-MCP:** Search for primitive (e.g., "JUCE FFT", "JUCE file scanning", "JUCE multi-output bus configuration")
2. **Verify existence:** Does this class exist in JUCE 8?
3. **Document API:** Class name, module dependency, usage pattern
4. **OR document custom need:** "No JUCE class - need custom implementation: [description]"

**Action:**
```bash
# For each primitive, query Context7-MCP
Context7-MCP: "JUCE [primitive name] API JUCE 8"
Context7-MCP: "JUCE [primitive name] module dependency"
Context7-MCP: "JUCE [primitive name] usage example"
```

**Example (Shimmer Reverb primitives → JUCE API mapping):**

| Primitive | JUCE API | Module | Usage Pattern |
|-----------|----------|--------|---------------|
| FFT Engine | `juce::dsp::FFT` | juce_dsp | `FFT fft(order); fft.performRealOnlyForwardTransform(data);` |
| Phase Vocoder | No JUCE class | Custom | Need custom implementation: frequency bin resampling + phase correction |
| Reverb Engine | `juce::dsp::Reverb` | juce_dsp | `Reverb reverb; reverb.setParameters(params); reverb.process(context);` |
| Feedback Mixer | Standard audio buffer | juce_audio_basics | `buffer.addFrom(channel, 0, feedbackBuffer, channel, 0, numSamples, gain);` |
| Dry/Wet Mixer | `juce::dsp::DryWetMixer` | juce_dsp | `DryWetMixer<float> mixer; mixer.setWetMixProportion(0.5f);` |

**Example (DrumRoulette - Folder Scanning primitives → JUCE API mapping):**

| Primitive | JUCE API | Module | Usage Pattern |
|-----------|----------|--------|---------------|
| Directory Traversal | `juce::File::findChildFiles()` | juce_core | `file.findChildFiles(results, File::findFiles, true, "*.wav");` |
| File Validation | `juce::AudioFormatManager` | juce_audio_formats | `manager.createReaderFor(file);` |
| File List Storage | `std::vector<juce::File>` | Standard C++ | `std::vector<File> audioFiles;` |
| Random Selection | `juce::Random::nextInt()` | juce_core | `Random::getSystemRandom().nextInt(audioFiles.size());` |

**Output:** Table mapping each primitive to JUCE class (or "custom implementation needed")

---

##### 3.2.5: Validation

**Sub-step A: Check Critical Patterns**

**Tool:** Read (for juce8-critical-patterns.md)

**Action:**
1. Read `troubleshooting/patterns/juce8-critical-patterns.md`
2. Search for each JUCE class mentioned in 3.2.4
3. Document gotchas, module dependencies, CMake requirements

**Example:**
```bash
# Read critical patterns
cat troubleshooting/patterns/juce8-critical-patterns.md

# Search for each JUCE class
grep -i "juce::dsp::FFT" troubleshooting/patterns/juce8-critical-patterns.md
grep -i "juce::dsp::Reverb" troubleshooting/patterns/juce8-critical-patterns.md
grep -i "multi-output" troubleshooting/patterns/juce8-critical-patterns.md
grep -i "BusesProperties" troubleshooting/patterns/juce8-critical-patterns.md
```

**Example findings:**

**For `juce::dsp::FFT`:**
- Gotcha: Requires power-of-2 sizes only (1024, 2048, 4096)
- Module: Requires juce_dsp in CMakeLists.txt
- CMake: `target_link_libraries(${TARGET} PRIVATE juce::juce_dsp)`

**For multi-output routing:**
- Gotcha: BusesProperties must be configured in AudioProcessor constructor, NOT prepareToPlay
- Pattern: `BusesProperties().withInput("Input", juce::AudioChannelSet::stereo()).withOutput("Main", juce::AudioChannelSet::stereo()).withOutput("Slot1", juce::AudioChannelSet::stereo())`
- Compatibility: Not all DAWs support >2 outputs (test required)

**Output:** List of gotchas, requirements, and patterns from juce8-critical-patterns.md

---

**Sub-step B: Feasibility Assessment**

**Tool:** Extended thinking

**Questions:**
- Is this approach implementable?
- What is the complexity (LOW/MEDIUM/HIGH)?
- What are the risks?
- What are alternative approaches?
- What is the fallback if this fails?

**Action:**
```xml
<extended_thinking>
Assess feasibility of [FeatureName] implementation:
- Implementability: Can this be implemented with identified JUCE APIs?
- Complexity rating: LOW | MEDIUM | HIGH
- Risk assessment: What could go wrong?
- Alternative approaches: What other ways exist?
- Fallback architecture: If this fails, what's Plan B?
</extended_thinking>
```

**Example (Shimmer Reverb - Phase Vocoder):**

**Implementable:** Yes (JUCE has FFT + Reverb)

**Complexity:** HIGH
- Phase vocoder implementation is algorithmically complex
- Requires deep understanding of FFT, frequency domain processing, phase correction
- Not a trivial "use JUCE class" implementation

**Risk Level:** HIGH

**Risk factors:**
1. Phase vocoder is non-trivial to implement correctly
2. Requires careful phase adjustment to avoid artifacts
3. High CPU cost may cause performance issues
4. No existing JUCE phase vocoder class (custom implementation)

**Alternative approaches:**
1. **Granular synthesis:** Simpler algorithm, lower quality, lower CPU
2. **Delay-based pitch shift:** Simplest, lowest quality, only works for small shifts

**Fallback architecture:**
- If phase vocoder proves too complex → Use granular synthesis
- If granular fails → Use simple delay-based pitch shift + reverb
- Reduced-scope version: Standard reverb with modulation (no pitch shift)

**Mitigation strategy:**
- Start with simpler delay-based approach to validate concept
- Research open-source phase vocoder implementations for reference
- Consider using third-party DSP library (e.g., Rubber Band Library)

**Example (DrumRoulette - Multi-Output Routing):**

**Implementable:** Yes (JUCE supports multi-output via BusesProperties)

**Complexity:** MEDIUM
- BusesProperties configuration is straightforward
- Output routing logic is standard buffer management
- DAW compatibility testing required

**Risk Level:** MEDIUM

**Risk factors:**
1. Not all DAWs support >2 outputs
2. Bus configuration must be done in constructor (common mistake)
3. Buffer management for 18 outputs requires careful indexing

**Alternative approaches:**
1. **Main stereo output only:** Simplest, loses per-slot routing
2. **User-selectable output count:** 2/8/18 outputs based on user preference

**Fallback architecture:**
- If 18 outputs fail DAW compatibility → Reduce to main stereo + 4 stereo pairs (10 outputs)
- If multi-output proves problematic → Main stereo output only with internal mixing

**Mitigation strategy:**
- Check critical patterns for BusesProperties gotchas
- Test with multiple DAWs (Logic Pro, Ableton, FL Studio)
- Document DAW compatibility in user manual

**Output:** Feasibility rating (LOW/MEDIUM/HIGH complexity and risk) with alternatives and fallbacks

---

##### 3.2.6: Documentation

**Action:** Write findings to architecture.md for this feature.

**Required content:**
- Algorithmic explanation (from 3.2.1)
- JUCE class mappings (from 3.2.4)
- Risks and complexity rating (from 3.2.5)
- Alternative approaches (from 3.2.5)
- Implementation notes (gotchas from critical patterns)

**Template for each feature in architecture.md:**

```markdown
### [Feature Name]

**Algorithm:** [Algorithmic explanation from 3.2.1]

**JUCE Classes:**
- [Primitive 1]: `juce::ClassName` (module: juce_module)
- [Primitive 2]: Custom implementation (description)

**Complexity:** [LOW | MEDIUM | HIGH]

**Risk Level:** [LOW | MEDIUM | HIGH]

**Implementation Notes:**
- [Gotcha 1 from critical patterns]
- [Gotcha 2 from critical patterns]
- [Module dependency requirements]

**Alternatives:**
1. [Alternative 1]: [Description and tradeoffs]
2. [Alternative 2]: [Description and tradeoffs]

**Fallback:** [Reduced-scope version if primary approach fails]
```

**WHY document per-feature:** Prevents information loss during iteration. Each feature gets fully documented before moving to next feature.

---

#### 3.3: Integration Analysis

**Tool:** Extended thinking

**After all features researched, analyze how they integrate.**

**Questions:**
- How do these features integrate?
- Are there dependencies between features?
- Are there parameter interactions? (e.g., filter cutoff affects reverb tone)
- Are there processing order requirements? (e.g., saturation before reverb)
- Are there thread boundaries? (file I/O on background thread, DSP on audio thread)

**Action:**
```xml
<extended_thinking>
Analyze integration of all features identified in 3.1:

For each pair of features:
- Do they depend on each other?
- Does processing order matter?
- Do parameters interact?
- Are there thread boundaries?

Output:
- Feature dependency diagram
- Processing chain with order requirements
- Parameter interaction notes
- Thread boundary documentation
</extended_thinking>
```

**Example (Shimmer Reverb):**

**Feature dependencies:**
- Pitch shifting → Feedback mixer: Pitch-shifted signal feeds back
- Feedback mixer → Reverb engine: Feedback must enter reverb input
- Reverb engine → Dry/wet mixer: Reverb output blends with dry signal

**Processing order requirements:**
1. Split dry signal (capture for later mixing)
2. Pitch shift input signal (+1 octave)
3. Mix pitch-shifted signal with feedback buffer
4. Process through reverb engine
5. Apply feedback gain and route to feedback buffer
6. Blend reverb output with dry signal via dry/wet mixer
7. Output final mixed signal

**Parameter interactions:**
- Shimmer amount (feedback gain) affects reverb decay time
- Reverb decay affects feedback loop stability (high decay + high feedback = runaway)
- Dry/wet mix affects perceived shimmer intensity

**Thread boundaries:**
- All DSP processing on audio thread (real-time)
- Parameter updates on message thread (via APVTS atomic reads)
- No file I/O or background processing needed

**Example (DrumRoulette):**

**Feature dependencies:**
- Folder scanning → File loading: Must scan before loading
- File loading → Sample playback: Must load before playing
- Randomization → Folder scanning: Needs file list from scan
- Lock buttons → Randomization: Locked slots skip randomization
- MIDI routing → Sample playback: MIDI note triggers sample
- Multi-output routing → Sample playback: Each slot routes to specific output

**Processing order requirements:**
1. MIDI input received
2. Note mapped to slot (0-7)
3. Check if slot has loaded sample
4. Trigger sample playback on appropriate output bus
5. Apply ADSR envelope
6. Route to correct output (slot-specific or main)

**Parameter interactions:**
- Attack/decay/sustain/release affect all slots equally
- Volume per-slot (not per-parameter, per-loaded-sample)
- Lock state doesn't have APVTS parameter (UI-only state)

**Thread boundaries:**
- Audio thread: Sample playback, MIDI processing, envelope, output routing
- Message thread: UI button clicks (randomize, lock), folder path changes
- Background thread: Folder scanning, file loading (NOT on audio thread!)
- Communication: Lock-free queues for file list updates, atomic flags for lock states

**Output:** Processing chain diagram with integration notes, dependencies, interactions, and thread boundaries documented in architecture.md

---

#### 3.4: Comprehensive Documentation

**Use enhanced architecture-template.md to compile all research.**

**File location:** `plugins/${PLUGIN_NAME}/.ideas/architecture.md`

**Required sections (see template for detailed structure):**

1. **Header** - Contract status, generation info, purpose
2. **Core Components** - DSP components with JUCE classes and configuration (preserve existing)
3. **Processing Chain** - Signal flow diagram (preserve existing)
4. **Parameter Mapping** - Table of all parameters (preserve existing)
5. **Algorithm Details** - Implementation approach for each component (preserve existing)
6. **System Architecture (NEW)** - Non-DSP systems (file I/O, multi-output, MIDI, state persistence)
7. **Integration Points (NEW)** - Feature dependencies, parameter interactions, processing order, thread boundaries
8. **Implementation Risks (NEW)** - Per-feature risk assessment with fallbacks, overall project risk
9. **Architecture Decisions (NEW)** - WHY this approach, alternatives considered, tradeoffs accepted
10. **Special Considerations** - Thread safety, performance, denormal protection, sample rate, latency (preserve existing)
11. **Research References** - Professional plugins, JUCE docs, technical resources (preserve existing)

**Action:**
1. Load template: `assets/architecture-template.md`
2. Fill all sections with research findings from 3.2.1-3.3
3. Ensure all features from 3.1 are documented
4. Include examples and concrete details (not vague placeholders)
5. Write file: `plugins/${PLUGIN_NAME}/.ideas/architecture.md`

**Quality check:**
- Every feature from 3.1 has a section in architecture.md
- Every JUCE class has module dependency documented
- Every HIGH risk has a fallback architecture
- Integration analysis covers all feature interactions
- Processing chain shows complete signal flow

**Output:** Comprehensive architecture.md that guides ALL plugin systems (DSP, file I/O, multi-output, MIDI, UI actions, state persistence)

---

**Duration tracking:**
- QUICK depth (Tier 1-2): Steps 3.0-3.2 minimal, skip most professional research, basic validation → 5-10 minutes
- MODERATE depth (Tier 3-4): Steps 3.0-3.2 moderate depth, professional research for key features → 15-20 minutes
- DEEP depth (Tier 5-6): Steps 3.0-3.2 full depth, comprehensive research per feature, full validation → 30 minutes

### 4. Research Parameter Ranges

**Note:** Professional plugin research is now integrated into Step 3.2.2 (per-feature research). This step focuses on parameter-specific ranges and defaults.

For each parameter type in the creative brief:

**Gain/Volume:**
- Range: -60dB to +20dB typical
- Default: 0dB (unity)
- Skew: Linear dB or exponential amplitude

**Filter Cutoff:**
- Range: 20Hz to 20kHz
- Default: 1kHz (center) or off
- Skew: Exponential (log frequency scale)

**Time-based (Delay, Reverb):**
- Range: 0ms to 5000ms (delay), 0.1s to 20s (reverb decay)
- Default: Context-dependent
- Skew: Linear or exponential depending on range

**Modulation Rate:**
- Range: 0.01Hz to 20Hz
- Default: 1Hz (slow) or 5Hz (fast)
- Skew: Exponential (wide range)

**Mix/Blend:**
- Range: 0% to 100%
- Default: 50% or context-dependent
- Skew: Linear

**Reference existing plugins in system:**
```bash
# Find similar parameter ranges in existing plugins
grep -r "addParameter" plugins/*/Source/*.cpp | grep -i "[parameter-type]"
```

### 5. Design Sync Check (If Mockup Exists)

Check for existing UI mockup:
```bash
ls -la plugins/${PLUGIN_NAME}/.ideas/mockups/v*-ui.yaml 2>/dev/null
```

**If mockup exists:**

1. Read mockup file to extract parameters
2. Read creative brief to extract expected parameters
3. Compare parameter lists

**If conflicts found:**
- Parameter in mockup but not in brief
- Parameter in brief but not in mockup
- Different parameter types or ranges

**Document conflicts:**
Document identified conflicts in architecture.md for resolution during mockup finalization.

Note: Conflicts will be auto-resolved when mockup is finalized (mockup becomes source of truth).

**Document findings:**
```markdown
## Design Sync Results

**Status:** [✓ Synchronized | ⚠ Conflicts resolved | ✗ Manual review needed]

**Changes made:**
- [List any parameter additions/removals]
- [List any range adjustments]
- [List any type changes]

**Contracts updated:**
- [creative-brief.md | parameter-spec.md | both]
```

---

## Create DSP Architecture Document

**Use template:** `assets/architecture-template.md`

**File location:** `plugins/${PLUGIN_NAME}/.ideas/architecture.md`

### Required Sections

#### 1. Header
```markdown
# DSP Architecture: [PluginName]

**CRITICAL CONTRACT:** This specification is immutable during Stages 2-5 implementation.

**Generated by:** Stage 0 Research
**Referenced by:** Stage 1 (Planning), Stage 4 (DSP Implementation)
**Purpose:** DSP specification defining processing components, signal flow, and JUCE module usage
```

#### 2. Core Components

For each DSP component identified:

```markdown
### [Component Name]
- **JUCE Class:** `juce::dsp::ClassName` or "Custom implementation (description)"
- **Purpose:** [What this component does]
- **Parameters Affected:** [List parameter IDs]
- **Configuration:**
  - [Initialization settings]
  - [Parameter mappings and ranges]
  - [Special handling notes]
```

**Mapping research to components:**
- JUCE classes identified → List each as a component
- Custom algorithms needed → Describe implementation approach
- Parameter research → Document ranges in Configuration section
- Professional plugins → Reference in Research References section

#### 3. Processing Chain

Create ASCII diagram showing:
- Signal flow from input to output
- Where each component fits
- Parameter control points
- Parallel paths or feedback loops

```
Example:
Input
  ↓
Dry/Wet Mixer (capture) ← MIX
  ↓
[Main Processing Chain]
  ↓
Output Gain ← VOLUME
  ↓
Dry/Wet Mixer (blend) ← MIX
  ↓
Output
```

#### 4. Parameter Mapping

Create table mapping every parameter to DSP components:

| Parameter ID | Type | Range | DSP Component | Usage |
|-------------|------|-------|---------------|-------|
| ... | ... | ... | ... | ... |

**Source:** Extract from creative brief + parameter-spec.md (if exists)

#### 5. Algorithm Details

For each component, describe implementation:
- Mathematical formulas
- Coefficient calculations
- Interpolation methods
- Edge case handling

**Use research findings:**
- JUCE documentation for built-in components
- Professional plugin behavior for custom algorithms
- Technical resources for DSP theory

#### 6. Special Considerations

Document:

**Thread Safety:**
- How parameters are accessed (atomic? locks?)
- Buffer ownership
- State updates

**Performance:**
- Estimated CPU per component
- Hot paths
- Optimization opportunities

**Denormal Protection:**
- Strategy (ScopedNoDenormals, flush-to-zero, etc.)
- Which components need it

**Sample Rate Handling:**
- Sample-rate-dependent calculations
- prepareToPlay requirements
- Coefficient updates on rate change

**Latency:**
- Sources of latency (delays, lookahead, etc.)
- Total latency calculation
- Host compensation via getLatencySamples()

#### 7. Research References

Document all research:

**Professional Plugins:**
- List each plugin researched
- Key observations
- Parameter ranges noted

**JUCE Documentation:**
- Classes researched
- Key findings
- Usage patterns

**Technical Resources:**
- Books, papers, tutorials
- Algorithms studied
- Reference implementations

---

## State Management

### 1. Create Handoff File

**File:** `plugins/${PLUGIN_NAME}/.continue-here.md`

**Content:**
```yaml
---
plugin: [PluginName]
stage: 0
status: complete
last_updated: [YYYY-MM-DD HH:MM:SS]
---

# Resume Point

## Current State: Stage 0 - Research Complete

DSP architecture documented. Ready to proceed to planning.

## Completed So Far

**Stage 0:** ✓ Complete
- Plugin type defined: [Type]
- Professional examples researched: [Count]
- JUCE modules identified: [List]
- DSP feasibility verified
- Parameter ranges researched

## Next Steps

1. Stage 1: Planning (calculate complexity, create implementation plan)
2. Review architecture.md findings
3. Pause here

## Files Created
- plugins/[PluginName]/.ideas/architecture.md
```

### 2. Update PLUGINS.md (ATOMIC - both locations)

**Check if entry exists:**
```bash
if ! grep -q "^### ${PLUGIN_NAME}$" PLUGINS.md; then
    # Create new entry
    echo "Creating new PLUGINS.md entry"
else
    # Update existing entry
    echo "Updating existing PLUGINS.md entry"
fi
```

**New entry format (MUST include registry table entry):**
```markdown
1. Add to registry table (at line ~34, before first ### entry):
   | [PluginName] | 🚧 Stage 0 | - | [YYYY-MM-DD] |

2. Add full entry section:
   ### [PluginName]

   **Status:** 🚧 Stage 0
   **Type:** [Audio Effect | MIDI Instrument | Synth | Utility]
   **Created:** [YYYY-MM-DD]

   [Brief description from creative-brief.md]

   **Lifecycle Timeline:**
   - **[YYYY-MM-DD]:** Creative brief created
   - **[YYYY-MM-DD] (Stage 0):** Research completed - DSP architecture documented

   **Last Updated:** [YYYY-MM-DD]
```

**Update existing entry (ATOMIC - both locations):**
```markdown
1. Update registry table:
   Find: | [PluginName] | 💡 Ideated | ...
   Replace: | [PluginName] | 🚧 Stage 0 | - | [YYYY-MM-DD] |

2. Update full entry section:
   Find: **Status:** 💡 Ideated
   Replace: **Status:** 🚧 Stage 0

3. Add timeline entry to full section:
   - **[YYYY-MM-DD] (Stage 0):** Research completed - DSP architecture documented

4. Update last updated in both locations
```

**CRITICAL:** Always update BOTH locations to prevent registry drift.

### 3. Git Commit

```bash
git add \
  plugins/${PLUGIN_NAME}/.ideas/architecture.md \
  plugins/${PLUGIN_NAME}/.continue-here.md \
  PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 0 - research complete

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

**Display commit hash:**
```bash
git log -1 --format='✓ Committed: %h - Stage 0 complete'
```

---

## Decision Menu

**Present numbered list (NOT AskUserQuestion):**

```
✓ Stage 0 complete: DSP architecture documented

What's next?
1. Continue to Stage 1 - Planning (recommended)
2. Review architecture.md findings
3. Improve creative brief based on research
4. Run deeper JUCE investigation (deep-research skill) ← Discover troubleshooting
5. Pause here
6. Other

Choose (1-6): _
```

**Handle user input:**

| Input | Action |
|-------|--------|
| 1 or "continue" | Proceed to Stage 1 |
| 2 or "review" | Display architecture.md, re-present menu |
| 3 | Open creative brief for editing, re-present menu |
| 4 | Invoke deep-research skill, return to menu |
| 5 or "pause" | Exit skill with handoff file ready |
| 6 or "other" | Ask "What would you like to do?", re-present menu |

---

**Return to:** Main plugin-planning orchestration in `SKILL.md`
