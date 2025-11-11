---
name: reset-to-ideation
description: Roll back plugin to ideation stage - keep idea/mockups, remove all implementation
args: "[PluginName]"
---

# /reset-to-ideation - Surgical Rollback

**Purpose:** Direct shortcut to reset plugin to ideation stage. Invokes plugin-lifecycle skill with mode: 'reset'.

**Use case:** Implementation went wrong, but the concept and UI design are solid. Start fresh from Stage 0.

## Behavior

When user runs `/reset-to-ideation [PluginName]`, invoke the plugin-lifecycle skill with mode: 'reset'.

## Quick Reference

**What gets preserved:**
- Creative brief (`.ideas/creative-brief.md`)
- UI mockups (`.ideas/mockups/`)
- Parameter specifications (`.ideas/parameter-spec.md`)

**What gets removed:**
- Source code (`Source/` directory)
- Build configuration (`CMakeLists.txt`)
- Implementation docs (`.ideas/architecture.md`, `.ideas/plan.md`)
- Build artifacts and installed binaries

**Status change:** [Any] → 💡 Ideated

## Implementation

See `.claude/skills/plugin-lifecycle/references/mode-3-reset.md` for complete rollback workflow.

## Example Confirmation

```
⚠️  Rolling back to ideation stage

Will REMOVE:
- Source code: plugins/[PluginName]/Source/ (47 files)
- Build config: plugins/[PluginName]/CMakeLists.txt
- Implementation docs: architecture.md, plan.md
- Binaries: VST3 + AU (if installed)
- Build artifacts

Will PRESERVE:
- Idea: creative-brief.md
- Mockups: 2 versions (v1-ui, v2-ui)
- Parameters: parameter-spec.md

A backup will be created in backups/rollbacks/

Status will change: [Current] → 💡 Ideated

Continue? (y/N): _
```

## Success Output

```
✓ [PluginName] reset to ideation stage

Removed:
- Source code: 47 files
- Build configuration: CMakeLists.txt
- Implementation docs: architecture.md, plan.md
- Binaries: VST3 + AU (uninstalled)
- Build artifacts

Preserved:
- Creative brief: creative-brief.md ✓
- UI mockups: 2 versions ✓
- Parameters: parameter-spec.md ✓

Backup available at:
backups/rollbacks/[PluginName]_20251110_235612.tar.gz

Status changed: [Previous] → 💡 Ideated

Next steps:
1. Review mockups and parameter spec
2. Run /implement [PluginName] to start fresh from Stage 0
3. Or modify creative brief and re-run /dream
```

## Routes To

`plugin-lifecycle` skill (mode: 'reset')

## Related Commands

- `/destroy` - Complete removal with backup
- `/uninstall` - Remove binaries only, keep source
- `/clean` - Interactive menu to choose cleanup operation
