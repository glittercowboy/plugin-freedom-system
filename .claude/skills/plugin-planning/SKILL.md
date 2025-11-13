---
name: plugin-planning
description: Interactive research and planning (Stages 0-1) for JUCE plugin development. Invoke via /plan command after creative-brief.md exists or as first step of /implement workflow.
allowed-tools:
  - Read # For contracts and references
  - Write # For architecture.md, plan.md
  - Edit # For state updates (PLUGINS.md, .continue-here.md)
  - Bash # For git commits, precondition checks
  - WebSearch # For professional plugin research
  - Grep # For searching existing implementations
  - Glob # For finding reference files
preconditions:
  - creative-brief.md must exist in plugins/[Name]/.ideas/
  - Plugin must NOT already be past Stage 1
---

# plugin-planning Skill

**Purpose:** Handle Stages 0-1 (Research and Planning) through interactive contract creation without subagents. This skill creates the foundation contracts (architecture.md, plan.md) that guide implementation.

**Invoked by:** `/plan` command (to be created) or as first step of `/implement` workflow

---

## Entry Point

**Check preconditions first:** See [references/preconditions.md](references/preconditions.md) for detailed validation logic.

Quick validation:
1. creative-brief.md must exist at `plugins/[Name]/.ideas/`
2. Plugin status must be ≤ Stage 1 (not already in implementation)
3. Detect existing contracts (architecture.md, plan.md) for resume logic

If all preconditions pass → proceed to Stage 0 or Stage 1 based on resume logic

---

## Stage 0: Research

**Goal:** Create DSP architecture specification (architecture.md)
**Duration:** 5-30 minutes (complexity-dependent)
**Implementation:** Delegated to research-agent subagent via Task tool

**Dispatch pattern:**

1. Read contracts to pass to subagent:
   - plugins/[Name]/.ideas/creative-brief.md (REQUIRED)
   - plugins/[Name]/.ideas/mockups/*.yaml (if exists, for design sync)

2. Construct prompt with contracts prepended:
   ```
   You are research-agent. Execute Stage 0 research for [PluginName].

   Creative brief:
   [content of creative-brief.md]

   [If mockup exists:]
   UI mockup:
   [content of mockup file]

   Execute the full Stage 0 research protocol:
   1. Complexity detection (Tier 1-6)
   2. Feature identification (meta-research)
   3. Per-feature deep research (algorithmic understanding, professional research, JUCE API mapping, validation)
   4. Integration analysis
   5. Create architecture.md from template
   6. Update state files and commit

   Return JSON report with architecture.md location and status.
   ```

3. Invoke subagent:
   ```
   Task(subagent_type="research-agent", description="[prompt with contracts]", model="sonnet")
   ```

4. AFTER subagent returns, execute checkpoint protocol:
   - Read subagent's JSON report
   - Verify architecture.md created and contains all required sections
   - Present decision menu (see [references/state-updates.md](references/state-updates.md))
   - WAIT for user response

**Subagent executes:** Full research protocol from references/stage-0-research.md
- Complexity detection (Tier 1-6) with extended thinking
- Per-feature deep research (algorithmic understanding, professional research, JUCE API mapping, validation)
- Integration analysis (dependencies, interactions, processing order, threads)
- architecture.md generation from assets/architecture-template.md
- State updates (.continue-here.md, PLUGINS.md, git commit)

**Output:** `plugins/[Name]/.ideas/architecture.md` (see assets/architecture-template.md)

**Decision menu after subagent completes:**

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

**Note:** research-agent runs in fresh context (5-30 min research doesn't pollute orchestrator)

**VALIDATION GATE: Before proceeding to Stage 1:**

1. Verify: architecture.md exists and is not empty
2. Verify: architecture.md contains all required sections (Core Components, Processing Chain, Parameter Mapping)
3. If any verification fails:
   - Display error: "Stage 0 incomplete - architecture.md missing or invalid"
   - Return to Stage 0
4. Only proceed when all verifications pass

---

## Stage 1: Planning

**Goal:** Calculate complexity score and create implementation plan (plan.md)
**Duration:** 2-5 minutes

**Preconditions:** parameter-spec.md AND architecture.md must exist (BLOCKING)

**Process:**

1. Read contracts (parameter-spec.md, architecture.md)
2. Calculate complexity score: `min(param_count/5, 2.0) + algorithm_count + feature_count`
3. Determine strategy: Simple (≤2.0) = single-pass, Complex (≥3.0) = phased
4. Create phase breakdown if complex
5. Generate plan.md from template

**For detailed complexity calculation and phase breakdown:** See [references/stage-1-planning.md](references/stage-1-planning.md)

**Workflow Checklist** - Copy and track progress:

```
Stage 1 Progress:
- [ ] Verify contracts exist (parameter-spec.md, architecture.md)
- [ ] Read all contracts
- [ ] Calculate complexity score
- [ ] Determine implementation strategy
- [ ] Create phase breakdown (if complex)
- [ ] Generate plan.md from template
- [ ] Update .continue-here.md
- [ ] Update PLUGINS.md status
- [ ] Git commit changes
- [ ] Present decision menu
```

**Output:** `plugins/[Name]/.ideas/plan.md` (see assets/plan-template.md)

**State management:** Update .continue-here.md, PLUGINS.md status, git commit (see [references/git-operations.md](references/git-operations.md))

**Decision menu:** Present menu from assets/decision-menu-stage-1.md, WAIT for user response

---

## Handoff to Implementation

<handoff_protocol id="planning-to-implementation">
<state_requirement>
CRITICAL: Handoff file must be updated to Stage 2. Execute steps EXACTLY in sequence.
</state_requirement>

**When user chooses to proceed to Stage 2:**

<critical_sequence>
1. Update handoff file at plugin root:
```bash
# Update existing plugins/${PLUGIN_NAME}/.continue-here.md for Stage 2
# Use template from assets/implementation-handoff-template.md
cat > plugins/${PLUGIN_NAME}/.continue-here.md <<'EOF'
[template content from assets/implementation-handoff-template.md]
EOF
```

2. Verify handoff:
```bash
test -f "plugins/${PLUGIN_NAME}/.continue-here.md" || { echo "✗ Handoff not created"; exit 1; }
echo "✓ Handoff verified"
```
</critical_sequence>

<verification_step>
After handoff, verify:
- plugins/[PluginName]/.continue-here.md exists at root
- PLUGINS.md status updated to 🚧 Stage 2
</verification_step>
</handoff_protocol>

Display to user:

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

---

## Reference Files

Detailed stage implementations are in:
- `references/stage-0-research.md` - Research stage details
- `references/stage-1-planning.md` - Planning stage details

Templates are in:
- `assets/architecture-template.md` - DSP architecture contract template
- `assets/plan-template.md` - Implementation plan template
