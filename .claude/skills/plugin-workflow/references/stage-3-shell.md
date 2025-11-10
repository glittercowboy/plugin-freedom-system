# Stage 3: Shell

**Context:** This file is part of the plugin-workflow skill.
**Invoked by:** Main workflow dispatcher after Stage 2 completion
**Purpose:** Implement ALL parameters with APVTS, plugin loads in DAW with parameter system

---

**Goal:** Implement ALL parameters, plugin loads in DAW with parameter system

**Duration:** 5 minutes

**Preconditions:**
- Stage 2 complete (build system operational)
- parameter-spec.md exists (from finalized UI mockup)

## Actions

### 1. Precondition Check: parameter-spec.md REQUIRED

**BLOCKING check before proceeding:**

```bash
if [ ! -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" ]; then
  echo "Cannot proceed to Stage 3 - parameter-spec.md missing"
  echo "Complete UI mockup workflow first (/mockup)"
  exit 1
fi
```

**If parameter-spec.md missing:** STOP IMMEDIATELY, exit skill, wait for user to create contract via ui-mockup skill.

### 2. Prepare Contracts for Subagent

Read contract files that shell-agent needs:

```bash
cat plugins/[PluginName]/.ideas/parameter-spec.md
cat plugins/[PluginName]/.ideas/creative-brief.md
cat plugins/[PluginName]/.ideas/architecture.md
```

### 3. Invoke shell-agent via Task Tool

Call shell-agent subagent with complete specification:

**CRITICAL:** Use JUCE 8 ParameterID format: `juce::ParameterID { "id", 1 }`

shell-agent implements:
- APVTS with ALL parameters from parameter-spec.md
- Parameter IDs matching spec exactly (zero-drift)
- State management (save/load)
- processBlock stub (no DSP yet)

### 4. Parse JSON Report

Use same `parseSubagentReport()` function with robust error handling.

### 5. Validate shell-agent Report

Check that:
- All parameters from parameter-spec.md were implemented
- No parameters missing (zero-drift validation)
- APVTS created successfully
- parameter_count matches spec

### 6. Handle Success/Failure

**If success:**
- Display parameters implemented (list with IDs, types, ranges)
- Show APVTS status
- Show parameter count

**If failure:**
- 4-option menu (investigate, show code, show spec, manual fix)
- Special handling for parameter_mismatch error (contract violation)

### 7. Update State Files

```typescript
updateHandoff(pluginName, 3, `Stage 3: Shell - ${report.outputs.parameter_count} parameters implemented`, ...)
updatePluginStatus(pluginName, "🚧 Stage 3")
updatePluginTimeline(pluginName, 3, `Shell complete - ${report.outputs.parameter_count} parameters`)
```

### 8. Git Commit

```bash
git add plugins/[PluginName]/Source/
git add plugins/[PluginName]/.continue-here.md
git add PLUGINS.md

git commit -m "feat: [PluginName] Stage 3 - shell"
```

### 9. Decision Menu

```
✓ Stage 3 complete: Parameter system operational

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

**Return to:** Main plugin-workflow orchestration in `SKILL.md`
