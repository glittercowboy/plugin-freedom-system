# Integration Contracts

Documentation of all skill-to-skill and skill-to-subagent data contracts in the Plugin Freedom System.

## Purpose

This document maps all integration points, data formats, and error handling between system components. It ensures that skills can invoke each other reliably without undocumented assumptions causing breakage.

## Schema References

All JSON reports must validate against schemas in `.claude/schemas/`:

- **subagent-report.json** - Reports from foundation-foundation-shell-agent (shell portion), foundation-shell-agent (shell portion), dsp-agent, gui-agent
- **validator-report.json** - Reports from validator agent

See `.claude/schemas/README.md` for validation examples.

## Skill-to-Skill Integrations

### context-resume → plugin-workflow

**When:** Resuming workflow at specific stage (Stages 0-6)

**Sends:**
- Plugin name (string)
- Stage number (integer 0-6)
- Phase (string or null)
- Orchestration mode (boolean, always true)
- Handoff context (full .continue-here.md content)

**Expects:**
- plugin-workflow takes control
- Resumes at specified stage/phase
- Presents checkpoint decision menu when complete

**Error handling:**
- Missing handoff → context-resume offers reconstruction
- Corrupted YAML → context-resume parses git log
- Stage mismatch → context-resume disambiguates

**Contract:** context-resume is READ-ONLY. Never modifies state files. plugin-workflow owns all state updates.

---

### context-resume → plugin-ideation

**When:** Resuming ideation/improvement brainstorming

**Sends:**
- Plugin name (string)
- Handoff context from `.continue-here.md`
- Creative brief status (optional)
- Mockup status (optional)

**Expects:**
- plugin-ideation presents summary
- Continues ideation session
- Updates handoff when complete

**Error handling:**
- Missing creative brief → plugin-ideation starts from scratch
- Stale handoff → warns user, offers to continue anyway

---

### context-resume → ui-mockup

**When:** Resuming mockup iteration

**Sends:**
- Plugin name (string)
- Mockup version (integer)
- Handoff context from `.ideas/mockups/.continue-here.md`
- Iteration notes (string)

**Expects:**
- ui-mockup loads latest mockup version
- Presents design status
- Continues iteration or finalizes

**Error handling:**
- Missing mockup files → ui-mockup offers to rebuild
- Version mismatch → ui-mockup uses highest version found

---

### context-resume → plugin-improve

**When:** Resuming improvement implementation

**Sends:**
- Plugin name (string)
- Improvement description (string)
- Handoff context (optional)

**Expects:**
- plugin-improve starts at Phase 0 (specificity detection)
- Follows normal improvement workflow
- Creates backup before changes

**Error handling:**
- Plugin not found → blocks with error
- Plugin in development → blocks, suggests /continue instead
- Missing source files → reports error, offers recovery

---

### ui-mockup → ui-template-library

**When:** User selects aesthetic template or saves design as template

**Operations:**

#### 1. List aesthetics

**Sends:** `operation: "list"`

**Expects:**
```json
{
  "aesthetics": [
    {
      "id": "vintage-hardware-001",
      "name": "Vintage Hardware",
      "vibe": "Vintage analog",
      "colors": "Orange/cream/brown with paper texture",
      "source_plugin": "TapeAge",
      "source_version": "v2"
    }
  ]
}
```

#### 2. Apply aesthetic

**Sends:**
```json
{
  "operation": "apply",
  "aesthetic_id": "vintage-hardware-001",
  "plugin_name": "NewPlugin",
  "parameters": [
    {"id": "gain", "label": "Gain", "type": "slider"},
    {"id": "tone", "label": "Tone", "type": "knob"}
  ]
}
```

**Expects:**
```json
{
  "success": true,
  "mockup_files": {
    "html": "<full html>",
    "css": "<full css>",
    "js": "<full js>"
  },
  "version": 1
}
```

#### 3. Save aesthetic

**Sends:**
```json
{
  "operation": "save",
  "name": "Vintage Hardware",
  "vibe": "Vintage analog",
  "html": "<html content>",
  "css": "<css content>",
  "source_plugin": "TapeAge",
  "source_version": "v2"
}
```

**Expects:**
```json
{
  "success": true,
  "aesthetic_id": "vintage-hardware-001",
  "saved_to": ".claude/aesthetics/vintage-hardware-001/"
}
```

**Error handling:**
- Missing aesthetic ID → ui-template-library returns error, ui-mockup falls back to manual
- Invalid parameters → ui-template-library validates, returns specific error
- Save conflict → ui-template-library auto-increments ID (vintage-hardware-002)

**Contract:** ui-mockup owns mockup files. ui-template-library only provides templates, never modifies plugin files directly.

---

### plugin-improve → deep-research

**When:** Complex issues requiring investigation (Tier 3)

**Sends:**
```json
{
  "plugin_name": "MyPlugin",
  "issue_description": "Filters sound harsh at high resonance",
  "investigation_tier": 3,
  "context": {
    "source_files": ["Source/PluginProcessor.cpp"],
    "build_log": "logs/build.log",
    "test_results": "filters passed pluginval"
  }
}
```

**Expects:**
```json
{
  "findings": [
    "Issue: Resonance parameter not clamped",
    "Root cause: juce::IIRFilter allows Q > 20",
    "Solution: Add parameter normalization in processBlock"
  ],
  "confidence": "high",
  "routing_directive": "Invoke plugin-improve skill",
  "proposed_changes": [
    {
      "file": "Source/PluginProcessor.cpp",
      "action": "Add Q clamping before filter update"
    }
  ]
}
```

**Error handling:**
- Investigation inconclusive → deep-research returns findings with confidence: "low"
- File not found → deep-research reports missing context, continues anyway
- Build failure → deep-research captures log, includes in findings

**Contract:** deep-research NEVER modifies code. Only investigates and reports. plugin-improve handles all implementation.

---

### plugin-improve → plugin-testing

**When:** Regression testing after changes (Phase 6)

**Sends:**
```json
{
  "plugin_name": "MyPlugin",
  "baseline_build": "builds/MyPlugin-v1.0.0/",
  "new_build": "builds/MyPlugin-v1.0.1/",
  "test_suite": ["stability", "parameters", "pluginval"]
}
```

**Expects:**
```json
{
  "baseline_results": {
    "stability": "PASS",
    "pluginval": "PASS"
  },
  "new_results": {
    "stability": "PASS",
    "pluginval": "PASS"
  },
  "regression_detected": false,
  "summary": "All tests passed. No regressions detected."
}
```

**Error handling:**
- Baseline build missing → plugin-testing skips baseline comparison, tests new build only
- Test crash → plugin-testing captures crash log, marks test as FAIL
- pluginval not found → plugin-testing skips pluginval, continues with other tests

**Contract:** plugin-testing is READ-ONLY. Never modifies builds or source. Only tests and reports.

---

### plugin-improve → build-automation

**When:** Building plugin after changes (Phase 5)

**Sends:**
```json
{
  "plugin_name": "MyPlugin",
  "build_type": "Release",
  "operation": "build"
}
```

**Expects:**
```json
{
  "success": true,
  "build_log": "logs/MyPlugin/build-2025-11-12.log",
  "binary_path": "builds/MyPlugin/Release/MyPlugin.vst3",
  "warnings": 0,
  "errors": 0
}
```

**Error handling:**
- Build failure → build-automation returns success: false, includes error excerpt
- CMake error → build-automation returns specific CMake error
- Missing dependencies → build-automation checks prerequisites, reports missing

**Contract:** build-automation owns all build operations. Skills NEVER invoke cmake/xcodebuild directly.

---

### plugin-improve → plugin-lifecycle

**When:** User requests installation after improvement

**Sends:**
```json
{
  "plugin_name": "MyPlugin",
  "operation": "install",
  "binary_path": "builds/MyPlugin/Release/MyPlugin.vst3"
}
```

**Expects:**
```json
{
  "success": true,
  "installed_to": [
    "/Library/Audio/Plug-Ins/VST3/MyPlugin.vst3"
  ],
  "cache_cleared": true
}
```

**Error handling:**
- Permission denied → plugin-lifecycle returns error with sudo command
- Binary not found → plugin-lifecycle validates path, returns specific error
- Install conflict → plugin-lifecycle offers to backup existing version

**Contract:** plugin-lifecycle owns system-level operations. Skills NEVER modify /Library directly.

---

### plugin-workflow → foundation-foundation-shell-agent (shell portion)

**When:** Stage 2 (Foundation) implementation

**Sends:**
- Plugin name (string)
- Contract files (creative-brief.md, parameter-spec.md, architecture.md, plan.md)
- Stage directive: "Implement Stage 2"

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`:

```json
{
  "agent": "foundation-foundation-shell-agent (shell portion)",
  "status": "success",
  "outputs": {
    "plugin_name": "MyPlugin",
    "source_files_created": [
      "Source/PluginProcessor.h",
      "Source/PluginProcessor.cpp",
      "Source/PluginEditor.h",
      "Source/PluginEditor.cpp",
      "CMakeLists.txt"
    ]
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

**Error handling:**
- Missing contract → foundation-foundation-shell-agent (shell portion) returns status: "failure", specific error
- File creation error → foundation-foundation-shell-agent (shell portion) reports which file failed
- Invalid plugin type → foundation-foundation-shell-agent (shell portion) validates architecture.md, returns error

**Contract:** foundation-foundation-shell-agent (shell portion) creates files, returns report. plugin-workflow validates schema, commits changes, updates state.

---

### plugin-workflow → foundation-shell-agent (shell portion)

**When:** Stage 3 (Shell) implementation

**Sends:**
- Plugin name (string)
- Contract files (parameter-spec.md)
- Existing source files
- Stage directive: "Implement Stage 3"

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`:

```json
{
  "agent": "foundation-shell-agent (shell portion)",
  "status": "success",
  "outputs": {
    "plugin_name": "MyPlugin",
    "parameters_implemented": [
      {
        "id": "gain",
        "type": "Float",
        "range": "-60.0 to 12.0",
        "default": "0.0",
        "units": "dB"
      }
    ],
    "parameter_count": 5,
    "apvts_created": true,
    "state_management_implemented": true
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

**Error handling:**
- Parameter mismatch → foundation-shell-agent (shell portion) returns missing/extra parameters in outputs
- APVTS creation failed → foundation-shell-agent (shell portion) returns apvts_created: false with error
- Validation failure → foundation-shell-agent (shell portion) returns status: "failure"

**Contract:** foundation-shell-agent (shell portion) modifies files, returns report. plugin-workflow validates schema and parameter counts.

---

### plugin-workflow → dsp-agent

**When:** Stage 4 (DSP) implementation

**Sends:**
- Plugin name (string)
- Contract files (architecture.md, parameter-spec.md)
- Existing source files
- Stage directive: "Implement Stage 4 [Phase X]"
- Phase number (if phased approach)

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`:

```json
{
  "agent": "dsp-agent",
  "status": "success",
  "outputs": {
    "plugin_name": "MyPlugin",
    "dsp_components": [
      "juce::dsp::StateVariableFilter",
      "juce::dsp::Gain"
    ],
    "processing_chain": "Input → Filter → Gain → Output"
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

**Error handling:**
- Missing DSP component → dsp-agent reports which component not found
- Real-time safety violation → dsp-agent warns in issues array
- Phase incomplete → dsp-agent returns ready_for_next_stage: false

**Contract:** dsp-agent implements DSP, returns report. plugin-workflow handles phase checkpoints and state updates.

---

### plugin-workflow → gui-agent

**When:** Stage 5 (GUI) implementation

**Sends:**
- Plugin name (string)
- Contract files (parameter-spec.md, mockup files)
- Existing source files
- Stage directive: "Implement Stage 5"

**Expects:** JSON report conforming to `.claude/schemas/subagent-report.json`:

```json
{
  "agent": "gui-agent",
  "status": "success",
  "outputs": {
    "plugin_name": "MyPlugin",
    "webview_integrated": true,
    "ui_files_created": [
      "Resources/index.html",
      "Resources/styles.css",
      "Resources/script.js"
    ],
    "relays_created": 5,
    "attachments_created": 5
  },
  "issues": [],
  "ready_for_next_stage": true
}
```

**Error handling:**
- Missing mockup → gui-agent returns error, cannot proceed
- Relay mismatch → gui-agent reports parameter count mismatch
- WebView integration failed → gui-agent returns webview_integrated: false

**Contract:** gui-agent integrates UI, returns report. plugin-workflow validates relay/attachment counts match parameters.

---

### plugin-workflow → validator

**When:** After each stage completion (Stages 1-5)

**Sends:**
- Stage number (integer 0-6)
- Plugin name (string)
- Contract files (stage-specific)
- Expected outputs (stage-specific list)

**Expects:** JSON report conforming to `.claude/schemas/validator-report.json`:

```json
{
  "agent": "validator",
  "stage": 3,
  "status": "PASS",
  "checks": [
    {
      "name": "parameter_count_match",
      "passed": true,
      "message": "Expected 5 parameters, found 5",
      "severity": "info"
    }
  ],
  "recommendation": "Proceed to Stage 4",
  "continue_to_next_stage": true
}
```

**Error handling:**
- Validation failure → validator returns status: "FAIL", specific checks
- Override file exists → validator suppresses false positives
- Missing expected files → validator reports in checks array

**Contract:** validator is advisory, not blocking. User makes final decision. plugin-workflow presents validation results in checkpoint menu.

---

### plugin-workflow → build-automation

**When:** After stage completion (Stages 2-6)

**Sends:**
```json
{
  "plugin_name": "MyPlugin",
  "build_type": "Debug",
  "operation": "build"
}
```

**Expects:**
```json
{
  "success": true,
  "build_log": "logs/MyPlugin/build-2025-11-12.log",
  "binary_path": "builds/MyPlugin/Debug/MyPlugin.vst3",
  "warnings": 0,
  "errors": 0
}
```

**Error handling:**
- Build failure → build-automation returns success: false, plugin-workflow presents error
- Missing CMakeLists.txt → build-automation validates prerequisites
- Compiler not found → build-automation checks system requirements

**Contract:** plugin-workflow NEVER builds directly. Always delegates to build-automation.

---

### troubleshooting-docs → juce8-critical-patterns.md

**When:** User promotes solved problem to Required Reading

**Sends:**
- Problem title (string)
- Symptom (string)
- Root cause (string)
- Solution (string)
- Code example (string)

**Expects:**
- New entry appended to juce8-critical-patterns.md
- YAML frontmatter updated (pattern count, last updated)
- Entry includes all required fields

**Error handling:**
- Duplicate pattern → troubleshooting-docs checks existing patterns, warns user
- Missing required field → troubleshooting-docs prompts for missing info
- File locked → troubleshooting-docs retries or reports error

**Contract:** troubleshooting-docs owns Required Reading updates. Subagents READ-ONLY juce8-critical-patterns.md via SessionStart hook.

---

## Decision Menu Protocol

All skills must follow consistent decision-making patterns:

### Use AskUserQuestion Tool ONLY When:

- In ideation/design phases (/dream, mockup design)
- Exploring creative concepts or directions
- 2-4 conceptual options (not actions)
- Examples:
  - "Which plugin concept direction?"
  - "Which UI aesthetic appeals to you?"
  - "Should this feature be subtle or prominent?"

### Use Inline Numbered Menu For:

- Checkpoint decision gates (all workflow navigation)
- Template/item selection from existing options
- ANY menu with >4 options
- Examples:
  - "Stage 3 complete. What's next? 1) Continue..."
  - "Which template? 1) Analog 2) Digital..."
  - "Phase complete. 1) Finalize 2) Iterate..."

**Key Difference:** AskUserQuestion = exploring ideas, Inline Menu = navigating workflow or choosing items

**Format for inline menus:**

```
[Completion statement]

What's next?

1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery option] ← User discovers [feature]
4. [Alternative path]
5. Other

Choose (1-5): _
```

ALWAYS wait for user response. NEVER auto-proceed.

---

## Checkpoint Protocol (System-Wide)

At every significant completion point:

1. Auto-commit changes (if in workflow)
2. Update state files (.continue-here.md, PLUGINS.md)
3. **Verify checkpoint completion** (all steps succeeded)
4. Present numbered decision menu (inline format)
5. WAIT for user response
6. Execute chosen action

This applies to:
- All workflow stages (0-6)
- All subagent completions
- Contract creation
- Any decision point

**Verification:** Before presenting menu, check:
- Git commit succeeded
- State files updated correctly
- No partial updates or corrupted state
- All expected files created/modified

If verification fails, report error and offer recovery options. Never present menu with corrupted state.

---

## Schema Validation Requirements

### plugin-workflow MUST:

1. Validate all subagent reports against `.claude/schemas/subagent-report.json`
2. Validate all validator reports against `.claude/schemas/validator-report.json`
3. Block on schema violations
4. Report specific validation errors to user

### Validation Example:

```python
import json
from jsonschema import validate, ValidationError

def validate_report(report_json, agent_type):
    """Validate report against schema."""
    schema_path = f".claude/schemas/{agent_type}-report.json"

    with open(schema_path) as f:
        schema = json.load(f)

    try:
        validate(instance=report_json, schema=schema)
        return True, None
    except ValidationError as e:
        return False, f"Schema validation failed: {e.message}"

# Usage in plugin-workflow
report = json.loads(subagent_output)
valid, error = validate_report(report, "subagent")

if not valid:
    # Block workflow, report to user
    print(f"ERROR: {error}")
    print("Cannot proceed with invalid report format")
    return
```

### Schema Violations:

When a report fails validation:

1. Log error with full details
2. Block workflow progression
3. Report to user with actionable message
4. Offer recovery options (retry, skip validation, fix manually)
5. NEVER proceed with malformed data

This prevents cascading failures from corrupted state or unexpected data formats.

---

## Error Handling Patterns

### Graceful Degradation

Skills should fail gracefully and offer alternatives:

```
ERROR: ui-template-library skill not available

Falling back to manual mockup creation.
You can install ui-template-library later with:
  /setup install-skill ui-template-library
```

### Specific Error Messages

Always include:
- What failed
- Why it failed
- How to fix it
- Alternative paths forward

Example:
```
ERROR: Cannot invoke deep-research skill

Reason: Task tool not available in current context
Solution: Run /research from main conversation instead
Alternative: Continue with manual investigation (Phase 0.5)
```

### Recovery Options

Present recovery menu when errors occur:

```
Build failed: CMakeLists.txt not found

What should I do?

1. Regenerate CMakeLists.txt - Recreate from template
2. Check foundation stage - Verify Stage 2 completed
3. Manual intervention - Fix manually then retry
4. Abort improvement - Rollback changes

Choose (1-4): _
```

---

## Testing Integration Contracts

Validate contracts work as documented:

### 1. Test skill invocations

```bash
# Test context-resume → plugin-workflow
echo "plugin: TestPlugin
stage: 3
orchestration_mode: true" > plugins/TestPlugin/.continue-here.md

# Invoke /continue TestPlugin
# Verify plugin-workflow receives correct data
```

### 2. Test JSON schemas

```python
# Test subagent report validation
sample_report = {
    "agent": "foundation-foundation-shell-agent (shell portion)",
    "status": "success",
    "outputs": {
        "plugin_name": "TestPlugin",
        "source_files_created": ["Source/PluginProcessor.h"]
    },
    "issues": [],
    "ready_for_next_stage": True
}

valid, error = validate_report(sample_report, "subagent")
assert valid, f"Schema validation failed: {error}"
```

### 3. Test error handling

```bash
# Test missing contract detection
rm plugins/TestPlugin/.ideas/parameter-spec.md

# Invoke foundation-foundation-shell-agent (shell portion)
# Verify returns status: "failure" with specific error
# Verify plugin-workflow blocks progression
```

### 4. Test decision menus

- Present checkpoint menu after stage completion
- Verify WAITS for user input (no auto-proceed)
- Verify all menu options work as documented
- Verify "Other" option accepts free-form text

---

## Maintenance Guidelines

When adding new integration points:

1. **Document the contract here** with send/expect/error sections
2. **Add to relevant SKILL.md** in Integration Contracts section
3. **Create schema if needed** (for structured data)
4. **Write validation code** (for critical integrations)
5. **Test error cases** (not just happy path)
6. **Update decision menu protocol** (if adds new menu type)

When modifying existing integrations:

1. **Update this document first**
2. **Update all affected skills**
3. **Update schemas** (increment version if breaking change)
4. **Test backward compatibility** (or document breaking changes)
5. **Update error handling** (ensure graceful degradation)

This ensures documentation stays synchronized with implementation.
