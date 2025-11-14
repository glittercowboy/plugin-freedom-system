# .continue-here.md Format Specification

## Purpose

The `.continue-here.md` file is the authoritative handoff file that preserves workflow context across sessions. It allows workflow resumption without context loss or checkpoint amnesia.

## Location

Always created at: `plugins/{PluginName}/.continue-here.md`

## Format

```yaml
---
plugin: [PluginName]
workflow: [workflow-name]
stage: [0-6] (for plugin-workflow only)
phase: [phase-number] (for other workflows)
status: [workflow-specific status]
last_updated: [ISO 8601 timestamp]
gui_type: [headless|webview] (optional - for plugin-workflow)
mode: [express|manual] (optional - workflow execution mode)
checkpoint_phase: [phase-identifier] (optional - for multi-phase checkpoints)
---

# [Workflow Name] - [Stage/Phase] - [Status]

[Optional prose description of current state and next steps]
```

## Field Definitions

### Required Fields

- **plugin**: Plugin name (matches directory name in `plugins/`)
- **workflow**: Workflow name (plugin-workflow, ui-mockup, plugin-improve, etc.)
- **last_updated**: ISO 8601 timestamp (YYYY-MM-DDTHH:MM:SSZ)

### Conditional Fields

- **stage**: Required for plugin-workflow (values: 0-6)
- **phase**: Required for ui-mockup, plugin-ideation, etc.
- **status**: Workflow-specific status (e.g., "in_progress", "paused", "complete")

### Optional Fields

- **gui_type**: Plugin UI type - "headless" (uses DAW controls) or "webview" (custom UI). Defaults to "webview" if not present. Only relevant for plugin-workflow.
- **mode**: Workflow execution mode - "express" (auto-progress) or "manual" (decision menus). Controls checkpoint behavior.
- **checkpoint_phase**: Phase identifier for multi-phase checkpoints (e.g., "3.1", "3.2"). Allows resuming within a stage.

## Examples

### plugin-workflow Stage 2

```yaml
---
plugin: DriveVerb
workflow: plugin-workflow
stage: 3
status: in_progress
last_updated: 2025-11-12T14:30:00Z
---

# plugin-workflow - Stage 2 - In Progress

DSP implementation for reverb algorithm. Currently implementing early reflections.
Next: Complete late reverb diffusion network.
```

### ui-mockup Phase 4

```yaml
---
plugin: DriveVerb
workflow: ui-mockup
phase: 4
status: iteration
last_updated: 2025-11-12T10:15:00Z
---

# ui-mockup - Phase 4 - Iteration

Design iteration for reverb controls. User requested darker theme with neon accents.
Next: Generate v2-ui.yaml with updated color palette.
```

## Reconciliation Usage

During reconciliation, this file is:
1. Read to detect workflow context (Phase 1: Context Detection)
2. Validated against expected stage/phase rules (Phase 2: Rule Loading)
3. Updated if gaps found (Phase 5: Remediation)
4. Committed with workflow-appropriate message
