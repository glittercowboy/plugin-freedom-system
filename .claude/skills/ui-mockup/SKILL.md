---
name: ui-mockup
description: Orchestrator for WebView UI mockup workflow - delegates design iteration to ui-design-agent and implementation scaffolding to ui-finalization-agent
allowed-tools:
  - Read
  - Task
  - AskUserQuestion
preconditions:
  - None (can work standalone or with creative brief)
---

# ui-mockup Skill

**Purpose:** Pure orchestrator for WebView UI mockup workflow. Gathers requirements from user, then delegates file generation to specialized subagents.

**Orchestration Pattern:**
- This skill NEVER generates files directly
- Phase A (design iteration): Delegates to ui-design-agent via Task tool
- Phase B (implementation scaffolding): Delegates to ui-finalization-agent via Task tool
- User interaction (questions, menus) handled in orchestrator
- File generation handled in fresh agent contexts (prevents context bloat)

## Workflow Overview

<critical_sequence phases="A,B" enforcement="gate">
<phase id="A" name="Design Iteration">
**Purpose:** Generate 2 design files for rapid iteration.

**Outputs:**
1. **v[N]-ui.yaml** - Machine-readable design specification
2. **v[N]-ui-test.html** - Browser-testable mockup (no JUCE required)

<decision_gate id="design_approval" required="true">
**STOP:** Do NOT proceed to Phase B until user approves design via Phase 5.5 decision menu.

**Gate criteria:**
- User selected option 2 from Phase 5.5 menu ("Finalize")
- Design validated against WebView constraints (Phase 5.3)
- Design validated against creative brief (Phase 5.6 automatic validation)
</decision_gate>
</phase>

<phase id="B" name="Implementation Scaffolding" requires_gate="design_approval">
**Purpose:** Generate 5 implementation files ONLY after Phase A approval.

**Outputs:**
3. **v[N]-ui.html** - Production HTML (copy-paste to plugin)
4. **v[N]-PluginEditor.h** - C++ header boilerplate
5. **v[N]-PluginEditor.cpp** - C++ implementation boilerplate
6. **v[N]-CMakeLists.txt** - WebView build configuration
7. **v[N]-integration-checklist.md** - Implementation steps
</phase>
</critical_sequence>

**Why two phases?** HTML mockups are cheap to iterate. C++ boilerplate is pointless if design isn't locked. This saves time by avoiding premature scaffolding generation.

All files saved to: `plugins/[PluginName]/.ideas/mockups/`

## Workflow Context Detection

This skill operates in two modes:

**Standalone Mode**:
- Invoked directly via natural language or /dream command
- No workflow state files present
- Generates mockups independently
- Skips state updates to .continue-here.md

**Workflow Mode**:
- Invoked by plugin-workflow skill during Stage 0
- File exists: `plugins/[PluginName]/.continue-here.md`
- File contains: `current_stage` field
- Updates workflow state after each phase

**Detection Logic**:
```bash
if [ -f "plugins/[PluginName]/.continue-here.md" ]; then
    # Workflow mode: update state files
    WORKFLOW_MODE=true
else
    # Standalone mode: skip state updates
    WORKFLOW_MODE=false
fi
```

## Phase 0: Check for Aesthetic Library

**Before starting design, check if saved aesthetics exist.**

```bash
if [ -f .claude/aesthetics/manifest.json ]; then
    AESTHETIC_COUNT=$(jq '.aesthetics | length' .claude/aesthetics/manifest.json)
    if [ $AESTHETIC_COUNT -gt 0 ]; then
        echo "Found $AESTHETIC_COUNT saved aesthetics"
    fi
fi
```

**If aesthetics exist, present decision menu:**

```
Found $AESTHETIC_COUNT saved aesthetics in library.

How would you like to start the UI design?
1. Start from aesthetic template - Apply saved visual system
2. Start from scratch - Create custom design
3. List all aesthetics - Browse library before deciding

Choose (1-3): _
```

**Option handling:**

- **Option 1: Start from aesthetic template**
  - Read manifest: `.claude/aesthetics/manifest.json`
  - Display available aesthetics with metadata:
    ```
    Available aesthetics:

    1. Vintage Hardware (vintage-hardware-001)
       Vibe: Vintage analog
       Colors: Orange/cream/brown with paper texture
       From: TapeAge mockup v2

    2. Modern Minimal Blue (modern-minimal-002)
       Vibe: Clean, modern
       Colors: Blue/gray/white with subtle shadows
       From: EQ4Band mockup v1

    4. None (start from scratch)

    Choose aesthetic: _
    ```
  - If user selects aesthetic: Invoke ui-template-library skill with "apply" operation
  - Skip to Phase 4 with generated mockup from aesthetic

- **Option 2: Start from scratch**
  - Continue to Phase 1 (load context)

- **Option 3: List all aesthetics**
  - Invoke ui-template-library skill with "list" operation
  - Show preview paths
  - Return to option menu

**If no aesthetics exist:**
- Skip Phase 0
- Continue directly to Phase 1

**See:** `references/aesthetic-integration.md` for complete integration details

---

## Phase 1: Load Context from Creative Brief

**CRITICAL: Read creative-brief.md if it exists:**

```bash
if [ -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md" ]; then
    # Extract context (see references/context-extraction.md)
    # Continue to Phase 1.5 with design-informed prompt
else
    # Skip to Phase 1.5 with generic prompt (standalone mode)
fi
```

**Note:** preconditions="None" means skill can work standalone without creative-brief.md, but MUST read it when present.

**Context extraction checklist:**
- Plugin type (compressor, EQ, reverb, synth, utility)
- Parameter count and types (sliders, toggles, combos)
- Visual style mentions (vintage, modern, minimal, skeuomorphic)
- Layout preferences (horizontal, vertical, grid, custom)
- Special elements (meters, waveforms, visualizers, animations)
- Color/theme references (dark, light, specific colors)

**See:** `references/context-extraction.md#example-extracting-from-creative-brief` for detailed extraction examples and guidelines

**Extract UI context from creative-brief.md:**

- **UI Concept section:** Layout preferences, visual style mentions
- **Parameters:** Count and types (determines control layout)
- **Plugin type:** Effect/synth/utility (affects typical layouts)
- **Vision section:** Any visual references or inspirations

## Phase 1.5: Context-Aware Initial Prompt

**Adapt the prompt based on what's in the creative brief:**

**If rich UI details exist:**
```
I see you want [extracted description from UI Concept] for [PluginName]. Let's refine that vision. Tell me more about the layout, control arrangement, and visual elements you're imagining.
```

**If minimal UI details:**
```
Let's design the UI for [PluginName]. You mentioned it's a [type] with [X] parameters. What layout and style are you envisioning?
```

**If zero UI context:**
```
Let's design the UI for [PluginName]. What do you envision? (layout, style, controls, visual elements)
```

**Why context-aware:** Don't ask the user to repeat information they already provided in the creative brief. Build on what they said.

**Listen for:**

- Layout preferences ("controls on left, visualizer on right")
- Visual references ("like FabFilter Pro-C", "vintage analog gear")
- Mood/feel ("minimal and clean", "skeuomorphic wood panels")
- Special requests ("animated VU meter", "resizable window")

**Capture verbatim notes before moving to targeted questions.**

## Phase 2: Gap Analysis and Question Prioritization

**Question Priority Tiers:**

- **Tier 1 (Critical):** Layout structure, control types
- **Tier 2 (Visual):** Visual style, key visual elements (meters, waveforms, displays)
- **Tier 3 (Polish):** Colors, typography, animations, interactive features

**Extract from Phase 1.5 response and creative brief, then identify gaps:**

1. Parse user's UI description
2. Check which tiers are covered
3. Identify missing critical/visual information
4. Never ask about already-provided information

**Example of context-aware extraction:**

```
Creative brief UI Concept:
"Vintage aesthetic with three knobs"

Phase 1.5 user response:
"I want the knobs arranged horizontally, with a tape reel animation in the background"

Extracted:
- Layout: horizontal arrangement ✓
- Control type: knobs ✓
- Visual style: vintage ✓
- Special element: tape reel animation ✓

Gaps identified:
- Window size? (Tier 1)
- Vintage style details? (Tier 2)
- Knob style (large vintage vs small precise)? (Tier 2)
- VU meters or other feedback? (Tier 2)
```

## Phase 2.5: Calculate Recommended Dimensions

**Purpose:** Calculate space requirements before asking user for window size.

**Reference:** See `.claude/skills/ui-mockup/references/layout-validation.md` Section 2 (Calculation Helpers) for formulas.

**Implementation steps:**

1. **Parse accumulated requirements**
   - Count controls from creative-brief.md (if exists) and user responses
   - Identify layout type (horizontal, vertical, grid, custom) from user descriptions
   - Note any special elements (meters, waveforms, visualizers)

2. **Estimate control sizes**
   - Use typical sizes from layout-validation.md Section 2
   - Example: If user mentioned "3 large knobs" → 3 × 100px × 100px
   - Example: If user mentioned "horizontal sliders" → estimate 200px × 30px each
   - Selection criteria:
     - Vintage/skeuomorphic style → Large (100px knobs)
     - Modern/minimal style → Medium (80px knobs)
     - Utility/compact style → Small (60px knobs)

3. **Apply layout-specific formula**
   - Based on layout type, use calculation helper from layout-validation.md
   - Example (horizontal): `min_width = sum(control_widths) + (control_count + 1) × spacing`
   - Include margins (default: 30px left/right, 30px top/bottom)
   - Include label space (default: 20px above controls)
   - Use default spacing: 20px between controls

4. **Calculate recommended dimensions**
   ```
   absolute_minimum = calculate_from_formula(layout_type, controls)
   recommended = absolute_minimum × 1.2  # 20% breathing room
   recommended_width = ceil(recommended.width / 50) × 50  # Round to nearest 50px
   recommended_height = ceil(recommended.height / 50) × 50

   # Enforce constraints
   recommended_width = max(recommended_width, 400)   # Minimum 400px
   recommended_width = min(recommended_width, 1200)  # Maximum 1200px
   recommended_height = max(recommended_height, 300) # Minimum 300px
   recommended_height = min(recommended_height, 800) # Maximum 800px
   ```

5. **Present calculation to user**
   ```
   Based on your requirements:
   - Layout: [horizontal/vertical/grid/custom]
   - Controls: [count] controls ([list types])
   - Special elements: [meters/waveforms/none]

   Calculated space requirements:
   - Absolute minimum: [min_width] × [min_height] px (tight fit)
   - Recommended: [rec_width] × [rec_height] px (comfortable spacing)

   [CONTINUE TO PHASE 3 - don't ask for window size yet]
   ```

6. **Store calculated dimensions**
   - Save recommended dimensions in context
   - Will be used in Phase 3 when asking window size question

**DO NOT ask for window size in this phase** - that happens in Phase 3 Question Batch

**Integration points:**

- **Before Phase 2.5:** User has provided layout preferences, control types, visual style
- **After Phase 2.5:** Proceed to Phase 3 (Question Batch Generation)
- **Phase 3 modification:** When generating window size question, use calculated recommended dimensions as default/suggested value

## Phase 3: Question Batch Generation

**Generate exactly 4 questions using AskUserQuestion based on identified gaps.**

**Rules:**
- If 4+ gaps exist: ask top 4 by tier priority
- If fewer gaps exist: pad with "nice to have" tier 3 questions
- Provide meaningful options (not just open text prompts)
- Always include "Other" option for custom input
- Users can skip questions via "Other" option and typing "skip"

**Note:** Internal question routing uses AskUserQuestion tool, but final decision menus (Phase 5.5, Phase 10.7) MUST use inline numbered format per checkpoint protocol.

**Window Size Question (uses Phase 2.5 calculations):**

When window size is a gap, use calculated dimensions from Phase 2.5:

```
Question:
  question: "Window dimensions for your plugin?"
  header: "Window size"
  options:
    - label: "[calculated_width]×[calculated_height] (recommended)",
      description: "Calculated based on your layout and controls"
    - label: "Custom size",
      description: "Specify different dimensions"
```

**If user chooses custom size:**
- Ask for specific width and height
- Compare against absolute minimum from Phase 2.5
- If smaller than minimum, present warning (see below)

**Warning for undersized windows:**

If user chooses custom size smaller than absolute minimum:

```
⚠️  Warning: Your chosen size ([width]×[height]) is smaller than the calculated minimum ([min_width]×[min_height]).
Controls may overflow or overlap.

Proceed anyway?
1. Yes, use my size (may need to adjust layout)
2. Use recommended size instead ([rec_width]×[rec_height])

Choose (1-2): _
```

**Example question batch (via AskUserQuestion):**

```
Question 1:
  question: "Layout structure for the three knobs?"
  header: "Layout"
  options:
    - label: "Horizontal row of knobs", description: "Classic single-row layout"
    - label: "Vertical stack", description: "Narrow, tall layout"
    - label: "Triangle arrangement", description: "Two bottom, one top"
    - label: "Other", description: "Custom arrangement"

Question 2:
  question: "Vintage style details?"
  header: "Aesthetic"
  options:
    - label: "Brushed metal", description: "Industrial, professional"
    - label: "Wood panel", description: "Warm, classic studio"
    - label: "Reel-to-reel theme", description: "Tape machine aesthetic"
    - label: "Other", description: "Different vintage style"

Question 3:
  question: "Visual feedback elements?"
  header: "Meters/displays"
  options:
    - label: "VU meters", description: "Needle-style level indicators"
    - label: "Tape reel animation", description: "Spinning reel visual"
    - label: "Both VU and reels", description: "Full tape machine feel"
    - label: "None (controls only)", description: "Clean, minimal"

Question 4:
  question: "Knob style?"
  header: "Controls"
  options:
    - label: "Large vintage knobs", description: "60s-70s style, prominent"
    - label: "Small precise knobs", description: "Modern, technical"
    - label: "Chicken-head style", description: "Classic pointer knobs"
    - label: "Other", description: "Different knob style"
```

**After receiving answers:**
1. Accumulate context with previous responses
2. Re-analyze gaps
3. Proceed to decision gate

**Question batch generation:**

Generate 4 questions using AskUserQuestion based on identified gaps.

**Question structure pattern:**
- question: Clear, specific question about the gap
- header: Short category label (max 12 chars)
- options: 2-4 distinct choices + "Other" (automatically added)

**See:** `references/design-questions.md#example-question-batches` for question templates and tiering examples.

**Tier priority:**
1. Critical gaps (layout, control types) - ask first
2. Visual gaps (style, key elements) - ask second
3. Polish gaps (colors, animations) - ask if needed

## Phase 3.5: Decision Gate

**Use AskUserQuestion with 3 options after each question batch:**

```
Question:
  question: "Ready to finalize the mockup design?"
  header: "Next step"
  options:
    - label: "Yes, finalize it", description: "Generate YAML and test HTML"
    - label: "Ask me 4 more questions", description: "Continue refining"
    - label: "Let me add more context first", description: "Provide additional details"

Route based on answer:
- Option 1 → Proceed to Phase 4 (generate YAML and test HTML)
- Option 2 → Return to Phase 2 (re-analyze gaps, generate next 4 questions)
- Option 3 → Collect free-form text, merge with context, return to Phase 2
```

## Phase 4-5.45: Dispatch ui-design-agent

**Invoke subagent via Task tool to generate design files (YAML + test HTML).**

<delegation_protocol>
**Before invocation:**

1. **Read context files:**
   - creative-brief.md (if exists)
   - Aesthetic template (if user selected one in Phase 0)
   - User requirements from Phases 1-3

2. **Detect version number:**
   ```bash
   MOCKUP_DIR="plugins/${PLUGIN_NAME}/.ideas/mockups"
   LATEST_VERSION=$(ls -1 "$MOCKUP_DIR"/v*-ui.yaml 2>/dev/null | \
                    sed 's/.*v\([0-9]*\)-.*/\1/' | sort -n | tail -1)
   NEXT_VERSION=$((LATEST_VERSION + 1))
   ```

3. **Construct prompt with all context:**
   - Plugin name
   - Creative brief summary (if exists)
   - User requirements (layout, style, controls)
   - Aesthetic template content (if selected)
   - Version number (v[N])
   - Refinement feedback (if iteration)

4. **Invoke ui-design-agent:**
   ```
   Task tool with:
   - subagent_type: "ui-design-agent"
   - description: "Generate UI mockup v[N] for [PluginName]"
   - prompt: [constructed prompt with all context]
   ```

5. **Wait for JSON report:**
   ```json
   {
     "status": "success" | "error",
     "phase": "design-iteration",
     "version": N,
     "filesCreated": ["v[N]-ui.yaml", "v[N]-ui-test.html"],
     "validationPassed": true | false,
     "stateUpdated": true | false,
     "commitHash": "abc123",
     "error": "..." (if status == "error")
   }
   ```

6. **Handle result:**
   - If status == "error": Present error menu (retry/manual fix/cancel)
   - If validationPassed == false: Agent already retried, present error menu
   - If stateUpdated == false: Log warning (orchestrator will verify later)
   - If status == "success": Continue to Phase 5.5

**Iteration support:**
- If user chose "Iterate" in Phase 5.5, invoke NEW ui-design-agent with:
  - Previous version number (for reference)
  - Refinement feedback from user
  - Incremented version number (v2, v3, etc.)
</delegation_protocol>

**Error menu (if agent fails):**
```
✗ Design generation failed: [error message]

What would you like to do?

1. Retry - Invoke ui-design-agent again
2. Manual fix - I'll create files myself
3. Debug - Show agent output
4. Cancel - Stop workflow
5. Other

Choose (1-5): _
```

---

<phase_gate_enforcement id="design-approval-gate" enforcement_level="STRICT">
**Purpose:** Prevent generating implementation scaffolding (Phase B: files 3-7) before design is finalized (Phase A: files 1-2).

**Why critical:** C++ boilerplate generation is expensive. If design changes after Phase B runs, all 5 implementation files must be regenerated. The two-phase approach saves time by deferring scaffolding until design is locked.

**Gate Trigger:** After Phase A completes (v[N]-ui.yaml + v[N]-ui-test.html generated and committed)

## Phase A Completion Detection

Before presenting decision menu, verify Phase A artifacts exist:

```bash
# Check for latest design iteration files
MOCKUP_DIR="plugins/${PLUGIN_NAME}/.ideas/mockups"
LATEST_VERSION=$(ls -1 "$MOCKUP_DIR"/v*-ui.yaml 2>/dev/null | \
                 sed 's/.*v\([0-9]*\)-.*/\1/' | sort -n | tail -1)

if [ -n "$LATEST_VERSION" ]; then
  YAML_FILE="$MOCKUP_DIR/v${LATEST_VERSION}-ui.yaml"
  TEST_HTML="$MOCKUP_DIR/v${LATEST_VERSION}-ui-test.html"

  # Phase A complete if both files exist
  if [ -f "$YAML_FILE" ] && [ -f "$TEST_HTML" ]; then
    PHASE_A_COMPLETE=true
  else
    echo "✗ ERROR: Phase A incomplete (missing YAML or test HTML)"
    exit 1
  fi
else
  echo "✗ ERROR: No mockup versions found"
  exit 1
fi
```

## Decision Menu (Required - Phase 5.5)

When Phase A completes, MUST present decision menu and WAIT for user choice.

**Menu format and option routing:** See `references/decision-menus.md#phase-5-5-design-decision-menu`

**Gate enforcement:** Phases 6-10 are CONDITIONALLY EXECUTED based on user choice. Only option 2 (Finalize) proceeds to Phase B.

## State Tracking (Finalization Marker)

When user selects option 2 (Finalize), mark design as finalized in YAML:

```bash
# Append finalization metadata to YAML
cat >> "$MOCKUP_DIR/v${LATEST_VERSION}-ui.yaml" << EOF

# Finalization metadata
finalized: true
finalized_at: $(date -u +%Y-%m-%dT%H:%M:%SZ)
finalized_by_phase: 5.5
EOF
```

**Purpose:**
- Prevents accidental regeneration of finalized designs
- Tracks which version was approved for implementation
- Enables version history queries (which designs were finalized vs exploratory)

## Phase B Guard Protocol

Before generating any Phase B file (Phases 6-10), verify design finalization.

**See:** `references/phase-b-enforcement.md` for complete guard implementation.

## Anti-Patterns to Avoid

**Critical rule: NEVER generate Phase B files (3-7) without Phase 5.5 menu approval.**

**See:** `references/common-pitfalls.md` for detailed anti-patterns and why they matter.

**Quick checklist:**
- ✓ Present Phase 5.5 menu after Phase A
- ✓ Wait for option 2 (Finalize) before Phase B
- ✓ Check finalization marker in YAML
- ✓ Validate WebView constraints (Phase 5.3)
- ✓ Read creative-brief.md if it exists

</phase_gate_enforcement>

---

<decision_gate id="phase_5_5_approval" blocking="true">
## Phase 5.5: Design Decision Menu

**Gate enforcement:** Phases 6-10 are CONDITIONALLY EXECUTED based on user choice.

<menu_presentation>
Present this decision menu:

```
✓ Mockup v[N] design created (2 files)

Files generated:
- v[N]-ui.yaml (design specification)
- v[N]-ui-test.html (browser-testable mockup)

What would you like to do?

1. Iterate - Refine design, adjust layout
2. Finalize - Validate alignment and complete mockup
3. Save as template - Add to aesthetic library for reuse
4. Other

Choose (1-4): _
```
</menu_presentation>

<conditional_execution requires="user_choice">
**Execution routing:**
- IF user chose option 2 (finalize) THEN proceed to Phase 5.6 (automatic validation)
- IF user chose option 1 (iterate) THEN return to Phase 2 with v[N+1]
- IF user chose option 3 (save template) THEN invoke ui-template-library skill
- ELSE handle custom options

**DO NOT proceed to Phase 6 unless user explicitly chose finalization option.**

**Option handling:**

- **Option 1: Iterate** → User gives feedback → Return to Phase 2 with new version number (v2, v3, etc.)

- **Option 2: Finalize** → Proceed to Phase 5.6 (automatic brief update)
  - AUTOMATIC: Update creative-brief.md from finalized mockup
  - Preserves conceptual sections (Vision, Use Cases, Inspirations)
  - Updates UI sections (Parameters, UI Concept) from mockup
  - No user interaction needed - mockup is source of truth
  - Proceeds to Phase 6-10 after brief update
  - See Phase 5.6 for update protocol

- **Option 3: Save as template** → Invoke ui-template-library skill with "save" operation
  ```
  Invoke Skill tool:
  - skill: "ui-template-library"
  - prompt: "Save aesthetic from plugins/[PluginName]/.ideas/mockups/v[N]-ui-test.html"
  ```
  After saving, return to Phase 5.5 decision menu

- **Option 4: Other** → Collect free-form text, handle custom request (test in browser, validate WebView constraints, etc.)
</conditional_execution>
</decision_gate>

---

<conditional_execution id="phase_5_6_brief_update" requires_gate="phase_5_5_approval">
## Phase 5.6: Update Creative Brief from Finalized Mockup

**Purpose:** Automatically sync creative-brief.md with finalized mockup design. Treats mockup as source of truth for UI decisions.

**Trigger:** User selected "Finalize" option in Phase 5.5

**Protocol:**

1. **Check if creative-brief.md exists:**
   - If no brief exists (standalone mockup mode): Skip this phase, proceed to Phase 6-10
   - If brief exists: Continue to step 2

2. **Determine plugin name and mockup version:**
   - Extract from context or current working directory
   - Find latest mockup version (highest v[N] in .ideas/mockups/)

3. **Execute sync script:**
   ```bash
   .claude/utils/sync-brief-from-mockup.sh "${PLUGIN_NAME}" "${MOCKUP_VERSION}"
   ```

4. **Script performs:**
   - Reads current creative-brief.md
   - Preserves conceptual sections: Overview, Vision, Use Cases, Inspirations, Technical Notes
   - Updates Parameters section from parameter-spec.md
   - Updates UI Concept section from v[N]-ui.yaml (layout, style, dimensions, features)
   - Reconstructs brief maintaining section order
   - Writes updated creative-brief.md

5. **Update workflow state:**
   - Mark in .continue-here.md:
     ```yaml
     brief_updated_from_mockup: true
     mockup_version_synced: {N}
     brief_update_timestamp: {ISO-8601}
     ```

6. **Commit changes:**
   ```bash
   git add plugins/${PLUGIN_NAME}/.ideas/creative-brief.md
   git add plugins/${PLUGIN_NAME}/.continue-here.md
   git commit -m "docs(${PLUGIN_NAME}): sync creative brief with finalized mockup v${VERSION}"
   ```

7. **Present confirmation:**
   ```
   ✓ Creative brief updated from mockup v{N}

   Updated sections:
   - Parameters (from parameter-spec.md)
   - UI Concept (layout, visual style, dimensions from mockup)

   Preserved sections:
   - Vision (original concept intact)
   - Use Cases (unchanged)
   - Inspirations (unchanged)
   - Technical Notes (unchanged)

   Proceeding to implementation file generation...
   ```

8. **Continue to Phase 6-10:** Generate 5 implementation files

**Error Handling:**
- If brief parse fails: Display error, offer manual fix option
- If YAML parse fails: Fallback to minimal update (dimensions only), warn user
- If git commit fails: Warn but continue (state recoverable from git)

**No user interaction required** - automatic update with confirmation display only.
</conditional_execution>

---

<conditional_execution requires_gate="phase_5_5_approval">
<critical_sequence phases="6,7,8,9,10" enforcement="sequential">

## Phase B: Implementation Scaffolding (Phases 6-10)

**Prerequisites for ALL Phase B phases:**
- User confirmed design in Phase 5.5 decision menu (selected option 2: Finalize)
- Execute Phase B guard from `references/phase-b-enforcement.md` before each phase
- Phase A files (v[N]-ui.yaml and v[N]-ui-test.html) exist and validated

These prerequisites apply to Phases 6, 7, 8, 9, and 10. Verify guard before proceeding to any Phase B phase.

---

## Phase 6-10.5: Dispatch ui-finalization-agent

**Invoke subagent via Task tool to generate implementation files (5 files + parameter-spec.md if v1).**

<delegation_protocol>
**Before invocation:**

1. **Read finalized design files:**
   - v[N]-ui.yaml (finalized YAML specification)
   - v[N]-ui-test.html (finalized test HTML)
   - parameter-spec.md (if exists - for v2+)

2. **Detect if parameter-spec.md generation needed:**
   ```bash
   PARAM_SPEC_PATH="plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md"
   if [ ! -f "$PARAM_SPEC_PATH" ]; then
     GENERATE_PARAM_SPEC=true
     # Check for draft
     DRAFT_PATH="plugins/${PLUGIN_NAME}/.ideas/parameter-spec-draft.md"
     if [ -f "$DRAFT_PATH" ]; then
       DRAFT_EXISTS=true
     fi
   fi
   ```

3. **Construct prompt with all context:**
   - Plugin name
   - Version number (v[N])
   - Finalized YAML path
   - Finalized HTML path
   - parameter-spec.md path OR flag to generate it
   - Draft path (if exists and param-spec.md needed)

4. **Invoke ui-finalization-agent:**
   ```
   Task tool with:
   - subagent_type: "ui-finalization-agent"
   - description: "Generate implementation files for mockup v[N]"
   - prompt: [constructed prompt with contracts]
   ```

5. **Wait for JSON report:**
   ```json
   {
     "status": "success" | "error",
     "phase": "implementation-scaffolding",
     "version": N,
     "filesCreated": [
       "v[N]-ui.html",
       "v[N]-PluginEditor-TEMPLATE.h",
       "v[N]-PluginEditor-TEMPLATE.cpp",
       "v[N]-CMakeLists-SNIPPET.txt",
       "v[N]-integration-checklist.md",
       "parameter-spec.md" (if v1 only)
     ],
     "parameterSpecGenerated": true | false,
     "draftConflictDetected": true | false,
     "stateUpdated": true | false,
     "commitHash": "abc123",
     "error": "..." (if status == "error")
   }
   ```

6. **Handle result:**
   - If draftConflictDetected == true: Agent presented resolution menu, user handled it
   - If status == "error": Present error menu (retry/manual fix/cancel)
   - If stateUpdated == false: Log warning
   - If status == "success": Continue to Phase 10.7 (completion menu)
</delegation_protocol>

**Error menu (if agent fails):**
```
✗ Implementation file generation failed: [error message]

What would you like to do?

1. Retry - Invoke ui-finalization-agent again
2. Manual fix - I'll create files myself
3. Debug - Show agent output
4. Cancel - Stop workflow
5. Other

Choose (1-5): _
```

---

## Orchestration Protocol

**Pure orchestrator pattern - delegates file generation to subagents.**

<orchestration_rules enforcement_level="STRICT">
  <delegation_rule id="no-direct-file-generation" violation="IMMEDIATE_STOP">
    This skill NEVER generates mockup files directly.
    ALL file generation delegated to subagents via Task tool.

    **Delegation sequence:**
    1. Orchestrator: Gather requirements (Phases 0-3.5)
    2. Orchestrator: Invoke ui-design-agent via Task tool
    3. Agent: Generate YAML + test HTML, commit, return JSON report
    4. Orchestrator: Parse JSON report, present Phase 5.5 menu
    5. If iterate: Orchestrator invokes NEW ui-design-agent instance
    6. If finalize: Orchestrator updates creative brief from mockup (Phase 5.6)
    7. Orchestrator: Invoke ui-finalization-agent via Task tool
    8. Agent: Generate 5 implementation files, commit, return JSON report
    9. Orchestrator: Parse JSON report, present completion menu (Phase 10.7)

    <enforcement>
      IF orchestrator attempts to generate files directly:
        STOP execution
        DISPLAY error: "File generation must be delegated to subagents. Use Task tool."
    </enforcement>
  </delegation_rule>

  <state_management_protocol>
    **Subagents handle their own state updates:**
    - ui-design-agent updates .continue-here.md with version tracking
    - ui-finalization-agent updates .continue-here.md with finalization status
    - Both agents commit their changes before returning

    **Orchestrator verifies stateUpdated flag:**
    - Check JSON report stateUpdated field
    - If false: Log warning and offer manual state recovery menu
    - If true: Proceed to next phase

    **State recovery menu (if stateUpdated == false):**
    ```
    ⚠️ Agent did not update state files

    What would you like to do?

    1. Verify state - Check if update actually happened
    2. Manual update - I'll fix .continue-here.md myself
    3. Continue anyway - State not critical for this step
    4. Other

    Choose (1-4): _
    ```
  </state_management_protocol>

  <iteration_protocol>
    **Each iteration runs in fresh agent context:**
    - User chooses "Iterate" in Phase 5.5
    - Orchestrator collects refinement feedback
    - Orchestrator invokes NEW ui-design-agent with:
      - Previous version context (for reference)
      - Refinement requirements
      - Incremented version number (v2, v3, etc.)
    - NEW agent has no accumulated context from previous iterations
    - This prevents context window bloat during iterative design

    **Why fresh context matters:**
    - After 4-5 iterations, context window fills and blocks progression
    - Fresh agent context keeps each iteration lightweight
    - Orchestrator maintains conversation history, agent does not
  </iteration_protocol>

  <error_handling_protocol>
    **Agent failures:**
    - Agent returns status == "error" in JSON report
    - Orchestrator presents error menu with options:
      1. Retry (invoke agent again with same prompt)
      2. Manual fix (user creates files themselves)
      3. Debug (show full agent output)
      4. Cancel (stop workflow)

    **Validation failures:**
    - Agent returns validationPassed == false
    - Agent already attempted retry internally
    - Orchestrator presents error menu (same as above)

    **State update failures:**
    - Agent returns stateUpdated == false
    - Not blocking - orchestrator can continue
    - Present state recovery menu (see above)
  </error_handling_protocol>
</orchestration_rules>

---

## After Completing All Phases

**Phase 10.7: Completion Menu**

Present completion menu after ui-finalization-agent successfully returns.

**Menu format and option routing:** See `references/decision-menus.md#completion-menu`

## Versioning Strategy

**Pattern:** v1, v2, v3... Each UI version is saved separately.

**Purpose:**
- Explore different layouts without losing previous work
- A/B test designs before committing
- Keep design history for rollback

**File naming:** All 7 files prefixed with version (e.g., `v2-ui.html`, `v2-PluginEditor.h`)

**Implementation:** Latest version used for Stage 5 unless user specifies different version.

**See:** `references/versioning.md` for file management details.

## Success Criteria

**Design phase successful when:**
- ✅ YAML spec generated matching user requirements
- ✅ Browser test HTML works (interactive controls, parameter messages)
- ✅ Design files committed to git (Phase 5.45)
- ✅ `.continue-here.md` updated with version number (if in workflow)
- ✅ User presented with Phase 5.5 decision menu
- ✅ Design approved OR user iterates with refinements

**Implementation phase successful when (after finalization):**
- ✅ All 7 files generated and saved to `.ideas/mockups/`
- ✅ Production HTML is complete (no placeholder content)
- ✅ C++ boilerplate matches YAML structure (correct parameter bindings)
- ✅ parameter-spec.md generated and locked (for v1 only)
- ✅ Implementation files committed to git (Phase 10.5)
- ✅ `.continue-here.md` updated with finalization status (if in workflow)

## Integration Points

**Invoked by:**

- `/dream` command → After creative brief, before parameter finalization
- `plugin-workflow` skill → During Stage 0 (UI design phase)
- `plugin-improve` skill → When redesigning existing plugin UI
- Natural language: "Design UI for [PluginName]", "Create mockup for compressor"

**ALWAYS invokes (via Task tool):**

- `ui-design-agent` subagent (Phase 4-5.45) - REQUIRED for design iteration
- `ui-finalization-agent` subagent (Phase 6-10.5) - REQUIRED for implementation files
- Never generates files directly - orchestration only

**Also invokes:**

- `ui-template-library` skill (if user saves aesthetic)

**Creates (via subagents):**

- `plugins/[Name]/.ideas/mockups/v[N]-*.{yaml,html,h,cpp,txt,md}` (7 files)
- `plugins/[Name]/.ideas/parameter-spec.md` (if v1 and doesn't exist)

**Updates (via subagents):**

- `PLUGINS.md` → Mark UI designed (if part of workflow)
- `.continue-here.md` → Update workflow state (if part of workflow)

**Blocks:**

- Stage 1 (Planning) → Cannot proceed without parameter-spec.md
- Stage 5 (GUI) → Cannot implement without approved UI mockup

## Reference Documentation

- **Context extraction:** `references/context-extraction.md` - Guidelines for loading plugin context
- **Design questions:** `references/design-questions.md` - Targeted question templates and defaults
- **HTML generation:** `references/html-generation.md` - Rules for production HTML generation
- **Browser testing:** `references/browser-testing.md` - Browser test workflow
- **CMake configuration:** `references/cmake-configuration.md` - WebView build settings

## Template Assets

- **UI YAML template:** `assets/ui-yaml-template.yaml` - Complete YAML structure
- **WebView boilerplate:** `assets/webview-boilerplate.md` - C++ integration templates
- **Integration checklist:** `assets/integration-checklist-template.md` - Step-by-step integration guide
- **Parameter spec template:** `assets/parameter-spec-template.md` - Parameter specification format
