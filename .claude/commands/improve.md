---
name: improve
description: Modify completed plugins with versioning
---

# /improve

When user runs `/improve [PluginName] [description?]`, invoke the plugin-improve skill.

## Preconditions

**Check PLUGINS.md status:**
- Plugin MUST exist
- Status MUST be ✅ Working OR 📦 Installed
- Status MUST NOT be 🚧 In Development

**If status is 🚧:**
Reject with message:
```
[PluginName] is still in development (Stage [N]).
Complete the workflow first with /continue [PluginName].
Cannot use /improve on in-progress plugins.
```

**If status is 💡:**
Reject with message:
```
[PluginName] is not implemented yet (Status: 💡 Ideated).
Use /implement [PluginName] to build it first.
```

## Behavior

**Without plugin name:**
Present menu of completed plugins (✅ or 📦 status).

**With plugin, no description:**
Ask what to improve:
```
What would you like to improve in [PluginName]?

1. From existing brief ([feature].md found in improvements/)
2. Describe the change
```

**With vague description:**
Detect vagueness (lacks specific feature, action, or criteria).

Present choice:
```
Your request is vague. How should I proceed?

1. Brainstorm approaches first → creates improvement brief
2. Just implement something reasonable → Phase 0.5 investigation
```

**With specific description:**
Proceed directly to plugin-improve skill with Phase 0.5 investigation.

## Vagueness Detection

Request IS vague if it lacks:
- Specific feature name
- Specific action
- Acceptance criteria

Examples of VAGUE:
- "improve the filters"
- "better presets"
- "UI feels cramped"

Examples of SPECIFIC:
- "add resonance parameter to filter, range 0-1"
- "fix bypass parameter - not muting audio"
- "increase window height to 500px"

## Plugin-Improve Workflow

The plugin-improve skill executes:
1. **Phase 0.5:** Investigation (root cause analysis)
2. **Approval:** Wait for user confirmation
3. **Version selection:** Ask PATCH/MINOR/MAJOR
4. **Backup:** Copy to backups/[Plugin]/v[X.Y.Z]/
5. **Implementation:** Make code changes
6. **CHANGELOG:** Update with version and description
7. **Git commit:** Conventional format with version
8. **Git tag:** Tag release (v[X.Y.Z])
9. **Build:** Compile Release mode
10. **Install:** Deploy to system folders

## Output

Changes are production-ready:
- Versioned and backed up
- Built in Release mode
- Installed to system folders
- Documented in CHANGELOG
- Git history preserved
