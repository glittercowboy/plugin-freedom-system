---
name: plugin-workflow
description: Complete 7-stage JUCE plugin development workflow
allowed-tools:
  - Task # For subagents (Stages 2-5)
  - Bash # For git commits
  - Read # For contracts
  - Write # For documentation
  - Edit # For state updates
preconditions:
  - Plugin must not exist OR status must be 💡 (ideated)
  - Plugin must NOT be 🚧 (use /continue instead)
---

# plugin-workflow Skill

**Purpose:** Orchestrate complete 7-stage JUCE plugin development from research to validated, production-ready plugin.

## Precondition Checking

**Before starting, verify:**

1. Read PLUGINS.md:

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

2. Check status:

   - If not found → OK to proceed (new plugin)
   - If status = 💡 Ideated → OK to proceed
   - If status = 🚧 Stage N → BLOCK with message:
     ```
     [PluginName] is already in development (Stage [N]).
     Use /continue [PluginName] to resume the workflow.
     ```
   - If status = ✅ Working or 📦 Installed → BLOCK with message:
     ```
     [PluginName] is already complete.
     Use /improve [PluginName] to make changes.
     ```

3. Check for creative brief:

```bash
test -f "plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
```

If missing, offer:

```
No creative brief found for [PluginName].

Would you like to:
1. Create one now (/dream [PluginName]) (recommended)
2. Continue without brief (not recommended)

Choose (1-2): _
```

If user chooses 1, exit and instruct them to run `/dream [PluginName]`.

---

## Stage Dispatcher

**Purpose:** Route to correct stage implementation based on current state.

**Entry point:** Called by /implement command or when resuming workflow.

**Implementation:**

1. Determine current stage:

```bash
# Check if handoff file exists (resuming)
if [ -f "plugins/${PLUGIN_NAME}/.continue-here.md" ]; then
    # Parse stage from handoff YAML frontmatter
    CURRENT_STAGE=$(grep "^stage:" plugins/${PLUGIN_NAME}/.continue-here.md | awk '{print $2}')
    echo "Resuming from Stage ${CURRENT_STAGE}"
else
    # Starting new workflow
    CURRENT_STAGE=0
    echo "Starting workflow at Stage 0"
fi
```

2. Verify preconditions for target stage:

```javascript
function checkStagePreconditions(pluginName, stage) {
  // Stage 0: Just needs creative brief (already checked)
  if (stage === 0) {
    return { allowed: true }
  }

  // Stage 1: Requires architecture.md + parameter-spec.md contracts
  if (stage === 1) {
    const architectureExists = fileExists(`plugins/${pluginName}/.ideas/architecture.md`)
    const paramSpecExists = fileExists(`plugins/${pluginName}/.ideas/parameter-spec.md`)

    if (!architectureExists) {
      return {
        allowed: false,
        reason: "Stage 0 architecture must complete before Stage 1",
        action: "Complete Stage 0 first"
      }
    }

    if (!paramSpecExists || !architectureExists) {
      return {
        allowed: false,
        reason: "Cannot proceed to Stage 1 - missing implementation contracts",
        action: "Create parameter-spec.md (via ui-mockup) and architecture.md (via Stage 0)",
        contracts: {
          "parameter-spec.md": paramSpecExists,
          "architecture.md": architectureExists
        }
      }
    }

    return { allowed: true }
  }

  // Stages 2-6: Require previous stage complete
  if (stage >= 2 && stage <= 6) {
    const status = getPluginStatus(pluginName)
    const expectedPrevious = stage - 1

    // Check PLUGINS.md shows previous stage complete
    if (!status.includes(`Stage ${expectedPrevious}`) &&
        !status.includes('complete')) {
      return {
        allowed: false,
        reason: `Stage ${expectedPrevious} must complete before Stage ${stage}`,
        action: `Complete Stage ${expectedPrevious} or use /continue to resume`
      }
    }

    return { allowed: true }
  }

  return { allowed: false, reason: "Invalid stage number" }
}
```

3. Route to stage implementation:

```javascript
function dispatchStage(pluginName, stageNumber) {
  // Check preconditions
  const preconditionCheck = checkStagePreconditions(pluginName, stageNumber)

  if (!preconditionCheck.allowed) {
    // Display blocking message
    console.log(`✗ BLOCKED: ${preconditionCheck.reason}`)
    console.log(`Action: ${preconditionCheck.action}`)

    if (preconditionCheck.contracts) {
      console.log("\nContract status:")
      for (const [name, exists] of Object.entries(preconditionCheck.contracts)) {
        console.log(`${exists ? '✓' : '✗'} ${name}`)
      }
    }

    return { status: 'blocked', reason: preconditionCheck.reason }
  }

  // Route to stage
  switch(stageNumber) {
    case 0:
      return executeStage0Research(pluginName)
    case 1:
      return executeStage1Planning(pluginName)
    case 2:
      return executeStage2Foundation(pluginName)
    case 3:
      return executeStage3Shell(pluginName)
    case 4:
      return executeStage4DSP(pluginName)
    case 5:
      return executeStage5GUI(pluginName)
    case 6:
      return executeStage6Validation(pluginName)
    default:
      return { status: 'error', reason: `Invalid stage: ${stageNumber}` }
  }
}
```

4. Stage loop for continuous execution:

```javascript
function runWorkflow(pluginName, startStage = 0) {
  let currentStage = startStage
  let shouldContinue = true

  while (shouldContinue && currentStage <= 6) {
    console.log(`\n━━━ Stage ${currentStage} ━━━\n`)

    // Execute stage
    const result = dispatchStage(pluginName, currentStage)

    if (result.status === 'blocked') {
      console.log("\nWorkflow blocked. Resolve issues and resume with /continue.")
      return result
    }

    if (result.status === 'error') {
      console.log(`\nError: ${result.reason}`)
      return result
    }

    // Present decision menu
    const choice = presentDecisionMenu({
      stage: currentStage,
      completionStatement: result.completionStatement,
      pluginName: pluginName
    })

    // Handle user choice
    if (choice === 'continue' || choice === 1) {
      currentStage++
    } else if (choice === 'pause') {
      console.log("\n✓ Workflow paused. Resume anytime with /continue")
      shouldContinue = false
    } else {
      // Handle other menu options (review, test, etc.)
      handleMenuChoice(choice, pluginName, currentStage)
      // Re-present menu after handling
    }
  }

  if (currentStage > 6) {
    console.log("\n✓ All stages complete!")
  }
}
```

**Usage:**

```javascript
// From /implement command:
runWorkflow(pluginName, 0)

// From /continue command (via context-resume):
const handoff = readHandoffFile(pluginName)
const resumeStage = handoff.stage
runWorkflow(pluginName, resumeStage)
```

---

## Stage 0: Research

**Goal:** Understand what we're building before writing code

**Duration:** 5-10 minutes

**Model Configuration:**
- Model: Opus (complex reasoning for algorithm comparison)
- Extended thinking: ENABLED
- Budget: 10000 tokens

**Actions:**

1. Read creative brief:

```bash
cat plugins/[PluginName]/.ideas/creative-brief.md
```

2. Identify plugin type and technical approach:

   - Audio effect, MIDI effect, synthesizer, or utility?
   - Input/output configuration (mono, stereo, sidechain)
   - Processing type (time-domain, frequency-domain, granular)

3. **Context7 JUCE Documentation Lookup** (CRITICAL):

   a. Resolve JUCE library ID:
   ```
   Use: mcp__context7__resolve-library-id("JUCE")
   Returns: Context7-compatible library ID (e.g., "/juce-framework/JUCE")
   ```

   b. Get JUCE DSP documentation:
   ```
   Use: mcp__context7__get-library-docs(libraryID, topic="dsp modules", tokens=5000)
   Extract: Relevant juce::dsp classes for identified plugin type
   ```

   c. Document JUCE modules found:
   - List specific juce::dsp classes (e.g., juce::dsp::Gain, juce::dsp::IIR::Filter)
   - Note Context7 library references
   - Identify any missing algorithms (need custom DSP)

4. Research professional plugin examples:

   - Search web for industry leaders (FabFilter, Waves, UAD, etc.)
   - Document 3-5 similar plugins
   - Note sonic characteristics
   - Extract typical parameter ranges used

5. Research parameter ranges:

   - Industry-standard ranges for plugin type
   - Typical defaults (reference professional plugins)
   - Skew factors for nonlinear ranges

6. Check design sync (if mockup exists):
   - Look for `plugins/[Name]/.ideas/mockups/v*-ui.yaml`
   - If exists: Compare mockup parameters with creative brief
   - If conflicts: Invoke design-sync skill
   - Document sync results

**Output:** Create `plugins/[PluginName]/.ideas/architecture.md` (DSP specification)

**CRITICAL:** Use the DSP architecture contract template from `assets/architecture.md`.

**Required sections:**
1. Title: `# DSP Architecture: [PluginName]`
2. Contract header: `**CRITICAL CONTRACT:** This specification is immutable...`
3. `## Core Components` - List each DSP component with JUCE class, purpose, parameters affected, configuration
4. `## Processing Chain` - ASCII diagram showing signal flow and parameter connections
5. `## Parameter Mapping` - Table mapping every parameter ID to DSP component and usage
6. `## Algorithm Details` - Implementation approach for each algorithm
7. `## Special Considerations` - Thread safety, performance, denormal protection, sample rate handling
8. `## Research References` - Context7 docs, professional plugins, technical resources

**How to populate from research:**

- **Similar Plugins research** → `## Research References` (Professional Examples section)
- **JUCE modules identified** → `## Core Components` (one subsection per component with structured format)
- **Parameter research** → `## Parameter Mapping` (table linking parameter IDs to components)
- **Technical approach** → `## Processing Chain` (ASCII diagram) + `## Algorithm Details`
- **Technical feasibility** → `## Special Considerations`

**Example Core Component entry:**
```markdown
### File Manager
- **JUCE Class:** `juce::AudioFormatManager`
- **Purpose:** Detect audio file formats and create readers
- **Parameters Affected:** station
- **Configuration:** registerBasicFormats() for WAV/AIFF/MP3/FLAC/OGG
```

**Example Processing Chain:**
```
MIDI In → Trigger Detection → Set Playback Position
                                        ↓
File Manager ← station parameter → AudioFormatReader
      ↓
Resampler ← speed_pitch parameter
      ↓
Volume Control ← volume parameter
      ↓
Output
```

See `assets/architecture.md` for complete template structure.

**Create handoff file:** `plugins/[PluginName]/.continue-here.md`

**Format:**

```yaml
---
plugin: [PluginName]
stage: 0
status: in_progress
last_updated: [YYYY-MM-DD HH:MM:SS]
---

# Resume Point

## Current State: Stage 0 - Research

Research phase complete. Ready to proceed to planning.

## Completed So Far

**Stage 0:** ✓ Complete
- Plugin type defined
- Professional examples researched
- DSP feasibility verified
- Parameter ranges researched

## Next Steps

1. Stage 1: Planning (calculate complexity, create implementation plan)
2. Review research findings
3. Pause here

## Context to Preserve

**Plugin Type:** [Effect/Synth/Utility]
**Processing:** [Approach]
**JUCE Modules:** [List]

**Files Created:**
- plugins/[PluginName]/.ideas/architecture.md
```

**Update PLUGINS.md:**

1. Check if entry exists:
```bash
grep "^### ${PLUGIN_NAME}$" PLUGINS.md
```

2. If NOT found, create initial entry:

Use Edit tool to add to PLUGINS.md:
```markdown
### [PluginName]
**Status:** 💡 Ideated
**Type:** [Audio Effect | MIDI Instrument | Synth]
**Created:** [YYYY-MM-DD]

[Brief description from creative-brief.md]

**Lifecycle Timeline:**
- **[YYYY-MM-DD]:** Creative brief created

**Last Updated:** [YYYY-MM-DD]
```

3. Update status to Stage 0:

Use Edit tool to change:
```markdown
**Status:** 💡 Ideated → **Status:** 🚧 Stage 0
```

4. Add timeline entry:

Use Edit tool to append to Lifecycle Timeline:
```markdown
- **[YYYY-MM-DD] (Stage 0):** Research completed
```

5. Update Last Updated field

**Git commit:**

```bash
git add plugins/[PluginName]/.ideas/architecture.md plugins/[PluginName]/.continue-here.md PLUGINS.md
git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 0 - research complete

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

Display commit hash:
```bash
git log -1 --format='✓ Committed: %h - Stage 0 complete'
```

**Decision menu:**

Present inline numbered list (NOT AskUserQuestion):

```
✓ Stage 0 complete: research documented

What's next?
1. Continue to Stage 1 (recommended)
2. Review research findings
3. Improve creative brief based on research
4. Run deeper investigation (deep-research skill) ← Discover JUCE troubleshooting
5. Pause here
6. Other

Choose (1-6): _
```

Wait for user response. Handle:
- Number (1-6): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 5
- "review" keyword: Execute option 2
- "other": Ask "What would you like to do?" then re-present menu

---

## Stage 1: Planning

**Goal:** Analyze complexity and create implementation plan

**Duration:** 2-5 minutes

**Model Configuration:**
- Model: Sonnet (deterministic planning)
- Extended thinking: DISABLED

**Preconditions:**

1. Stage 0 must be complete (architecture.md exists)
2. PLUGINS.md shows `🚧 Stage 0` or similar

**Contract Prerequisites (CRITICAL - BLOCKING):**

Check for required contract files:

```bash
test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" && echo "✓ parameter-spec.md" || echo "✗ parameter-spec.md MISSING"
test -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" && echo "✓ architecture.md" || echo "✗ architecture.md MISSING"
```

**If parameter-spec.md OR architecture.md is missing:**

STOP IMMEDIATELY and BLOCK with this exact message:

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
   - Use architecture.md DSP specification (JUCE modules and processing chain)
   - Document DSP components and processing chain
   - Map parameters to DSP components
   - Save to plugins/[PluginName]/.ideas/architecture.md

Once both contracts exist, Stage 1 will proceed.
```

DO NOT PROCEED PAST THIS POINT if contracts are missing.

Exit skill and wait for user to create contracts.

**Actions (contracts confirmed present):**

1. Read all contracts:

```bash
# Read parameter specification
cat plugins/[PluginName]/.ideas/parameter-spec.md

# Read DSP architecture specification
cat plugins/[PluginName]/.ideas/architecture.md
```

2. Calculate complexity score:

**Formula:**
```
score = min(param_count / 5, 2.0) + algorithm_count + feature_count
Cap at 5.0
```

**Extract metrics:**

From parameter-spec.md:
- Count parameters (each parameter definition = 1)
- param_score = min(param_count / 5, 2.0)

From architecture.md:
- Count distinct DSP algorithms/components
- algorithm_count = number of juce::dsp classes or custom algorithms

From architecture.md (identify features):
- Feedback loops present? (+1)
- FFT/frequency domain processing? (+1)
- Multiband processing? (+1)
- Modulation systems (LFO, envelope)? (+1)
- External MIDI control? (+1)
- feature_count = sum of above

**Calculate:**
```
total_score = param_score + algorithm_count + feature_count
final_score = min(total_score, 5.0)
```

**Show breakdown:**
```
Complexity Calculation:
- Parameters: [N] parameters ([N/5] points, capped at 2.0) = [X.X]
- Algorithms: [N] DSP components = [N]
- Features: [List] = [N]
Total: [X.X] / 5.0
```

3. Determine implementation strategy:

   - **Simple (score ≤ 2.0):** Single-pass implementation
   - **Complex (score ≥ 3.0):** Phase-based implementation with staged commits

4. For complex plugins (score ≥ 3), create phases:

**Stage 4 (DSP) phases:**

- Phase 4.1: Core processing (essential audio path)
- Phase 4.2: Parameter modulation (APVTS integration)
- Phase 4.3: Advanced features (if applicable)

**Stage 5 (GUI) phases:**

- Phase 5.1: Layout and basic controls
- Phase 5.2: Advanced UI elements
- Phase 5.3: Polish and styling (if applicable)

Each phase needs:

- Description of what gets implemented
- Test criteria to verify completion
- Estimated duration

**Output:** Create `plugins/[PluginName]/.ideas/plan.md`

**Format for simple plugins:**

```markdown
# [PluginName] - Implementation Plan

**Date:** [YYYY-MM-DD]
**Complexity Score:** [X.X] (Simple)
**Strategy:** Single-pass implementation

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ← Current
- Stage 2: Foundation
- Stage 3: Shell
- Stage 4: DSP
- Stage 5: GUI
- Stage 6: Validation

## Estimated Duration

Total: ~[X] minutes

- Stage 2: 5 min
- Stage 3: 5 min
- Stage 4: [X] min
- Stage 5: [X] min
- Stage 6: 15 min
```

**Format for complex plugins:**

```markdown
# [PluginName] - Implementation Plan

**Date:** [YYYY-MM-DD]
**Complexity Score:** [X.X] (Complex)
**Strategy:** Phase-based implementation

## Complexity Factors

- Parameters: [N] ([N/5] points)
- Algorithms: [N] ([N] points)
- Features: [List] ([N] points)

## Stage 4: DSP Phases

### Phase 4.1: Core Processing

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

### Phase 4.2: Parameter Modulation

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

### Phase 4.3: Advanced Features

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

## Stage 5: GUI Phases

### Phase 5.1: Layout and Basic Controls

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

### Phase 5.2: Advanced UI Elements

**Goal:** [Description]
**Test Criteria:**

- [ ] [Criterion 1]
- [ ] [Criterion 2]
      **Duration:** [X] min

## Estimated Duration

Total: ~[X] hours

- Stage 2: 5 min
- Stage 3: 5 min
- Stage 4: [X] min (phased)
- Stage 5: [X] min (phased)
- Stage 6: 20 min
```

**Update .continue-here.md:**

Use Edit tool to update YAML frontmatter:

```yaml
---
plugin: [PluginName]
stage: 1
status: complete
last_updated: [YYYY-MM-DD HH:MM:SS]
complexity_score: [X.X]
phased_implementation: [true/false]
---
```

Update markdown sections:
- Current State: "Stage 1 - Planning complete"
- Completed So Far: Add Stage 1 details
- Next Steps: List Stage 2 actions
- Context to Preserve: Add complexity score, strategy

**Update PLUGINS.md:**

1. Update status:

Use Edit tool to change:
```markdown
**Status:** 🚧 Stage 0 → **Status:** 🚧 Stage 1
```

2. Add timeline entry:

Use Edit tool to append to Lifecycle Timeline:
```markdown
- **[YYYY-MM-DD] (Stage 1):** Planning - Complexity [X.X]
```

3. Update Last Updated field

**Git commit:**

```bash
git add plugins/[PluginName]/.ideas/plan.md plugins/[PluginName]/.continue-here.md PLUGINS.md
git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 1 - planning complete

Complexity: [X.X]
Strategy: [Single-pass | Phased implementation]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

Display commit hash:
```bash
git log -1 --format='✓ Committed: %h - Stage 1 complete'
```

**Decision menu:**

Present inline numbered list (NOT AskUserQuestion):

```
✓ Stage 1 complete: plan created (Complexity [X.X], [single-pass/phased])

What's next?
1. Continue to Stage 2 (recommended)
2. Review plan details
3. Adjust complexity assessment
4. Review contracts (parameter-spec, architecture)
5. Pause here
6. Other

Choose (1-6): _
```

Wait for user response. Handle:
- Number (1-6): Execute corresponding option
- "continue" keyword: Execute option 1
- "pause" keyword: Execute option 5
- "review" keyword: Execute option 2
- "other": Ask "What would you like to do?" then re-present menu

---

## Stage 2: Foundation

**Goal:** Create plugin structure that compiles

**Duration:** 5 minutes

**Preconditions:**
- Stage 1 complete (plan.md exists)
- creative-brief.md exists
- architecture.md exists

**Actions:**

### 1. Prepare Contracts for Subagent

Read contract files that foundation-agent needs:

```bash
cat plugins/[PluginName]/.ideas/creative-brief.md
cat plugins/[PluginName]/.ideas/architecture.md
cat plugins/[PluginName]/.ideas/plan.md
```

### 2. Invoke foundation-agent via Task Tool

Call foundation-agent subagent with complete specification:

```typescript
const foundationResult = Task({
  subagent_type: "foundation-agent",
  model: "sonnet",
  description: `Create build system for ${pluginName}`,
  prompt: `
You are foundation-agent. Your task is to create the JUCE plugin build system and verify compilation for ${pluginName}.

**Plugin Name:** ${pluginName}
**Plugin Location:** plugins/${pluginName}/

**Contract Files:**

creative-brief.md:
\`\`\`
${creativeBriefContent}
\`\`\`

architecture.md:
\`\`\`
${architectureContent}
\`\`\`

plan.md:
\`\`\`
${planContent}
\`\`\`

Follow the instructions in .claude/agents/foundation-agent.md exactly.

Extract PRODUCT_NAME from creative-brief, determine plugin type from architecture, create CMakeLists.txt, create minimal PluginProcessor and PluginEditor classes, build with --no-install flag.

Return JSON report in the exact format specified in foundation-agent.md.
  `
})
```

### 3. Parse JSON Report with Error Handling

**Implement robust JSON parsing:**

```typescript
function parseSubagentReport(rawOutput: string): object | null {
  try {
    // Strategy 1: Extract from markdown code blocks
    const codeBlockMatch = rawOutput.match(/```json\n([\s\S]*?)\n```/)
    if (codeBlockMatch) {
      return JSON.parse(codeBlockMatch[1])
    }

    // Strategy 2: Find JSON object via brace matching
    const braceStart = rawOutput.indexOf('{')
    const braceEnd = rawOutput.lastIndexOf('}')
    if (braceStart !== -1 && braceEnd !== -1 && braceEnd > braceStart) {
      const jsonCandidate = rawOutput.substring(braceStart, braceEnd + 1)
      return JSON.parse(jsonCandidate)
    }

    // Strategy 3: Try parsing entire output
    return JSON.parse(rawOutput)
  } catch (error) {
    console.error("JSON parsing failed:", error)
    return null
  }
}

const report = parseSubagentReport(foundationResult)

if (!report) {
  // JSON parsing failed - present error to user
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 2 Error: Could not parse foundation-agent report
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

The foundation-agent subagent completed but returned malformed output.

Raw output:
${foundationResult}

What would you like to do?
1. Retry foundation-agent dispatch
2. Show full subagent output
3. Report bug (subagent should return valid JSON)
4. Manual intervention (I'll fix and say "resume automation")

Choose (1-4): _
  `)

  // Wait for user response, handle accordingly
  return
}
```

### 4. Validate Required Fields

**Check JSON structure:**

```typescript
function validateFoundationReport(report: any): { valid: boolean; error?: string } {
  if (!report.agent || report.agent !== "foundation-agent") {
    return { valid: false, error: "Missing or wrong 'agent' field" }
  }

  if (!report.status || !["success", "failure"].includes(report.status)) {
    return { valid: false, error: "Missing or invalid 'status' field" }
  }

  if (!report.outputs || typeof report.outputs !== 'object') {
    return { valid: false, error: "Missing 'outputs' object" }
  }

  if (!report.hasOwnProperty('ready_for_next_stage')) {
    return { valid: false, error: "Missing 'ready_for_next_stage' field" }
  }

  return { valid: true }
}

const validation = validateFoundationReport(report)
if (!validation.valid) {
  console.log(`
✗ Invalid report format: ${validation.error}

Report received:
${JSON.stringify(report, null, 2)}

What would you like to do?
1. Retry with fresh foundation-agent
2. Report bug (malformed JSON structure)
3. Manual intervention

Choose (1-3): _
  `)
  return
}
```

### 5. Handle foundation-agent Success/Failure

**If status="success" (files created):**

```typescript
if (report.status === "success" && report.ready_for_next_stage) {
  console.log(`✓ foundation-agent complete: Source files created`)
  console.log(`  - Plugin: ${report.outputs.plugin_name}`)
  console.log(`  - Files: ${report.outputs.source_files_created?.length || 0}`)

  // Now invoke build-automation to verify compilation
  console.log(`\nInvoking build-automation to verify compilation...`)

  // Proceed to section 5a (Build Verification)
}
```

**If status="failure" (file creation failed):**

```typescript
if (report.status === "failure") {
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 2 Failed: File Creation Error
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Error: ${report.outputs.error_message || "Unknown error"}
Type: ${report.outputs.error_type || "N/A"}

Issues:
${report.issues.map(issue => `  - ${issue}`).join('\n')}

What would you like to do?
1. Show me the code (display files that were created)
2. Retry foundation-agent
3. I'll fix it manually (say "resume automation" when ready)

Choose (1-3): _
  `)

  // Wait for user response
  const choice = getUserInput()

  if (choice === "1" || choice === "show code") {
    // Display generated files
    const files = [
      'CMakeLists.txt',
      'Source/PluginProcessor.h',
      'Source/PluginProcessor.cpp',
      'Source/PluginEditor.h',
      'Source/PluginEditor.cpp'
    ]

    for (const file of files) {
      const path = `plugins/${pluginName}/${file}`
      if (fileExists(path)) {
        console.log(`\n=== ${file} ===`)
        console.log(readFile(path))
      } else {
        console.log(`\n✗ ${file} - NOT FOUND`)
      }
    }

    // Re-present menu
  }

  if (choice === "2" || choice === "retry") {
    console.log("Retrying foundation-agent...")
    // Return to step 2 (Invoke foundation-agent)
  }

  if (choice === "3" || choice === "manual") {
    console.log("\n⏸ Paused for manual intervention.")
    console.log("Fix the issue, then say 'resume automation' to continue.")
    return  // Exit, wait for user to say "resume automation"
  }
}
```

### 5a. Build Verification via build-automation

**After foundation-agent succeeds, invoke build-automation skill:**

```typescript
console.log("Invoking build-automation skill with --no-install flag...")

// build-automation skill handles the entire build process
// It will invoke scripts/build-and-install.sh --no-install [PluginName]
// and present appropriate menus on success or failure

Skill({
  skill: "build-automation",
  context: {
    plugin_name: pluginName,
    stage: 2,
    flags: ["--no-install"],
    invoking_skill: "plugin-workflow",
    purpose: "Verify Stage 2 foundation compiles successfully"
  }
})
```

**build-automation will:**
1. Run build script with --no-install flag
2. If build succeeds: Display success message, return control to plugin-workflow
3. If build fails: Present 4-option failure protocol:
   - Investigate (troubleshooter)
   - Show build log
   - Show code
   - Wait for manual fix

**After build-automation returns success:**

```typescript
console.log(`
✓ Stage 2 build verification complete
  - Compilation: Successful
  - Artifacts: VST3, AU, Standalone (not installed)
`)

// Continue to stage completion workflow (section 6-8)
```

**Note:** Build failures are now handled entirely by build-automation skill. plugin-workflow does NOT need to handle build troubleshooting - build-automation provides comprehensive failure protocol with troubleshooter integration.

### 6. Update State Files

**Update .continue-here.md:**

```typescript
updateHandoff(
  pluginName,
  2,  // stage
  "Stage 2: Foundation - Build system created, compilation verified",
  ["Stage 3: Implement parameters", "Review build artifacts", "Test compilation"],
  complexityScore,
  phased
)
```

**Update PLUGINS.md:**

```typescript
updatePluginStatus(pluginName, "🚧 Stage 2")
updatePluginTimeline(pluginName, 2, "Foundation complete - build system operational")
```

### 7. Git Commit

**Atomic commit of all Stage 2 changes:**

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/CMakeLists.txt
git add plugins/[PluginName]/.continue-here.md
git add PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 2 - foundation

Build system created with JUCE 8 configuration
Minimal PluginProcessor and PluginEditor classes
Compilation verified (VST3, AU, Standalone)

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

Display commit hash:
```bash
git log -1 --format='✓ Committed: %h - Stage 2 complete'
```

### 8. Decision Menu

**Present decision menu:**

```
✓ Stage 2 complete: Build system operational

Plugin: [PluginName]
Build artifacts: VST3, AU, Standalone
Status: Compiles successfully (no DSP yet)

What's next?
1. Continue to Stage 3 (implement parameters) (recommended)
2. Review build artifacts
3. Test compilation manually
4. Review Stage 2 code
5. Pause here
6. Other

Choose (1-6): _
```

**Handle responses:**
- 1 or "continue": Proceed to Stage 3
- 2: Show build artifacts list
- 3: Provide manual test instructions
- 4: Display CMakeLists.txt and source files
- 5 or "pause": Update handoff, exit
- 6 or "other": Ask "What would you like to do?" then re-present menu

---

## Stage 3: Shell

**Goal:** Implement ALL parameters, plugin loads in DAW with parameter system

**Duration:** 5 minutes

**Preconditions:**
- Stage 2 complete (build system operational)
- parameter-spec.md exists (from finalized UI mockup)

**Actions:**

### 1. Precondition Check: parameter-spec.md REQUIRED

**BLOCKING check before proceeding:**

```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" ]; then
  echo "
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ BLOCKED: Cannot proceed to Stage 3
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Missing required contract: parameter-spec.md

Stage 3 requires a complete parameter specification to implement the APVTS.
This contract is the single source of truth for all plugin parameters.

WHY BLOCKED:
Without parameter-spec.md, shell-agent cannot know:
- Which parameters to create
- Parameter types (Float/Bool/Choice)
- Parameter ranges and defaults
- Parameter IDs for UI binding

HOW TO UNBLOCK:
1. Complete UI mockup workflow (/mockup)
2. Design UI with parameter controls
3. Finalize a design version (Phase 4.5)
4. Finalization auto-generates parameter-spec.md
5. Then resume Stage 3 with /continue ${PLUGIN_NAME}

Current status: parameter-spec.md NOT FOUND

Cannot proceed without this contract.
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  "
  exit 1
fi
```

**If parameter-spec.md missing:** STOP IMMEDIATELY, exit skill, wait for user to create contract.

### 2. Prepare Contracts for Subagent

Read contract files that shell-agent needs:

```bash
cat plugins/[PluginName]/.ideas/parameter-spec.md
cat plugins/[PluginName]/.ideas/creative-brief.md
cat plugins/[PluginName]/.ideas/architecture.md
```

### 3. Invoke shell-agent via Task Tool

Call shell-agent subagent with complete specification:

```typescript
const shellResult = Task({
  subagent_type: "shell-agent",
  model: "sonnet",
  description: `Implement parameters for ${pluginName}`,
  prompt: `
You are shell-agent. Your task is to implement ALL parameters from parameter-spec.md for ${pluginName}.

**Plugin Name:** ${pluginName}
**Plugin Location:** plugins/${pluginName}/

**Contract Files:**

parameter-spec.md:
\`\`\`
${parameterSpecContent}
\`\`\`

creative-brief.md:
\`\`\`
${creativeBriefContent}
\`\`\`

architecture.md:
\`\`\`
${architectureContent}
\`\`\`

Follow the instructions in .claude/agents/shell-agent.md exactly.

Create APVTS with ALL parameters from parameter-spec.md, implement state management, update processBlock stub.

Build verification handled by plugin-workflow after agent completes.

CRITICAL: Use JUCE 8 ParameterID format: juce::ParameterID { "id", 1 }

Return JSON report in the exact format specified in shell-agent.md.
  `
})
```

### 4. Parse JSON Report (Same as Stage 2)

Use same `parseSubagentReport()` function with robust error handling.

### 5. Validate shell-agent Report

```typescript
function validateShellReport(report: any): { valid: boolean; error?: string } {
  if (!report.agent || report.agent !== "shell-agent") {
    return { valid: false, error: "Wrong agent (expected shell-agent)" }
  }

  if (!report.status || !["success", "failure"].includes(report.status)) {
    return { valid: false, error: "Invalid status" }
  }

  if (report.status === "success") {
    if (!report.outputs.parameters_implemented || !Array.isArray(report.outputs.parameters_implemented)) {
      return { valid: false, error: "Missing parameters_implemented array" }
    }

    if (!report.outputs.parameter_count || typeof report.outputs.parameter_count !== 'number') {
      return { valid: false, error: "Missing parameter_count" }
    }
  }

  return { valid: true }
}
```

### 6. Handle Success/Failure

**If status="success":**

```typescript
if (report.status === "success" && report.ready_for_next_stage) {
  console.log(`✓ Stage 3 complete: Parameters implemented`)
  console.log(`  - Plugin: ${report.outputs.plugin_name}`)
  console.log(`  - Parameters: ${report.outputs.parameter_count}`)
  console.log(`  - APVTS created: ${report.outputs.apvts_created ? 'Yes' : 'No'}`)

  // List parameters
  console.log(`\n  Parameters implemented:`)
  report.outputs.parameters_implemented.forEach(param => {
    console.log(`    - ${param.id} (${param.type}): ${param.range || param.default}`)
  })

  // Continue to stage completion workflow
}
```

**If status="failure":**

Present 4-option failure menu same as Stage 2, with shell-agent specific context:

```typescript
if (report.status === "failure") {
  // If error_type is "parameter_mismatch" - contract violation
  if (report.outputs.error_type === "parameter_mismatch") {
    console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 3 Failed: Parameter Contract Violation
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Error: Not all parameters from parameter-spec.md were implemented

Expected parameters: ${report.outputs.expected_count}
Implemented: ${report.outputs.actual_count}
Missing: ${report.outputs.missing_parameters.join(', ')}

This is a ZERO-DRIFT violation. All parameters from the spec must be implemented.

What would you like to do?
1. Investigate (why were parameters missing?)
2. Show me the code (PluginProcessor.cpp)
3. Show me the parameter-spec.md
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
    `)
  } else {
    // Other error types (compilation, build failure, etc.)
    console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 3 Failed: Parameter Implementation
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Error: ${report.outputs.error_message}
Type: ${report.outputs.error_type}

Issues:
${report.issues.map(i => `  - ${i}`).join('\n')}

What would you like to do?
1. Investigate
2. Show me the code
3. Show me the build output
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
    `)
  }

  // Handle choice (same pattern as Stage 2)
}
```

### 7. Update State Files

```typescript
updateHandoff(
  pluginName,
  3,  // stage
  `Stage 3: Shell - ${report.outputs.parameter_count} parameters implemented`,
  ["Stage 4: Implement DSP", "Test parameters in DAW", "Review APVTS code"],
  complexityScore,
  phased
)

updatePluginStatus(pluginName, "🚧 Stage 3")
updatePluginTimeline(pluginName, 3, `Shell complete - ${report.outputs.parameter_count} parameters`)
```

### 8. Git Commit

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/.continue-here.md
git add PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: [PluginName] Stage 3 - shell

Implemented [N] parameters with APVTS
State management (save/load) implemented
Plugin loads in DAW with parameter system

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
```

### 9. Decision Menu

```
✓ Stage 3 complete: Parameter system operational

Plugin: [PluginName]
Parameters: [N] parameters implemented
APVTS: Created and functional
Status: Plugin loads in DAW (no audio processing yet)

What's next?
1. Continue to Stage 4 (implement DSP) (recommended)
2. Test parameters in DAW
3. Review parameter code
4. Show parameter list
5. Pause here
6. Other

Choose (1-6): _
```

---

## Stage 4: DSP

**Goal:** Implement audio processing, parameters control DSP

**Duration:** 15-45 minutes (depending on complexity)

**Preconditions:**
- Stage 3 complete (parameters implemented)
- architecture.md exists (DSP component specifications)
- plan.md exists (complexity score, phase breakdown)

**Actions:**

### 1. Read Complexity Score from plan.md

Determine if phased implementation is required:

```typescript
const planContent = readFile(`plugins/${pluginName}/.ideas/plan.md`)

// Extract complexity score
const complexityMatch = planContent.match(/Complexity Score:\*\*\s*([\d.]+)/)
const complexityScore = complexityMatch ? parseFloat(complexityMatch[1]) : 0

// Check if plan specifies phases
const hasPhases = planContent.includes("### Phase 4.1") || planContent.includes("## Stage 4: DSP Phases")

console.log(`Complexity: ${complexityScore} (${hasPhases ? 'phased' : 'single-pass'})`)
```

### 2a. Single-Pass Implementation (Complexity ≤2)

**If complexity ≤2 OR no phases defined:**

Invoke dsp-agent once for complete DSP implementation:

```typescript
const dspResult = Task({
  subagent_type: "dsp-agent",
  model: "sonnet",  // Sonnet for simple DSP
  description: `Implement DSP for ${pluginName}`,
  prompt: `
You are dsp-agent. Your task is to implement audio processing for ${pluginName}.

**Plugin Name:** ${pluginName}
**Plugin Location:** plugins/${pluginName}/
**Complexity:** ${complexityScore} (single-pass implementation)

**Contract Files:**

architecture.md:
\`\`\`
${architectureContent}
\`\`\`

parameter-spec.md:
\`\`\`
${parameterSpecContent}
\`\`\`

plan.md:
\`\`\`
${planContent}
\`\`\`

Follow the instructions in .claude/agents/dsp-agent.md exactly.

Implement all DSP components from architecture.md in processBlock(), connect all parameters, ensure real-time safety (no allocations, use juce::ScopedNoDenormals).

Build verification handled by plugin-workflow after agent completes.

Return JSON report in the exact format specified in dsp-agent.md.
  `
})

// Parse and validate report (same as Stage 2/3)
const report = parseSubagentReport(dspResult)

if (report.status === "success") {
  console.log(`✓ Stage 4 complete: DSP implemented`)
  console.log(`  - Components: ${report.outputs.dsp_components_implemented.length}`)
  console.log(`  - Parameters connected: ${report.outputs.parameters_connected.length}`)
  console.log(`  - Real-time safe: ${report.outputs.real_time_safe ? 'Yes' : 'No'}`)

  // Continue to auto-test (Step 5)
}
```

### 2b. Phased Implementation (Complexity ≥3)

**If complexity ≥3 AND plan.md defines phases:**

Parse phase breakdown from plan.md:

```typescript
// Extract phases from plan.md
const phasePattern = /### Phase (4\.\d+):\s*(.+?)\n/g
const phases = []
let match

while ((match = phasePattern.exec(planContent)) !== null) {
  phases.push({
    number: match[1],  // e.g., "4.1"
    description: match[2]  // e.g., "Core Processing"
  })
}

console.log(`Stage 4 will execute in ${phases.length} phases:`)
phases.forEach(phase => {
  console.log(`  - Phase ${phase.number}: ${phase.description}`)
})
```

**Execute each phase sequentially:**

```typescript
for (let i = 0; i < phases.length; i++) {
  const phase = phases[i]

  console.log(`\n━━━ Stage ${phase.number} - ${phase.description} ━━━\n`)

  // Determine model based on complexity
  const model = complexityScore >= 4 ? "opus" : "sonnet"

  const phaseResult = Task({
    subagent_type: "dsp-agent",
    model: model,  // Opus for complexity ≥4
    description: `Implement DSP Phase ${phase.number} for ${pluginName}`,
    prompt: `
You are dsp-agent. Your task is to implement Phase ${phase.number} of DSP for ${pluginName}.

**Plugin Name:** ${pluginName}
**Plugin Location:** plugins/${pluginName}/
**Complexity:** ${complexityScore}
**Current Phase:** ${phase.number} - ${phase.description}
**Total Phases:** ${phases.length}

**Contract Files:**

architecture.md:
\`\`\`
${architectureContent}
\`\`\`

parameter-spec.md:
\`\`\`
${parameterSpecContent}
\`\`\`

plan.md (Phase ${phase.number} section):
\`\`\`
${extractPhaseSection(planContent, phase.number)}
\`\`\`

Follow the instructions in .claude/agents/dsp-agent.md exactly.

Implement ONLY the components specified for Phase ${phase.number}. Build on existing code from previous phases. Ensure real-time safety.

After completion, update plan.md with phase completion timestamp.

Return JSON report with phase_completed: "${phase.number}".
    `
  })

  const phaseReport = parseSubagentReport(phaseResult)

  if (!phaseReport || phaseReport.status === "failure") {
    console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage ${phase.number} Failed
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Phase ${phase.number} (${phase.description}) failed.

Error: ${phaseReport?.outputs.error_message || "Unknown error"}

What would you like to do?
1. Investigate
2. Show me the code
3. Show me the build output
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
    `)

    // Handle failure (same 4-option menu as Stage 2/3)
    return  // Stop phased execution on failure
  }

  // Phase succeeded
  console.log(`✓ Phase ${phase.number} complete: ${phase.description}`)
  console.log(`  - Components: ${phaseReport.outputs.components_this_phase?.join(', ') || 'N/A'}`)

  // Git commit for this phase
  await bash(`
git add plugins/${pluginName}/Source/
git add plugins/${pluginName}/.ideas/plan.md
git add plugins/${pluginName}/.continue-here.md

git commit -m "$(cat <<'EOF'
feat: ${pluginName} Stage ${phase.number} - ${phase.description}

Phase ${i + 1} of ${phases.length} complete
Components implemented this phase

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
  `)

  console.log(`✓ Committed: ${await bash('git log -1 --format="%h"')}`)

  // Update plan.md with phase timestamp
  const timestamp = new Date().toISOString()
  updatePlanMd(pluginName, phase.number, timestamp)

  // Update handoff
  updateHandoff(
    pluginName,
    phase.number,  // e.g., "4.1"
    `Phase ${phase.number} complete: ${phase.description}`,
    i < phases.length - 1
      ? [`Phase ${phases[i+1].number}: ${phases[i+1].description}`, "Review current phase", "Test", "Pause"]
      : ["Auto-test Stage 4 output", "Review complete DSP", "Pause"],
    complexityScore,
    true  // phased
  )

  // Decision menu after each phase
  if (i < phases.length - 1) {
    // Not last phase
    console.log(`
✓ Phase ${phase.number} complete

Progress: ${i + 1} of ${phases.length} phases complete

What's next?
1. Continue to Phase ${phases[i+1].number} (${phases[i+1].description}) (recommended)
2. Review Phase ${phase.number} code
3. Test current state in DAW
4. Pause here
5. Other

Choose (1-5): _
    `)

    const choice = getUserInput()
    if (choice === "4" || choice === "pause") {
      console.log("\n⏸ Paused between phases. Resume with /continue to continue Phase ${phases[i+1].number}.")
      return
    }
    // Other choices: review, test, etc., then re-present menu
  }

  // Last phase - continue to auto-test
}

console.log(`\n✓ All ${phases.length} phases complete!`)
```

### 3. Handle Non-Phased Success

**After single-pass success:**

```typescript
// Update state
updateHandoff(
  pluginName,
  4,
  "Stage 4: DSP - Audio processing implemented",
  ["Auto-test Stage 4", "Review DSP code", "Test audio manually"],
  complexityScore,
  false
)

updatePluginStatus(pluginName, "🚧 Stage 4")
updatePluginTimeline(pluginName, 4, `DSP complete - ${report.outputs.dsp_components_implemented.length} components`)

// Git commit
bash(`
git add plugins/${pluginName}/Source/
git add plugins/${pluginName}/.continue-here.md
git add PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: ${pluginName} Stage 4 - DSP

Audio processing implemented
${report.outputs.dsp_components_implemented.length} DSP components
All parameters connected to audio engine
Real-time safe implementation

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
`)
```

### 4. Handle Failure (Single-Pass or Any Phase)

**4-option failure menu:**

```typescript
if (report.status === "failure") {
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 4 Failed: DSP Implementation
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Error: ${report.outputs.error_message}
Type: ${report.outputs.error_type}

Issues:
${report.issues.map(i => `  - ${i}`).join('\n')}

What would you like to do?
1. Investigate (invoke troubleshooter)
2. Show me the code (PluginProcessor.cpp)
3. Show me the build output
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
  `)

  // Handle choice (same as Stage 2/3)
}
```

### 5. Auto-Invoke plugin-testing Skill

**After Stage 4 succeeds (all phases if phased):**

```typescript
console.log("\n━━━ Running automated tests ━━━\n")

const testResult = Task({
  subagent_type: "general-purpose",  // Or invoke plugin-testing skill directly
  description: `Test ${pluginName} after Stage 4`,
  prompt: `
Run automated tests for ${pluginName} after Stage 4 DSP implementation.

Use the plugin-testing skill to run the 5 automated tests:
1. Build test (already passed)
2. Load test (plugin loads without crash)
3. Process test (audio processing works)
4. Parameter test (parameters affect audio)
5. State test (save/load works)

Report results.
  `
})

console.log(testResult)

// Parse test results
const testsPassed = testResult.includes("✓ All tests passed") || testResult.includes("PASS")

if (!testsPassed) {
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 4 Tests FAILED
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Stage 4 DSP implementation completed, but automated tests failed.

Test results:
${testResult}

CANNOT proceed to Stage 5 (GUI) with failing DSP tests.

What would you like to do?
1. Investigate test failures
2. Show me the DSP code
3. Show me the test output
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
  `)

  // STOP - Do not proceed to Stage 5 with failing tests
  return
}

console.log("✓ All Stage 4 tests passed")
```

### 6. Decision Menu (After Tests Pass)

```
✓ Stage 4 complete: Audio processing operational

Plugin: [PluginName]
DSP components: [N]
Parameters connected: [N]
Tests: All passed

What's next?
1. Continue to Stage 5 (implement UI) (recommended)
2. Test audio manually in DAW
3. Review DSP code
4. Adjust DSP before UI
5. Pause here
6. Other

Choose (1-6): _
```

**CRITICAL: Do NOT proceed to Stage 5 if tests fail.**

---

## Stage 5: GUI

**Goal:** Integrate WebView UI with parameter bindings

**Duration:** 20-60 minutes (depending on complexity)

**Preconditions:**
- Stage 4 complete and tests passed (DSP operational)
- Finalized UI mockup exists (v[N]-ui.html)
- parameter-spec.md exists

**Actions:**

### 1. Locate Finalized UI Mockup

**Scan for finalized mockup version:**

```bash
cd plugins/${PLUGIN_NAME}/.ideas/mockups/

# Find highest version number
LATEST_MOCKUP=$(ls -1 v*-ui.html 2>/dev/null | sort -V | tail -1)

if [ -z "$LATEST_MOCKUP" ]; then
  echo "
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ BLOCKED: Cannot proceed to Stage 5
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Missing required contract: Finalized UI mockup

Stage 5 requires a finalized UI mockup (v[N]-ui.html) to integrate.

WHY BLOCKED:
Without a finalized UI mockup, gui-agent cannot know:
- UI layout and styling
- Parameter control types (sliders, knobs, toggles)
- Visual design and branding
- Dimensions and layout

HOW TO UNBLOCK:
1. Run /mockup to create UI mockup
2. Design UI through iterative refinement
3. Finalize a design version (marks it as v[N])
4. Then resume Stage 5 with /continue ${PLUGIN_NAME}

Current status: No finalized mockup found in .ideas/mockups/

Cannot proceed without this contract.
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  "
  exit 1
fi

echo "✓ Found finalized mockup: $LATEST_MOCKUP"
```

**If no mockup found:** STOP IMMEDIATELY, exit skill, wait for user to create mockup.

### 2. Read Complexity and Check for Phases

Same as Stage 4 - check if phased implementation needed:

```typescript
const planContent = readFile(`plugins/${pluginName}/.ideas/plan.md`)
const complexityScore = extractComplexityScore(planContent)
const hasPhases = planContent.includes("### Phase 5.1") || planContent.includes("## Stage 5: GUI Phases")

console.log(`Complexity: ${complexityScore} (${hasPhases ? 'phased' : 'single-pass'})`)
```

### 3a. Single-Pass Implementation (Complexity ≤2)

**If complexity ≤2 OR no phases defined:**

Invoke gui-agent once for complete UI integration:

```typescript
const mockupPath = findLatestMockup(pluginName)
const mockupContent = readFile(mockupPath)

const guiResult = Task({
  subagent_type: "gui-agent",
  model: "sonnet",
  description: `Integrate WebView UI for ${pluginName}`,
  prompt: `
You are gui-agent. Your task is to integrate the finalized WebView UI for ${pluginName}.

**Plugin Name:** ${pluginName}
**Plugin Location:** plugins/${pluginName}/
**Finalized Mockup:** ${mockupPath}
**Complexity:** ${complexityScore} (single-pass implementation)

**Contract Files:**

Finalized UI mockup (${mockupPath}):
\`\`\`html
${mockupContent}
\`\`\`

parameter-spec.md:
\`\`\`
${parameterSpecContent}
\`\`\`

creative-brief.md:
\`\`\`
${creativeBriefContent}
\`\`\`

Follow the instructions in .claude/agents/gui-agent.md exactly.

Copy finalized mockup to ui/public/index.html, download JUCE frontend library, create relay members in PluginEditor.h (CRITICAL: Relays → WebView → Attachments order), implement parameter bindings, update CMakeLists.txt for WebView.

Build verification handled by plugin-workflow after agent completes.

⚠️ CRITICAL: Member declaration order prevents 90% of release build crashes.

Return JSON report in the exact format specified in gui-agent.md.
  `
})

// Parse and validate report
const report = parseSubagentReport(guiResult)

if (report.status === "success") {
  console.log(`✓ Stage 5 complete: UI integrated`)
  console.log(`  - Mockup version: ${report.outputs.ui_mockup_version}`)
  console.log(`  - Bindings: ${report.outputs.binding_count}`)
  console.log(`  - All parameters bound: ${report.outputs.all_parameters_bound ? 'Yes' : 'No'}`)
  console.log(`  - Member order correct: ${report.outputs.member_order_correct ? 'Yes' : 'No'}`)

  // Continue to auto-test (Step 5)
}
```

### 3b. Phased Implementation (Complexity ≥3)

**If complexity ≥3 AND plan.md defines UI phases:**

Parse phase breakdown (similar to Stage 4):

```typescript
// Extract Phase 5.1, 5.2, etc. from plan.md
const phasePattern = /### Phase (5\.\d+):\s*(.+?)\n/g
const phases = []
let match

while ((match = phasePattern.exec(planContent)) !== null) {
  phases.push({
    number: match[1],  // e.g., "5.1"
    description: match[2]  // e.g., "Layout and Basic Controls"
  })
}

console.log(`Stage 5 will execute in ${phases.length} phases:`)
phases.forEach(phase => {
  console.log(`  - Phase ${phase.number}: ${phase.description}`)
})

// Execute each phase sequentially (same pattern as Stage 4)
for (let i = 0; i < phases.length; i++) {
  const phase = phases[i]

  console.log(`\n━━━ Stage ${phase.number} - ${phase.description} ━━━\n`)

  const phaseResult = Task({
    subagent_type: "gui-agent",
    model: "sonnet",
    description: `Implement UI Phase ${phase.number} for ${pluginName}`,
    prompt: `
You are gui-agent. Your task is to implement Phase ${phase.number} of UI for ${pluginName}.

**Current Phase:** ${phase.number} - ${phase.description}
**Total Phases:** ${phases.length}

[Include mockup, contracts, phase-specific instructions]

Return JSON report with phase_completed: "${phase.number}".
    `
  })

  const phaseReport = parseSubagentReport(phaseResult)

  if (!phaseReport || phaseReport.status === "failure") {
    console.log(`✗ Phase ${phase.number} failed`)
    // 4-option menu, stop on failure
    return
  }

  // Phase succeeded - git commit, update plan, handoff, decision menu
  console.log(`✓ Phase ${phase.number} complete`)

  // Git commit for this phase
  bash(`
git add plugins/${pluginName}/Source/
git add plugins/${pluginName}/ui/
git add plugins/${pluginName}/CMakeLists.txt
git add plugins/${pluginName}/.ideas/plan.md
git add plugins/${pluginName}/.continue-here.md

git commit -m "feat: ${pluginName} Stage ${phase.number} - ${phase.description}"
  `)

  // Decision menu between phases
  if (i < phases.length - 1) {
    console.log(`
✓ Phase ${phase.number} complete

Progress: ${i + 1} of ${phases.length} phases complete

What's next?
1. Continue to Phase ${phases[i+1].number} (recommended)
2. Review Phase ${phase.number} code
3. Test UI in DAW
4. Pause here
5. Other

Choose (1-5): _
    `)

    const choice = getUserInput()
    if (choice === "4" || choice === "pause") {
      console.log(`\n⏸ Paused between UI phases. Resume with /continue.`)
      return
    }
  }
}

console.log(`\n✓ All ${phases.length} UI phases complete!`)
```

### 4. Handle Non-Phased Success

**After single-pass success:**

```typescript
// Update state
updateHandoff(
  pluginName,
  5,
  "Stage 5: GUI - WebView UI integrated, all parameters bound",
  ["Auto-test Stage 5", "Test UI in DAW", "Review bindings"],
  complexityScore,
  false
)

updatePluginStatus(pluginName, "🚧 Stage 5")
updatePluginTimeline(pluginName, 5, `GUI complete - ${report.outputs.binding_count} parameter bindings`)

// Git commit
bash(`
git add plugins/${pluginName}/Source/
git add plugins/${pluginName}/ui/
git add plugins/${pluginName}/CMakeLists.txt
git add plugins/${pluginName}/.continue-here.md
git add PLUGINS.md

git commit -m "$(cat <<'EOF'
feat: ${pluginName} Stage 5 - GUI

WebView UI integrated from ${report.outputs.ui_mockup_version}
${report.outputs.binding_count} parameter bindings created
Member order: Relays → WebView → Attachments
CMakeLists.txt updated for WebView support

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
EOF
)"
`)
```

### 5. Handle Failure

**4-option failure menu:**

```typescript
if (report.status === "failure") {
  // Special handling for binding_mismatch error
  if (report.outputs.error_type === "binding_mismatch") {
    console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 5 Failed: Parameter Binding Mismatch
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Error: Not all parameters from parameter-spec.md have UI bindings

Expected: ${report.outputs.expected_count} parameters
Implemented: ${report.outputs.actual_count} relay/attachment pairs
Missing: ${report.outputs.missing_bindings.join(', ')}

This is a ZERO-DRIFT violation. All parameters must be bound to UI.

What would you like to do?
1. Investigate (why were bindings missing?)
2. Show me the code (PluginEditor.h)
3. Show me the parameter-spec.md
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
    `)
  } else {
    // Other errors (build failure, member order wrong, etc.)
    console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 5 Failed: UI Integration
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Error: ${report.outputs.error_message}
Type: ${report.outputs.error_type}

Issues:
${report.issues.map(i => `  - ${i}`).join('\n')}

What would you like to do?
1. Investigate
2. Show me the code (PluginEditor.h/cpp)
3. Show me the build output
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
    `)
  }

  // Handle choice (same pattern as Stages 2-4)
}
```

### 6. Auto-Invoke plugin-testing Skill

**After Stage 5 succeeds (all phases if phased):**

```typescript
console.log("\n━━━ Running automated tests (including UI validation) ━━━\n")

const testResult = Task({
  subagent_type: "general-purpose",
  description: `Test ${pluginName} after Stage 5`,
  prompt: `
Run automated tests for ${pluginName} after Stage 5 UI integration.

Use the plugin-testing skill to run all 5 tests plus UI-specific checks:
1. Build test (already passed)
2. Load test (plugin loads without crash)
3. Process test (audio processing works)
4. Parameter test (parameters affect audio)
5. State test (save/load works)
6. UI test (WebView renders, parameters sync)

Report results.
  `
})

console.log(testResult)

// Parse test results
const testsPassed = testResult.includes("✓ All tests passed") || testResult.includes("PASS")

if (!testsPassed) {
  console.log(`
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✗ Stage 5 Tests FAILED
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Stage 5 UI integration completed, but automated tests failed.

Test results:
${testResult}

CANNOT proceed to Stage 6 (validation) with failing UI tests.

What would you like to do?
1. Investigate test failures
2. Show me the UI code
3. Show me the test output
4. I'll fix it manually (say "resume automation" when ready)

Choose (1-4): _
  `)

  // STOP - Do not proceed to Stage 6 with failing tests
  return
}

console.log("✓ All Stage 5 tests passed (including UI validation)")
```

### 7. Decision Menu (After Tests Pass)

```
✓ Stage 5 complete: UI operational

Plugin: [PluginName]
UI: WebView integrated from [mockup version]
Bindings: [N] parameters bound to UI
Tests: All passed (including UI sync)

What's next?
1. Continue to Stage 6 (final validation) (recommended)
2. Test UI manually in DAW
3. Review UI code
4. Adjust UI styling
5. Pause here
6. Other

Choose (1-6): _
```

**CRITICAL: Do NOT proceed to Stage 6 if tests fail.**

---

## Stage 6: Validation

**Goal:** Ready to install and use

**Duration:** 10-20 minutes

**Preconditions:**

- Stages 0-5 complete
- Plugin compiles successfully
- Automated tests pass (if run)

**Actions:**

1. Create factory presets:

```bash
mkdir -p plugins/[PluginName]/Presets/
```

Create 3-5 preset files showcasing plugin capabilities.

**Preset format (.preset or .xml):**

```xml
<?xml version="1.0" encoding="UTF-8"?>
<preset name="[PresetName]">
  <param id="[paramID]" value="[value]"/>
  <param id="[paramID]" value="[value]"/>
  ...
</preset>
```

2. Invoke plugin-testing skill:

Present test method choice:

```
How would you like to test [PluginName]?

1. Automated stability tests (if Tests/ directory exists)
2. Build and run pluginval (recommended)
3. Manual DAW testing checklist
4. Skip testing (not recommended)

Choose (1-4): _
```

If tests fail, STOP and wait for fixes.

3. Generate CHANGELOG.md:

**Format:**

```markdown
# Changelog

All notable changes to [PluginName] will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.0] - [YYYY-MM-DD]

### Added

- Initial release
- [Feature 1]
- [Feature 2]
- [Parameter 1]: [Description]
- [Parameter 2]: [Description]

### Audio Processing

- [DSP description]

### User Interface

- [UI description]

### Validation

- Passed pluginval strict validation
- Tested in [DAW names]
```

4. Invoke validator subagent:

Call validator to verify Stage 6 completion:

```typescript
const validation = Task({
  subagent_type: "validator",
  description: `Validate ${pluginName} Stage 6`,
  prompt: `
Validate Stage 6 completion for ${pluginName}.

**Stage:** 6
**Plugin:** ${pluginName}
**Contracts:**
- parameter-spec.md: [paste content or "not applicable"]
- architecture.md: [paste content or "not applicable"]
- plan.md: [paste content]

**Expected outputs for Stage 6:**
- CHANGELOG.md exists in Keep a Changelog format
- Version 1.0.0 for initial release
- Presets/ directory has 3+ preset files
- pluginval passed (or skipped with reason)
- PLUGINS.md updated to ✅ Working

Return JSON validation report with status, checks, and recommendation.
  `
})

const report = JSON.parse(validation)

if (report.status === "FAIL") {
  // Present validation failure menu
  presentValidationFailure(report)
  // Options: Fix issues / Continue anyway / Pause
  // Wait for user choice before proceeding
}
```

5. Update PLUGINS.md:

Call `updatePluginStatus(pluginName, "✅ Working")`.

Add final fields to entry:
```markdown
**Version:** 1.0.0
**Completed:** [YYYY-MM-DD]

**Validation:**
- ✓ Factory presets: [N] presets created
- ✓ CHANGELOG.md: Generated in Keep a Changelog format
- ✓ Pluginval: [Passed | Skipped (no build)]

**Formats:** VST3, AU (if built)
```

Add timeline entry: `updatePluginTimeline(pluginName, 6, "Validation complete")`.

6. Delete .continue-here.md:

Call `deleteHandoff(pluginName)`:

```bash
rm plugins/[PluginName]/.continue-here.md
```

Workflow is complete, no need for handoff file.

**Git commit:**

Call `commitStage(pluginName, 6, "validation complete")`.

This commits:
- `plugins/[PluginName]/Presets/`
- `plugins/[PluginName]/CHANGELOG.md`
- `PLUGINS.md`

Note: `.continue-here.md` is deleted and NOT committed (workflow complete).

**Decision menu:**

```
✓ Stage 6 complete: [PluginName] is ready!

What's next?
1. Install plugin to system folders (recommended)
2. Test in DAW from build folder first
3. Create another plugin
4. Document this plugin
5. Share plugin (export build)
6. Other

Choose (1-6): _
```

**Handle responses:**

- Option 1 → Invoke `plugin-lifecycle` skill (Phase 1b Task 9)
- Option 2 → Provide instructions for manual DAW testing
- Option 3 → Exit, suggest `/dream` or `/implement`
- Option 4 → Suggest creating README or documentation
- Option 5 → Provide instructions for exporting builds
- Option 6 → Ask what they'd like to do

---

## State Machine Functions

### updatePluginStatus(pluginName, newStatus)

**Purpose:** Update plugin status emoji in PLUGINS.md.

**Valid statuses:**
- `💡 Ideated` - Creative brief exists, no Source/
- `🚧 Stage N` - In development (with stage number)
- `🚧 Stage N.M` - In development (with stage and phase)
- `✅ Working` - Stage 6 complete, not installed
- `📦 Installed` - Deployed to system folders

**Implementation:**
1. Read PLUGINS.md
2. Find section: `### [pluginName]`
3. Update `**Status:**` line with new emoji and text
4. Validate transition is legal (see validateTransition below)
5. Write back to PLUGINS.md

**Example:**
```markdown
### TapeDelay
**Status:** 🚧 Stage 4 → **Status:** 🚧 Stage 5
```

### createPluginEntry(pluginName, type, brief)

**Purpose:** Create initial PLUGINS.md entry when starting new plugin.

**Implementation:**
1. Read PLUGINS.md
2. Check if entry already exists (search for `### [pluginName]`)
3. If not exists, append new entry:
   ```markdown
   ### [pluginName]
   **Status:** 💡 Ideated
   **Type:** [Audio Effect | MIDI Instrument | Synth]
   **Created:** [YYYY-MM-DD]

   [Brief description from creative-brief.md]

   **Lifecycle Timeline:**
   - **[YYYY-MM-DD]:** Creative brief created

   **Last Updated:** [YYYY-MM-DD]
   ```
4. Write back to PLUGINS.md

### updatePluginTimeline(pluginName, stage, description)

**Purpose:** Add timeline entry to PLUGINS.md when stage completes.

**Implementation:**
1. Read PLUGINS.md
2. Find plugin entry
3. Find `**Lifecycle Timeline:**` section
4. Append new entry:
   ```markdown
   - **[YYYY-MM-DD] (Stage N):** [description]
   ```
5. Update `**Last Updated:**` field
6. Write back to PLUGINS.md

### getPluginStatus(pluginName)

**Purpose:** Return current status emoji for routing logic.

**Implementation:**
1. Read PLUGINS.md
2. Find `### [pluginName]` section
3. Extract `**Status:**` line
4. Parse emoji: 💡, 🚧, ✅, or 📦
5. If 🚧, extract stage number (e.g., "🚧 Stage 4" → 4)
6. Return object: `{ emoji: "🚧", stage: 4, text: "Stage 4" }`

### validateTransition(currentStatus, newStatus)

**Purpose:** Enforce legal state machine transitions.

**Legal transitions:**
```
💡 Ideated → 🚧 Stage 0 (start workflow)
🚧 Stage N → 🚧 Stage N+1 (sequential stages)
🚧 Stage 6 → ✅ Working (validation complete)
✅ Working → 📦 Installed (install plugin)
📦 Installed → 🚧 Improving (start improvement)
🚧 Improving → 📦 Installed (improvement complete)
```

**Illegal transitions:**
```
💡 → ✅ (can't skip implementation)
🚧 Stage 2 → 🚧 Stage 5 (can't skip stages)
✅ Working → 💡 (can't go backward)
```

**Implementation:**
1. Parse current and new status
2. Check transition against rules
3. Return: `{ allowed: true }` or `{ allowed: false, reason: "..." }`


## Interactive Decision Menu System

### presentDecisionMenu(context)

**Purpose:** Present context-aware decision menu at every checkpoint.

**Context parameters:**
- `stage`: Current stage number (0-6)
- `completionStatement`: What was just accomplished
- `pluginName`: Plugin being worked on
- `errors`: Any errors/failures (optional)
- `options`: Custom options (optional)

**Format - Inline Numbered List (NOT AskUserQuestion):**

```
✓ [Completion statement]

What's next?
1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery feature] ← User discovers [capability]
4. [Alternative path]
5. [Escape hatch]
6. Other

Choose (1-6): _
```

**Implementation:**
1. Generate context-appropriate options (see generateContextualOptions below)
2. Format as inline numbered list
3. Display to user
4. Wait for response (number, keyword shortcut, or "Other")
5. Parse response
6. Execute chosen action or re-present menu if invalid

**Keyword Shortcuts:**
- "continue" → Option 1 (primary action)
- "pause" → Pause option (creates checkpoint)
- "review" → Review option (show code/context)

**Handle "Other" responses:**
```
User: Other
System: "What would you like to do?"
User: [Free-form request]
System: [Process request]
System: [Re-present decision menu afterward]
```

### generateContextualOptions(context)

**Purpose:** Generate situation-specific menu options.

**After Stage 0 (Research):**
```javascript
[
  { label: "Continue to Stage 1", recommended: true },
  { label: "Review research findings" },
  { label: "Improve creative brief based on research" },
  { label: "Run deeper investigation (deep-research skill)" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 1 (Planning):**
```javascript
[
  { label: "Continue to Stage 2", recommended: true },
  { label: "Review plan details" },
  { label: "Adjust complexity assessment" },
  { label: "Review contracts" },
  { label: "Pause here" },
  { label: "Other" }
]
```

**After Stage 6 (Validation):**
```javascript
[
  { label: "Install plugin to system folders", recommended: true },
  { label: "Test in DAW first" },
  { label: "Create another plugin" },
  { label: "Review complete plugin code" },
  { label: "Document this plugin" },
  { label: "Other" }
]
```

**Build Failure:**
```javascript
[
  { label: "Investigate", discovery: "Run deep-research to find root cause" },
  { label: "Show me the code" },
  { label: "Show full build output" },
  { label: "I'll fix it manually (say \"resume automation\" when ready)" },
  { label: "Other" }
]
```

**Validation Failure:**
```javascript
[
  { label: "Fix and re-validate", recommended: true },
  { label: "Re-run stage" },
  { label: "Override (not recommended)" },
  { label: "Other" }
]
```

### formatDecisionMenu(completionStatement, options)

**Purpose:** Format options as inline numbered list.

**Implementation:**
```
output = `✓ ${completionStatement}\n\n`
output += `What's next?\n`

options.forEach((opt, i) => {
  output += `${i+1}. ${opt.label}`

  if (opt.recommended) {
    output += ` (recommended)`
  }

  if (opt.discovery) {
    output += ` ← ${opt.discovery}`
  }

  output += `\n`
})

output += `\nChoose (1-${options.length}): _`

return output
```

**Progressive Disclosure:**
Use discovery markers (`← User discovers [feature]`) to surface hidden capabilities:
- "Save as template ← Add to UI template library"
- "Design sync ← Validate brief matches mockup"
- "/troubleshoot-juce ← Document problems for knowledge base"

### handleMenuChoice(choice, options, context)

**Purpose:** Parse user response and execute chosen action.

**Implementation:**
```javascript
// Parse response
if (isNumber(choice)) {
  const index = parseInt(choice) - 1
  if (index >= 0 && index < options.length) {
    return executeOption(options[index], context)
  } else {
    return { error: "Invalid choice", reprompt: true }
  }
}

// Handle keyword shortcuts
if (choice.toLowerCase() === "continue") {
  return executeOption(options[0], context) // First option
}

if (choice.toLowerCase() === "pause") {
  const pauseOption = options.find(o => o.label.includes("Pause"))
  return executeOption(pauseOption, context)
}

if (choice.toLowerCase() === "review") {
  const reviewOption = options.find(o => o.label.includes("Review"))
  return executeOption(reviewOption, context)
}

// Handle "Other"
if (choice.toLowerCase() === "other" || options[choice - 1].label === "Other") {
  return { action: "ask_freeform", reprompt: true }
}
```

**After executing action:**
- Re-present menu if action was exploratory (review, show code)
- Continue workflow if action was directive (continue, pause)

## Git Commit Functions

### commitStage(pluginName, stage, description)

**Purpose:** Create standardized git commit after stage completion.

**Commit message format:**
```
feat: [PluginName] Stage [N] - [description]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

**For phased stages:**
```
feat: [PluginName] Stage [N.M] - [phase description]

🤖 Generated with Claude Code

Co-Authored-By: Claude <noreply@anthropic.com>
```

**Implementation:**
1. Stage changes atomically in single commit:
   ```bash
   git add plugins/[PluginName]/Source/ (if exists)
   git add plugins/[PluginName]/.ideas/ (contracts)
   git add plugins/[PluginName]/.continue-here.md (handoff)
   git add PLUGINS.md (state)
   ```

2. Commit with standardized message using heredoc:
   ```bash
   git commit -m "$(cat <<'EOF'
   feat: [PluginName] Stage [N] - [description]

   🤖 Generated with Claude Code

   Co-Authored-By: Claude <noreply@anthropic.com>
   EOF
   )"
   ```

3. Verify commit succeeded:
   ```bash
   git log -1 --format='%h'
   ```

4. Display commit hash to user:
   ```
   ✓ Committed: abc1234 - Stage [N] complete
   ```

5. If commit fails:
   - Warn user
   - Suggest manual commit
   - Continue workflow (don't block)

**Atomic state transitions:**
- PLUGINS.md update + handoff update + code changes = Single commit
- If commit fails → Rollback state changes (or warn user about inconsistency)

**Commit variations by stage:**
- Stage 0: `feat: [Plugin] Stage 0 - research complete`
- Stage 1: `feat: [Plugin] Stage 1 - planning complete`
- Stage 2: `feat: [Plugin] Stage 2 - foundation compiles`
- Stage 3: `feat: [Plugin] Stage 3 - shell loads in DAW`
- Stage 4: `feat: [Plugin] Stage 4 - DSP complete`
- Stage 4.1: `feat: [Plugin] Stage 4.1 - core processing`
- Stage 4.2: `feat: [Plugin] Stage 4.2 - parameter modulation`
- Stage 5: `feat: [Plugin] Stage 5 - GUI complete`
- Stage 6: `feat: [Plugin] Stage 6 - validation complete`

### verifyGitAvailable()

**Purpose:** Check git is available before workflow starts.

**Implementation:**
```bash
if ! command -v git &> /dev/null; then
    echo "⚠️ Warning: git not found. Commits will be skipped."
    echo "Install git to enable automatic commit workflow."
    return false
fi

if ! git rev-parse --git-dir &> /dev/null; then
    echo "⚠️ Warning: Not a git repository. Commits will be skipped."
    echo "Run 'git init' to enable automatic commit workflow."
    return false
fi

return true
```

Call at beginning of Stage 0.

## Handoff Management Functions

### createHandoff(pluginName, stage, context)

**Purpose:** Create initial handoff file after Stage 0 completion.

**Implementation:**
1. Read handoff template from `.claude/skills/plugin-workflow/assets/continue-here-template.md`
2. Fill in YAML frontmatter:
   - plugin: [pluginName]
   - stage: [stage number]
   - phase: null (only for complex plugins)
   - status: "in_progress"
   - last_updated: [current timestamp]
   - complexity_score: null (filled in Stage 1)
   - phased_implementation: null (filled in Stage 1)
3. Fill in markdown sections with context:
   - Current State: "Stage [N] - [description]"
   - Completed So Far: [what's done]
   - Next Steps: [prioritized actions]
   - Context to Preserve: [key decisions, files, build status]
4. Write to `plugins/[pluginName]/.continue-here.md`

### updateHandoff(pluginName, stage, completed, nextSteps, complexityScore, phased)

**Purpose:** Update handoff file after each stage/phase completion.

**Implementation:**
1. Read existing `plugins/[pluginName]/.continue-here.md`
2. Update YAML frontmatter:
   - stage: [new stage number]
   - phase: [phase number if complex]
   - status: [in_progress | complete]
   - last_updated: [current timestamp]
   - complexity_score: [score if known]
   - phased_implementation: [true/false if known]
3. Append to "Completed So Far" section
4. Update "Next Steps" with new actions
5. Update "Context to Preserve" with latest context
6. Write back to file

### deleteHandoff(pluginName)

**Purpose:** Remove handoff file when plugin reaches ✅ Working or 📦 Installed.

**Implementation:**
1. Check if `plugins/[pluginName]/.continue-here.md` exists
2. Delete file
3. Log deletion (workflow complete)

**When to call:**
- After Stage 6 complete (status → ✅ Working)
- After plugin installation (status → 📦 Installed)

## Checkpoint Types

### Hard Checkpoints (MUST pause for user decision)

**Stages:**
- Stage 0: Research complete
- Stage 1: Planning complete
- Stage 6: Validation complete

**Behavior:**
1. Complete stage work
2. Auto-commit changes
3. Update handoff file
4. Update PLUGINS.md
5. Present decision menu
6. **WAIT for user response** - do NOT auto-continue
7. Execute user choice

### Soft Checkpoints (can auto-continue)

**Phases within complex stages (complexity ≥3):**
- Stage 4.1, 4.2, 4.3: DSP phases
- Stage 5.1, 5.2: GUI phases

**Behavior:**
1. Complete phase work
2. Auto-commit changes
3. Update handoff file
4. Present decision menu with "Continue to next phase" as recommended option
5. If user chooses continue: proceed to next phase
6. If user chooses pause: stop and preserve state

### Decision Checkpoints

**Occur before significant choices:**
- Build failures (show 4-option menu)
- Validation failures (show 3-option menu)
- Manual pause requests

**Behavior:**
1. Update handoff with current context
2. Present situation-specific menu
3. Wait for user choice
4. Execute chosen path

## Resume Handling

**Support "resume automation" command:**

If user paused and says "resume automation" or chooses to continue:

1. Read `.continue-here.md` to determine current stage/phase
2. Parse YAML frontmatter for stage, phase, complexity_score, phased_implementation
3. Continue from documented "Next Steps"
4. Load relevant context (contracts, research, plan)

---

## Stage Boundary Protocol

**At every stage completion:**

1. Show completion statement:

```
✓ Stage [N] complete: [description]
```

2. Run automated tests (Stages 4, 5 only):

   - Invoke plugin-testing skill
   - If fail: STOP, show results, wait for fixes
   - If pass: Continue

3. Auto-commit:

```bash
git add [files]
# Message format: feat: [Plugin] Stage [N] - [description]
# For complex: feat: [Plugin] Stage [N.M] - [phase description]
```

4. Update `.continue-here.md` with new stage, timestamp, context

5. Update PLUGINS.md with new status

6. Present decision menu with context-appropriate options

7. Wait for user response

**Do NOT auto-proceed without user confirmation.**

---

## Integration Points

**Invoked by:**

- `/implement` command
- `plugin-ideation` skill (after creative brief)
- `context-resume` skill (when resuming)

**Invokes:**

- `plugin-testing` skill (Stages 4, 5, 6)
- `plugin-lifecycle` skill (after Stage 6, if user chooses install)

**Creates:**

- `.continue-here.md` (handoff file)
- `architecture.md` (Stage 0 - DSP specification)
- `plan.md` (Stage 1)
- `CHANGELOG.md` (Stage 6)
- `Presets/` directory (Stage 6)

**Updates:**

- PLUGINS.md (status changes throughout)

---

## Error Handling

**If contract files missing at Stage 1:**
Block and guide to create UI mockup first.

**If build fails at any stage:**
Present menu:

```
Build error at [stage]:
[Error context]

What would you like to do?
1. Investigate (triggers deep-research)
2. Show me the code
3. Show full build output
4. I'll fix it manually (say "resume automation" when ready)
5. Other

Choose (1-5): _
```

**If tests fail:**
Present menu with investigation options.

**If git staging fails:**
Continue anyway, log warning.

---

## Success Criteria

Workflow is successful when:

- Plugin compiles without errors
- All stages completed in sequence
- Tests pass (if run)
- PLUGINS.md updated to ✅ Working
- Handoff file deleted (workflow complete)
- Git history shows all stage commits
- Ready for installation or improvement
