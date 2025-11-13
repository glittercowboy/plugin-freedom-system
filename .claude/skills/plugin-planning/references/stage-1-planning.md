# Stage 1: Planning

**Context:** This file is part of the plugin-planning skill.
**Invoked by:** Main orchestrator in `SKILL.md` after Stage 0 completion
**Purpose:** Analyze complexity and create implementation plan with phase breakdown if needed

---

**Goal:** Create implementation plan (plan.md) with complexity assessment

**Duration:** 2-5 minutes

**Model Configuration:**
- Extended thinking: DISABLED

---

## Prerequisites

### 1. Check Stage 0 Completion

```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" ]; then
    echo "✗ Stage 0 incomplete - architecture.md not found"
    echo "Run Stage 0 (Research) first"
    exit 1
fi
```

### 2. Check for Required Contracts (BLOCKING)

```bash
# Check for parameter specification
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" ]; then
    MISSING_PARAM_SPEC=true
fi

# Check for architecture specification
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" ]; then
    MISSING_ARCHITECTURE=true
fi
```

**If ANY contract is missing:**

STOP IMMEDIATELY and BLOCK with this message:

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ BLOCKED: Cannot proceed to Stage 1
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Missing implementation contracts:

Required contracts:
✓ creative-brief.md - exists
[✓/✗] parameter-spec.md - [exists/MISSING (required)]
[✓/✗] architecture.md - [exists/MISSING (required)]

WHY BLOCKED:
Stage 1 planning requires complete specifications to prevent implementation
drift. These contracts are the single source of truth.

HOW TO UNBLOCK:

1. parameter-spec.md: Complete ui-mockup two-phase workflow
   - Run: /dream [PluginName]
   - Choose: "Create UI mockup"
   - Design UI and finalize (Phase 4.5)
   - Finalization generates parameter-spec.md

2. architecture.md: Create DSP architecture specification
   - Run Stage 0 (Research) to generate architecture.md
   - Document DSP components and processing chain
   - Map parameters to DSP components
   - Save to plugins/[PluginName]/.ideas/architecture.md

Once both contracts exist, Stage 1 will proceed.
```

**Exit skill immediately. Do not proceed.**

---

## Planning Process (Contracts Confirmed)

### 1. Read All Contracts

```bash
# Read parameter specification
cat plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md

# Read DSP architecture specification
cat plugins/${PLUGIN_NAME}/.ideas/architecture.md

# Read creative brief for context
cat plugins/${PLUGIN_NAME}/.ideas/creative-brief.md
```

### 2. Calculate Complexity Score

**Formula:**
```
score = min(param_count / 5, 2.0) + algorithm_count + feature_count
Cap at 5.0
```

#### Extract Metrics

**From parameter-spec.md:**

Count parameter definitions:
```bash
# Each parameter entry in parameter-spec.md counts as 1
# Look for pattern: "### [PARAM_ID]" or similar
grep -c "^###" plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md
```

Calculate param_score:
```
param_score = min(param_count / 5, 2.0)
```

**Example:**
- 3 parameters → 3/5 = 0.6
- 7 parameters → 7/5 = 1.4
- 12 parameters → 12/5 = 2.4 → capped at 2.0

**From architecture.md:**

Count DSP algorithms/components:
- Each "### [Component]" subsection in "## Core Components" = 1
- juce::dsp classes count individually
- Custom algorithms count individually

```bash
# Count component subsections
grep -c "^###" plugins/${PLUGIN_NAME}/.ideas/architecture.md
```

**From architecture.md (Feature Analysis):**

Identify complexity features:

| Feature | Score | How to Detect |
|---------|-------|--------------|
| Feedback loops | +1 | Look for "feedback" in Processing Chain or Algorithm Details |
| FFT/frequency domain | +1 | Search for "FFT", "juce::dsp::FFT", "frequency domain" |
| Multiband processing | +1 | Search for "multiband", "band split", "crossover" |
| Modulation systems | +1 | Search for "LFO", "envelope", "modulation", "juce::dsp::Oscillator" |
| External MIDI control | +1 | Search for "MIDI", "MPE", "controller", "aftertouch" |

```bash
# Example feature detection
grep -i "feedback" plugins/${PLUGIN_NAME}/.ideas/architecture.md && echo "Feedback: +1"
grep -i "FFT\|frequency domain" plugins/${PLUGIN_NAME}/.ideas/architecture.md && echo "FFT: +1"
grep -i "LFO\|envelope\|modulation" plugins/${PLUGIN_NAME}/.ideas/architecture.md && echo "Modulation: +1"
```

#### Calculate Total Score

```
total_score = param_score + algorithm_count + feature_count
final_score = min(total_score, 5.0)
```

#### Display Breakdown

```
Complexity Calculation:
- Parameters: [N] parameters ([N/5] points, capped at 2.0) = [X.X]
- Algorithms: [N] DSP components = [N]
  - [List components counted]
- Features: [N] points
  - [List features found, e.g., "Feedback loops (+1)", "FFT processing (+1)"]
Total: [X.X] / 5.0
```

### 3. Determine Implementation Strategy

**Decision matrix:**

| Score | Classification | Strategy |
|-------|---------------|----------|
| ≤ 2.0 | Simple | Single-pass implementation |
| 2.1 - 2.9 | Moderate | Single-pass (but note complexity) |
| ≥ 3.0 | Complex | Phase-based implementation |

**Simple plugins (score ≤ 2.0):**
- Implement each stage in one pass
- No phase breakdown needed
- Straightforward implementation plan

**Complex plugins (score ≥ 3.0):**
- Break Stage 4 (DSP) into phases
- Break Stage 5 (GUI) into phases
- Each phase gets git commit
- Clear test criteria per phase

### 4. Create Phase Breakdown (Complex Plugins Only)

#### Stage 4: DSP Phases

**Phase 4.1: Core Processing**

Goal: Implement essential audio path

Identify core components:
- Primary audio processing (reverb, delay, filter, etc.)
- Basic parameter connections
- Input → Core → Output path

**Phase 4.2: Parameter Modulation**

Goal: Add modulation systems

Identify modulation components:
- LFOs, envelopes
- Modulation routing
- Parameter smoothing

**Phase 4.3: Advanced Features**

Goal: Add complex features

Identify advanced components:
- FFT processing
- Feedback loops
- Multiband processing
- MIDI control

#### Stage 5: GUI Phases

**Phase 5.1: Layout and Basic Controls**

Goal: Integrate mockup and bind basic parameters

Components:
- Copy HTML mockup
- WebView setup
- Basic parameter bindings (knobs, sliders)
- Layout rendering

**Phase 5.2: Parameter Binding and Interaction**

Goal: Two-way communication

Components:
- JavaScript → C++ relay calls
- C++ → JavaScript updates
- Host automation
- Preset changes

**Phase 5.3: Advanced UI Elements**

Goal: Add complex visualizations

Components:
- VU meters
- Waveform displays
- Spectrum analyzers
- Real-time animations

---

## Create Implementation Plan

**Use template:** `assets/plan-template.md`

**File location:** `plugins/${PLUGIN_NAME}/.ideas/plan.md`

### For Simple Plugins (Score ≤ 2.0)

```markdown
# [PluginName] - Implementation Plan

**Date:** [YYYY-MM-DD]
**Complexity Score:** [X.X] (Simple)
**Strategy:** Single-pass implementation

## Complexity Factors

- Parameters: [N] parameters ([N/5] points, capped at 2.0) = [X.X]
- Algorithms: [N] DSP components = [N]
  - [List components]
- Features: [N] points
  - [List features if any]
- Total: [X.X] / 5.0

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ✓
- Stage 2: Foundation ← Next
- Stage 3: Shell
- Stage 4: DSP
- Stage 5: GUI
- Stage 6: Validation

## Estimated Duration

Total: ~[X] minutes

- Stage 2: 5 min (Foundation - project structure)
- Stage 3: 5 min (Shell - APVTS parameters)
- Stage 4: [X] min (DSP - single pass)
- Stage 5: [X] min (GUI - single pass)
- Stage 6: 15 min (Validation - presets, pluginval, changelog)

## Implementation Notes

[Any specific considerations for implementation]
```

### For Complex Plugins (Score ≥ 3.0)

Use template with full phase breakdown:
- Complexity calculation shown
- Stage 4 phases with test criteria
- Stage 5 phases with test criteria
- Duration estimates per phase

**Example phase entry:**

```markdown
### Phase 4.1: Core Processing

**Goal:** Implement basic reverb with dry/wet mixing

**Components:**
- juce::dsp::Reverb initialization
- SIZE and DECAY parameter mapping
- juce::dsp::DryWetMixer for MIX parameter
- Audio path: Input → Reverb → Mixer → Output

**Test Criteria:**
- [ ] Plugin loads in DAW without crashes
- [ ] Audio passes through (wet and dry audible)
- [ ] SIZE parameter affects reverb character
- [ ] DECAY parameter affects tail length
- [ ] MIX parameter blends correctly

**Duration:** 15 min
```

---

## State Management

### 1. Update Handoff File

**File:** `plugins/${PLUGIN_NAME}/.continue-here.md`

Update YAML frontmatter:
```yaml
---
plugin: [PluginName]
stage: 1
status: complete
last_updated: [YYYY-MM-DD HH:MM:SS]
complexity_score: [X.X]
phased_implementation: [true/false]
next_stage: 2
ready_for_implementation: true
---
```

Update markdown sections:
```markdown
## Current State: Stage 1 - Planning Complete

Implementation plan created. Ready to proceed to foundation (Stage 2).

## Completed So Far

**Stage 0:** ✓ Complete
**Stage 1:** ✓ Complete
- Complexity score: [X.X]
- Strategy: [Single-pass | Phased implementation]
- Plan documented

## Next Steps

1. Stage 2: Foundation (create build system) - Run /implement [PluginName]
2. Review plan.md details
3. Pause here

## Files Created
- plugins/[PluginName]/.ideas/architecture.md
- plugins/[PluginName]/.ideas/plan.md
```

### 2. Update PLUGINS.md (ATOMIC - both locations)

**CRITICAL:** Update BOTH registry table and full entry section to prevent drift.

**Approach 1 (Using Edit tool - recommended):**
```
1. Update full entry section (canonical source):
   Find: **Status:** 🚧 Stage 0
   Replace: **Status:** 🚧 Stage 1

2. Update registry table (derived view):
   Find: | [PluginName] | 🚧 Stage 0 | ...
   Replace: | [PluginName] | 🚧 Stage 1 | ...

3. Add timeline entry to full section:
   - **[YYYY-MM-DD] (Stage 1):** Planning complete - Complexity [X.X]

4. Update Last Updated field in full section

5. Update Last Updated date in registry table
```

**Approach 2 (Using helper function):**
```
Reference .claude/skills/plugin-workflow/references/state-management.md
Call: updatePluginStatus([PluginName], "🚧 Stage 1")
This ensures atomic updates with automatic validation.
```

**Verification:**
After updates, verify consistency:
```bash
# Registry table should match full entry
TABLE=$(grep "^| [PluginName] |" PLUGINS.md | awk -F'|' '{print $3}' | xargs)
ENTRY=$(grep -A 10 "^### [PluginName]$" PLUGINS.md | grep "^\*\*Status:\*\*" | sed 's/\*\*//g' | xargs)
# Both should show: 🚧 Stage 1
```

### 3. Git Commit

```bash
git add \
  plugins/${PLUGIN_NAME}/.ideas/plan.md \
  plugins/${PLUGIN_NAME}/.continue-here.md \
  PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 1 - planning complete

Complexity: [X.X]
Strategy: [Single-pass | Phased implementation]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

**Display commit hash:**
```bash
git log -1 --format='✓ Committed: %h - Stage 1 complete'
```

---

## Decision Menu

**Present numbered list (NOT AskUserQuestion):**

```
✓ Stage 1 complete: Implementation plan created (Complexity [X.X], [single-pass/phased])

What's next?
1. Continue to Stage 2 - Foundation (via /implement) (recommended)
2. Review plan.md details
3. Adjust complexity assessment
4. Review contracts (parameter-spec, architecture)
5. Pause here
6. Other

Choose (1-6): _
```

**Handle user input:**

| Input | Action |
|-------|--------|
| 1 or "continue" | Display handoff message, exit skill |
| 2 or "review" | Display plan.md, re-present menu |
| 3 | Recalculate complexity with user input, update plan.md |
| 4 | Display contracts, re-present menu |
| 5 or "pause" | Exit skill with handoff ready |
| 6 or "other" | Ask "What would you like to do?", re-present menu |

---

## Handoff to Implementation

**When user chooses option 1 (continue):**

Display final message:

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✓ Planning Complete
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Plugin: [PluginName]
Complexity: [X.X] ([Simple/Complex])
Strategy: [Single-pass | Phased implementation]

Contracts created:
✓ creative-brief.md
✓ parameter-spec.md
✓ architecture.md
✓ plan.md

Ready to build. Run: /implement [PluginName]
```

Exit skill. plugin-workflow will pick up from .continue-here.md.

---

**Return to:** Main plugin-planning orchestration in `SKILL.md`
