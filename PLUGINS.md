# PLUGIN REGISTRY

## State Legend

- **💡 Ideated** - Creative brief exists, no implementation
- **🚧 Stage N** - In development (specific stage number)
- **✅ Working** - Completed Stage 6, not installed
- **📦 Installed** - Deployed to system folders
- **🐛 Has Issues** - Known problems (combines with other states)
- **🗑️ Archived** - Deprecated

## State Machine Rules

**CRITICAL:** Only ONE plugin can be 🚧 at a time (prevents concurrent modification bugs)

- If status is 🚧: ONLY plugin-workflow can modify (use `/continue` to resume)
- plugin-improve blocks if status is 🚧 (must complete workflow first)

## Plugin Registry

| Plugin Name | Status | Version | Last Updated |
|-------------|--------|---------|--------------|
| | | | |

## Entry Template

```markdown
### [PluginName]
**Status:** [Emoji] **[State Name]**
**Created:** YYYY-MM-DD
**Version:** X.Y.Z (if applicable)
**Type:** [Audio Effect | MIDI Instrument | Synth]

**Description:**
[Brief description]

**Parameters ([N] total):**
- [Param1]: [range], default [value]
- [Param2]: [range], default [value]

**DSP:** [Architecture description]

**GUI:** [Design description]

**Lifecycle Timeline:**
- **YYYY-MM-DD (Stage 0):** Research completed
- **YYYY-MM-DD (Stage 1):** Planning - Complexity N

**Known Issues:**
- None

**Last Updated:** YYYY-MM-DD
```
