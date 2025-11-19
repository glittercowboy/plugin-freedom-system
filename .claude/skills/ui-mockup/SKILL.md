---
name: ui-mockup
description: Orchestrator for WebView UI mockup workflow - delegates design iteration to ui-design-agent and implementation scaffolding to ui-finalization-agent. Use when user mentions UI design, mockup, WebView interface, or requests 'design UI for [plugin]'.
allowed-tools:
  - Read
  - Task
  - AskUserQuestion
preconditions:
  - None (can work standalone or with creative brief)
---

<objective>
Creates WebView UI mockups for audio plugins through iterative design and implementation scaffolding.
</objective>

<quick_start>
1. Check for aesthetic library (Phase 0)
2. Gather requirements through tiered questions (Phases 1-3)
3. Dispatch ui-design-agent for mockup generation
4. Iterate until user approves
5. Generate implementation files
</quick_start>

<workflow>
<phase name="A-design-iteration">
<purpose>Generate 2 design files for rapid iteration</purpose>
<outputs>
- `v[N]-ui.yaml` - Machine-readable design specification
- `v[N]-ui-test.html` - Browser-testable mockup
</outputs>
<gate>Do NOT proceed to Phase B until user approves design via Phase 5.5 decision menu</gate>
</phase>

<phase name="B-implementation-scaffolding">
<purpose>Generate 5 implementation files ONLY after Phase A approval</purpose>
<outputs>
- `v[N]-ui.html` - Production HTML
- `v[N]-PluginEditor.h` - C++ header boilerplate
- `v[N]-PluginEditor.cpp` - C++ implementation boilerplate
- `v[N]-CMakeLists.txt` - WebView build configuration
- `v[N]-integration-checklist.md` - Implementation steps
</outputs>
<location>plugins/[PluginName]/.ideas/mockups/</location>
</phase>
</workflow>

<context_detection>
<standalone_mode>
- No `.continue-here.md` file present
- Generates mockups independently
- Skips state updates
</standalone_mode>

<workflow_mode>
- File `plugins/[PluginName]/.continue-here.md` exists with `current_stage` field
- Updates workflow state after each phase
</workflow_mode>

Check for `.continue-here.md` existence to determine mode. If present, update state files. If absent, skip state updates.
</context_detection>

<phase name="0-aesthetic-check">
Check if `.claude/aesthetics/manifest.json` exists. If found, count aesthetics using jq.

<menu>
Found {N} saved aesthetics in library.

How would you like to start the UI design?
1. Start from aesthetic template - Apply saved visual system
2. Start from scratch - Create custom design
3. List all aesthetics - Browse library before deciding

Choose (1-3): _
</menu>

<routing>
- Option 1: Display aesthetics from manifest with metadata (name, vibe, colors, source). Invoke ui-template-library skill with "apply" operation. Skip to Phase 4 with generated mockup.
- Option 2: Continue to Phase 1 (load context).
- Option 3: Invoke ui-template-library skill with "list" operation, show previews, return to menu.
- No aesthetics: Skip directly to Phase 1.
</routing>

See `references/aesthetic-integration.md` for complete integration details.
</phase>

<phase name="1-load-context">
Load context from creative brief and extract requirements.
See `references/phase-details.md` for extraction protocol.
See `references/context-extraction.md#example-extracting-from-creative-brief` for examples.
</phase>

<phase name="1.5-context-aware-prompt">
Adapt initial prompt based on creative brief contents (rich/minimal/zero UI details).
See `references/phase-details.md` for prompt templates.
</phase>

<phase name="2-gap-analysis">
Analyze gaps in Tier 1 (Critical), Tier 2 (Visual), Tier 3 (Polish) requirements.
See `references/phase-details.md` for tier definitions.
See `references/design-questions.md` for question templates.
</phase>

<phase name="2.5-calculate-dimensions">
Calculate recommended window size before asking user.
See `references/phase-details.md` for calculation steps.
See `references/layout-validation.md` Section 2 for formulas.
</phase>

<phase name="3-question-batch">
Generate exactly 4 questions using AskUserQuestion based on identified gaps.
See `references/phase-details.md` for rules and question format.
See `references/design-questions.md#example-question-batches` for templates.
</phase>

<phase name="3.5-decision-gate">
Present decision gate with AskUserQuestion: finalize, ask more questions, or add context.
Route to Phase 4, back to Phase 2, or collect free-form text.
See `references/phase-details.md` for menu format and routing.
</phase>

<phase name="4-5.45-design-agent">
Invoke ui-design-agent via Task tool to generate YAML + test HTML.

Include in invocation prompt:
- All gathered requirements (layout, controls, colors, etc.)
- Quality expectation: "Design must look like commercial $50-200 audio plugin - intentional decisions, not defaults"

See `references/phase-details.md` for invocation summary.
See `references/delegation-protocols.md` for complete protocol.
</phase>

<phase name="5.5-design-decision-menu">
<gate>CRITICAL: Phases 6-10 ONLY execute if user chooses option 2 (Finalize)</gate>

<menu>
Mockup v[N] design created (2 files)

Files generated:
- v[N]-ui.yaml (design specification)
- v[N]-ui-test.html (browser-testable mockup)

What would you like to do?

1. Iterate - Refine design, adjust layout
2. Finalize - Validate alignment and complete mockup
3. Save as template - Add to aesthetic library for reuse
4. Other

Choose (1-4): _
</menu>

<routing>
- Option 1 (Iterate): Collect refinement feedback, return to Phase 2 with new version (v2, v3, etc.), invoke NEW ui-design-agent instance (fresh context).
- Option 2 (Finalize): Proceed to Phase 5.6 (automatic brief update), mockup is source of truth for UI sections, no user interaction needed, continue to Phase 6-10 after brief sync.
- Option 3 (Save as template): Invoke ui-template-library skill with "save" operation, after saving return to Phase 5.5 menu.
- Option 4 (Other): Handle custom request (test in browser, validate constraints, etc.).
</routing>

<iteration_guidance>
When collecting feedback for Option 1 (Iterate):

**If user provides specific changes** ("make it vertical", "add a meter", "change colors to blue"):
- Pass these as explicit requirements to new ui-design-agent instance

**If user provides vague improvement requests** ("make it better", "improve it", "polish it"):
- Prompt for specifics: "What aspect should I focus on? (layout, colors, spacing, controls)"
- If user says "everything" or "overall quality": Pass instruction to refine existing elements (spacing, color harmony, control styling) rather than add new elements
</iteration_guidance>

See `references/decision-menus.md#phase-5-5-design-decision-menu` for detailed routing.
</phase>

<phase name="5.6-update-brief">
Automatic update of creative brief from finalized mockup.
Triggered by "Finalize" selection in Phase 5.5.
No user interaction required.
See `references/phase-details.md` for protocol.
</phase>

<phase name="6-10-implementation">
<prerequisites>
- User confirmed design in Phase 5.5 (selected option 2: Finalize)
- Phase A files exist (`v[N]-ui.yaml`, `v[N]-ui-test.html`)
- Finalization marker present in YAML
</prerequisites>

See `references/phase-b-enforcement.md` for guard implementation.
</phase>

<phase name="6-10.5-finalization-agent">
Invoke ui-finalization-agent via Task tool to generate 5 implementation files.
See `references/phase-details.md` for invocation summary.
See `references/delegation-protocols.md` for complete protocol.
</phase>

<phase name="10.7-completion-menu">
Present completion menu after ui-finalization-agent returns successfully.
See `references/decision-menus.md#completion-menu` for format and routing.
</phase>

<orchestration_protocol>
<delegation_rules>
This skill NEVER generates mockup files directly. ALL file generation delegated to subagents.

<sequence>
1. Orchestrator: Gather requirements (Phases 0-3.5)
2. Orchestrator: Invoke ui-design-agent via Task tool
3. ui-design-agent: Generate YAML + test HTML, commit, return JSON
4. Orchestrator: Parse JSON, present Phase 5.5 menu
5. If iterate: Orchestrator invokes NEW ui-design-agent
6. If finalize: Orchestrator updates brief (Phase 5.6)
7. Orchestrator: Invoke ui-finalization-agent via Task tool
8. ui-finalization-agent: Generate 5 files, commit, return JSON
9. Orchestrator: Parse JSON, present completion menu
</sequence>

See `references/delegation-protocols.md` for enforcement details.
</delegation_rules>

<state_management>
Subagents update `.continue-here.md` with their phase results. Orchestrator verifies `stateUpdated` flag in JSON report.

After subagent returns `stateUpdated: true`, verify actual state contents match expected values (not just boolean flag).

Read `.continue-here.md`, parse YAML, check specific fields match JSON report values. If mismatch, present state recovery menu.

See `references/state-tracking.md` for complete state schema and verification protocol.
</state_management>

<iteration_protocol>
Each iteration runs in fresh agent context. User chooses "Iterate" then orchestrator collects feedback and invokes NEW ui-design-agent with incremented version. Fresh context prevents context window bloat during iterative design.
</iteration_protocol>

<error_handling>
- Agent failures: Present error menu (retry/manual fix/debug/cancel).
- Validation failures: Agent returns `validationPassed: false` - present error menu.
- State update failures: Agent returns `stateUpdated: false` - present state recovery menu (verify/manual update/continue anyway).
</error_handling>
</orchestration_protocol>

<versioning_strategy>
Pattern: v1, v2, v3... Each version saved separately.

Purpose: Explore layouts without losing work, A/B test designs, keep history for rollback.

File naming: All 7 files prefixed with version (e.g., `v2-ui.html`, `v2-PluginEditor.h`).

Implementation: Latest version used for Stage 3 (GUI) unless user specifies different version.

See `references/versioning.md` for file management details.
</versioning_strategy>

<success_criteria>
<design_phase>
- YAML spec generated matching user requirements
- Browser test HTML works (interactive controls, parameter messages)
- Visual quality meets commercial standard (intentional design, not defaults)
- Design files committed to git
- `.continue-here.md` updated with version (if workflow mode)
- User presented with Phase 5.5 decision menu
- Design approved OR user iterates with refinements
</design_phase>

<implementation_phase>
- All 7 files generated and saved to `.ideas/mockups/`
- Production HTML complete (no placeholders)
- C++ boilerplate matches YAML structure (correct parameter bindings)
- parameter-spec.md generated and locked (v1 only)
- Implementation files committed to git
- `.continue-here.md` updated with finalization status (if workflow mode)
</implementation_phase>
</success_criteria>

<integration_points>
<invoked_by>
- `/dream` command - After creative brief, before implementation
- `plugin-workflow` skill - During Stage 0 (UI design phase)
- `plugin-improve` skill - When redesigning existing plugin UI
- Natural language: "Design UI for [PluginName]", "Create mockup for compressor"
</invoked_by>

<always_invokes>
- `ui-design-agent` subagent (Phase 4-5.45) - REQUIRED for design iteration
- `ui-finalization-agent` subagent (Phase 6-10.5) - REQUIRED for implementation files
</always_invokes>

<also_invokes>
- `ui-template-library` skill (if user saves aesthetic)
</also_invokes>

<creates>
- `plugins/[Name]/.ideas/mockups/v[N]-*.{yaml,html,h,cpp,txt,md}` (7 files)
- `plugins/[Name]/.ideas/parameter-spec.md` (if v1 and doesn't exist)
</creates>

<updates>
- `PLUGINS.md` - Mark UI designed (if workflow)
- `.continue-here.md` - Update workflow state (if workflow)
</updates>

<blocks>
- Stage 1 (Foundation) - Cannot proceed without parameter-spec.md
- Stage 3 (GUI) - Cannot implement without approved UI mockup
</blocks>
</integration_points>

<reference_documentation>
Progressive disclosure - load references when reaching specific phases:

- **Phase 0:** `references/aesthetic-integration.md` - Aesthetic library integration
- **Phase 1:** `references/context-extraction.md` - Creative brief extraction
- **Phase 2-3:** `references/design-questions.md` - Question templates and tiering
- **Phase 2.5:** `references/layout-validation.md` - Dimension calculation formulas
- **Phase 4-5.45:** `references/delegation-protocols.md` - Subagent invocation
- **Phase 5.5:** `references/decision-menus.md` - Menu format and routing
- **Phase 5.6:** `references/state-tracking.md` - Brief sync protocol
- **Phase 6-10:** `references/phase-b-enforcement.md` - Phase B guard
- **Anti-patterns:** `references/common-pitfalls.md` - What to avoid

Technical details:
- `references/html-generation.md` - Production HTML rules
- `references/browser-testing.md` - Browser test workflow
- `references/cmake-configuration.md` - WebView build settings
- `references/cpp-boilerplate-generation.md` - C++ template generation
- `references/ui-design-rules.md` - Design constraints and patterns
- `references/versioning.md` - File management
</reference_documentation>

<template_assets>
- `assets/ui-yaml-template.yaml` - YAML structure
- `assets/webview-boilerplate.md` - C++ integration templates
- `assets/integration-checklist-template.md` - Integration guide
- `assets/parameter-spec-template.md` - Parameter specification format
</template_assets>
