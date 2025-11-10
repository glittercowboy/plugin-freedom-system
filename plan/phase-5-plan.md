# Phase 5 Implementation Plan

## Prerequisites Status

**Previous Phase(s) Required:** Phase 0, Phase 1, Phase 2, Phase 3, Phase 4

**Deliverables Check:**

### Phase 0 Deliverables:
- ✅ `.claude/` directory structure - EXISTS
- ✅ `.claude/CLAUDE.md` - EXISTS
- ✅ `PLUGINS.md` - EXISTS
- ✅ Contract templates in skill assets - EXISTS

### Phase 1 Deliverables:
- ✅ `.claude/commands/` - EXISTS (9 command files)
- ✅ `.claude/skills/` - EXISTS (8 skill directories)
- ✅ All 7 core skills - EXISTS (plugin-workflow, plugin-ideation, plugin-improve, context-resume, ui-mockup, plugin-testing, plugin-lifecycle, build-automation)

### Phase 2 Deliverables:
- ✅ plugin-workflow Stages 0, 1, 6 - EXISTS (verified in skill)
- ✅ State management (PLUGINS.md, .continue-here.md) - EXISTS
- ✅ Git commit integration - EXISTS

### Phase 3 Deliverables:
- ✅ `.claude/agents/foundation-agent.md` - EXISTS
- ✅ `.claude/agents/shell-agent.md` - EXISTS
- ✅ `.claude/agents/dsp-agent.md` - EXISTS
- ✅ `.claude/agents/gui-agent.md` - EXISTS
- ✅ Dispatcher integration in plugin-workflow - EXISTS

### Phase 4 Deliverables:
- ✅ `scripts/build-and-install.sh` - EXISTS
- ✅ `.claude/skills/build-automation/` - EXISTS
- ✅ `.claude/agents/troubleshooter.md` - EXISTS

**Status:** ✅ Complete

---

## Phase Overview

Phase 5 implements the hybrid validation system that ensures contract adherence and code quality throughout the plugin development workflow. This phase is critical because it prevents "contract drift" - the divergence between what's specified in contracts (parameter-spec.md, architecture.md) and what's actually implemented in code.

**Why This Phase Is Critical:**
- **Zero Drift Guarantee:** Deterministic hooks catch contract violations immediately (within 2s)
- **Quality Assurance:** Semantic validator provides advisory recommendations for complexity ≥4 plugins
- **Fast Feedback:** Layer 1 validation blocks bad implementations before they compound
- **Advisory Layer 2:** Human retains final decision authority on semantic concerns

**Two-Layer Validation Strategy:**
1. **Layer 1 (Hooks):** Fast (1-3s), deterministic pattern matching, blocking on failure
2. **Layer 2 (validator):** Slow (30-60s), semantic analysis, advisory recommendations

---

## Required Documentation

### Architecture Files (MUST READ IN ENTIRETY)

- **@architecture/02-core-abstractions.md** (lines 185-252) - Hybrid validation abstraction and two-layer strategy
- **@architecture/10-extension-architecture.md** (lines 96-165) - Hook system architecture and conditional execution pattern
- **@architecture/13-error-handling-recovery.md** (lines 182-461) - Complete hook patterns with examples, error handling, graceful skip logic
- **@architecture/03-model-selection-extended-thinking-strategy.md** - Model selection for validator (Sonnet, not Haiku)
- **@architecture/06-state-architecture.md** - State transitions and PLUGINS.md management

### Procedure Files (MUST READ IN ENTIRETY)

- **@procedures/agents/troubleshooter.md** - Agent configuration pattern (validator follows similar structure)
- **@procedures/core/checkpoint-system.md** - How hooks integrate with checkpoints
- **@procedures/skills/plugin-workflow.md** - Where validation is invoked in workflow
- **@procedures/examples/** - No specific validation examples, but shows skill integration patterns

---

## Sub-Phase Breakdown

Phase 5 is split into three sub-phases for logical organization:

- **Phase 5a:** Python Validation Scripts - Deterministic contract validators
- **Phase 5b:** Hook Scripts - Lifecycle hooks for validation gates
- **Phase 5c:** Validator Subagent - Semantic quality review

Each sub-phase builds on the previous, creating a complete validation system.

---

## Implementation Sequence

### Phase 5a: Python Validation Scripts

#### Task 1: Verify Existing Validation Scripts

**Description**: Confirm all 4 Python validation scripts exist and follow the specification from architecture/procedure docs.

**Required Reading**:
- @architecture/02-core-abstractions.md (lines 185-252) - Validation layer 1 specification
- @architecture/13-error-handling-recovery.md (lines 223-290) - SubagentStop hook validation details

**Dependencies**: None (Phase 4 complete)

**Implementation Steps**:
1. Verify `.claude/hooks/validators/` directory exists
2. Check for 4 validator scripts:
   - `validate-parameters.py` (Stage 3: parameter-spec.md ↔ APVTS)
   - `validate-dsp-components.py` (Stage 4: architecture.md ↔ processBlock)
   - `validate-gui-bindings.py` (Stage 5: parameters ↔ WebView relays)
   - `validate-foundation.py` (Stage 2: CMakeLists.txt ↔ build verification)
3. Read each script and verify:
   - Shebang: `#!/usr/bin/env python3`
   - Executable permissions: `chmod +x`
   - Exit code convention: 0 = PASS, 1 = FAIL
   - Graceful skip: Exit 0 when not applicable (no plugin found, no contracts yet)
   - Clear error messages to stderr
4. Test each validator independently with a test plugin

**Expected Output**:
- All 4 Python scripts exist in `.claude/hooks/validators/`
- All scripts are executable
- Each script follows exit code convention
- Scripts handle edge cases gracefully (missing files, empty contracts)

**Verification**:
- Automated: `ls -la .claude/hooks/validators/*.py` shows all 4 scripts with execute permissions
- Automated: `python3 .claude/hooks/validators/validate-parameters.py; echo "Exit code: $?"` (should exit 0 when no plugin active)
- Manual: STOP AND ASK LEX: "Please verify that running each validator script manually with RadioMusic plugin (if it exists) produces expected output. Run: `cd plugins/RadioMusic && python3 ../../.claude/hooks/validators/validate-parameters.py` and check output."

---

#### Task 2: Enhance validate-parameters.py (if needed)

**Description**: Ensure validate-parameters.py handles all parameter types (Float, Bool, Choice) and validates type mapping correctly.

**Required Reading**:
- @architecture/02-core-abstractions.md (lines 111-183) - Contract specifications
- @procedures/skills/juce-foundation.md - Parameter management rules

**Dependencies**: Task 1 complete

**Implementation Steps**:
1. Read current `validate-parameters.py` implementation
2. Verify it parses parameter-spec.md table format correctly:
   ```
   | paramID | Type | Range | Default | Label |
   ```
3. Verify it extracts APVTS parameters from PluginProcessor.cpp with JUCE 8 pattern:
   ```cpp
   AudioParameterFloat(ParameterID { "gainParam", 1 }, ...)
   AudioParameterBool(ParameterID { "bypassParam", 1 }, ...)
   AudioParameterChoice(ParameterID { "filterType", 1 }, ...)
   ```
4. Confirm type mapping validation (spec type matches code type)
5. Check error reporting clarity (which parameter, what mismatch)
6. Add any missing validation logic

**Expected Output**:
- `validate-parameters.py` correctly parses all 3 parameter types
- Type mismatches clearly reported
- Missing parameters reported with specific IDs
- Extra parameters generate warnings (not errors)

**Verification**:
- Automated: Create test parameter-spec.md with Float/Bool/Choice parameters
- Automated: Create test PluginProcessor.cpp with matching APVTS
- Automated: Run validator, expect exit 0
- Automated: Remove one parameter from code, run validator, expect exit 1 with clear error
- Manual: STOP AND ASK LEX: "Please create a test plugin with mismatched parameter types (spec says Float, code has Bool) and verify the validator catches this with a clear error message."

---

#### Task 3: Enhance validate-dsp-components.py (if needed)

**Description**: Ensure validate-dsp-components.py parses architecture.md DSP components and verifies they exist in processBlock implementation.

**Required Reading**:
- @architecture/02-core-abstractions.md (lines 111-183) - architecture.md specification
- @procedures/skills/juce-foundation.md - DSP constraints and patterns

**Dependencies**: Task 1 complete

**Implementation Steps**:
1. Read current `validate-dsp-components.py` implementation
2. Verify it parses architecture.md "Core Components" section:
   ```markdown
   ## Core Components
   - **Delay Line** (juce::dsp::DelayLine)
   - **Filter** (juce::dsp::StateVariableTPTFilter)
   - **Reverb** (juce::dsp::Reverb)
   ```
3. Verify it searches PluginProcessor.cpp for component declarations:
   - Member variables: `juce::dsp::DelayLine<float> delayLine;`
   - Usage in processBlock: `delayLine.process(...)`
   - Usage in prepareToPlay: `delayLine.prepare(...)`
4. Check that it validates component initialization in prepareToPlay
5. Verify error reporting includes which component is missing and where

**Expected Output**:
- `validate-dsp-components.py` parses architecture.md Core Components
- Verifies component declarations in PluginProcessor.h
- Verifies component usage in processBlock and prepareToPlay
- Clear error messages for missing components

**Verification**:
- Automated: Create test architecture.md with 3 DSP components
- Automated: Create test PluginProcessor with matching components
- Automated: Run validator, expect exit 0
- Automated: Remove one component from code, run validator, expect exit 1
- Manual: STOP AND ASK LEX: "Please verify with RadioMusic plugin (or a test plugin) that removing a DSP component from processBlock while keeping it in architecture.md causes validation failure with a specific error message."

---

#### Task 4: Enhance validate-gui-bindings.py (if needed)

**Description**: Ensure validate-gui-bindings.py verifies WebView parameter bindings match parameter-spec.md.

**Required Reading**:
- @architecture/12-webview-integration-design.md - WebView parameter binding architecture
- @procedures/webview/04-parameter-binding.md - Relay pattern and attachment specification
- @procedures/webview/best-practices.md (Member order section) - Critical member declaration order

**Dependencies**: Task 1 complete

**Implementation Steps**:
1. Read current `validate-gui-bindings.py` implementation
2. Verify it checks for relay declarations in PluginEditor.h:
   ```cpp
   juce::WebSliderRelay gainRelay;
   juce::WebToggleButtonRelay bypassRelay;
   juce::WebComboBoxRelay filterTypeRelay;
   ```
3. Verify it checks HTML element IDs in `ui/public/index.html` match:
   ```html
   <input type="range" id="gainRelay" />
   <input type="checkbox" id="bypassRelay" />
   <select id="filterTypeRelay">
   ```
4. **CRITICAL**: Verify it checks member declaration order (prevents 90% of WebView crashes):
   ```cpp
   // CORRECT ORDER (destroyed in reverse):
   private:
       juce::WebSliderRelay gainRelay;        // 1. Relays first
       juce::WebBrowserComponent webView;     // 2. WebView second
       juce::WebSliderParameterAttachment attachment; // 3. Attachments last
   ```
5. Verify it checks for parameter attachments matching relays
6. Check error reporting clarity

**Expected Output**:
- `validate-gui-bindings.py` verifies relay declarations
- Verifies HTML element IDs match relay names
- **CRITICAL CHECK**: Validates member declaration order (relays → webView → attachments)
- Verifies parameter attachments exist
- Clear error messages for missing bindings or wrong order

**Verification**:
- Automated: Create test PluginEditor.h with relays + webView + attachments in correct order
- Automated: Create test index.html with matching element IDs
- Automated: Run validator, expect exit 0
- Automated: Swap webView and relay order, run validator, expect exit 1 with "MEMBER ORDER ERROR"
- Manual: STOP AND ASK LEX: "Please verify that the validator catches incorrect member declaration order (webView declared before relays) and produces a clear error explaining the crash risk."

---

#### Task 5: Enhance validate-foundation.py (if needed)

**Description**: Ensure validate-foundation.py verifies CMakeLists.txt exists, is properly configured, and plugin compiles.

**Required Reading**:
- @architecture/11-build-automation-architecture.md - Build system requirements
- @procedures/scripts/build-and-install.md - Build script specification

**Dependencies**: Task 1 complete

**Implementation Steps**:
1. Read current `validate-foundation.py` implementation
2. Verify it checks CMakeLists.txt existence
3. Verify it validates key CMakeLists.txt elements:
   - `juce_add_plugin()` declaration
   - PRODUCT_NAME extraction (for installation)
   - Required JUCE modules (juce_audio_processors, juce_audio_basics)
   - Plugin formats (VST3, AU)
4. Optionally: Check if plugin compiles with `--no-install` flag
5. Verify error reporting includes what's missing

**Expected Output**:
- `validate-foundation.py` verifies CMakeLists.txt exists
- Validates required JUCE configuration
- Optionally runs build verification
- Clear error messages for missing configuration

**Verification**:
- Automated: Create test CMakeLists.txt with proper JUCE 8 configuration
- Automated: Run validator, expect exit 0
- Automated: Remove PRODUCT_NAME, run validator, expect exit 1
- Manual: STOP AND ASK LEX: "Please verify that the validator catches missing juce_add_plugin() in CMakeLists.txt and produces a helpful error message."

---

### Phase 5b: Hook Scripts

#### Task 6: Verify Existing Hook Scripts

**Description**: Confirm all 4 existing hook scripts follow the graceful skip pattern and call validators correctly.

**Required Reading**:
- @architecture/13-error-handling-recovery.md (lines 182-461) - Complete hook patterns with examples
- @architecture/10-extension-architecture.md (lines 96-165) - Hook system architecture

**Dependencies**: Phase 5a complete (all validators verified)

**Implementation Steps**:
1. Verify `.claude/hooks/` directory exists
2. Check for 4 existing hook scripts:
   - `SubagentStop.sh` - Calls validators after each subagent
   - `Stop.sh` - Stage enforcement, verifies workflow integrity
   - `UserPromptSubmit.sh` - Auto-inject context (handoff files, PLUGINS.md)
   - `PreCompact.sh` - Preserves contracts before context compaction
3. Read each hook script and verify:
   - Shebang: `#!/bin/bash`
   - Executable permissions: `chmod +x`
   - **CRITICAL**: Graceful skip pattern at the start:
     ```bash
     # Check relevance FIRST
     if [[ ! condition_makes_hook_relevant ]]; then
       echo "Hook not relevant, skipping gracefully"
       exit 0  # Success, not failure
     fi
     ```
   - Proper exit codes: 0 (success/skip), 1+ (failure)
4. Test each hook with relevant and non-relevant scenarios

**Expected Output**:
- All 4 hook scripts exist in `.claude/hooks/`
- All scripts are executable (`chmod +x`)
- Each script checks relevance first and exits 0 when not applicable
- SubagentStop.sh correctly calls appropriate validators
- Error messages go to stderr

**Verification**:
- Automated: `ls -la .claude/hooks/*.sh` shows all 4 scripts with execute permissions
- Automated: `echo '{}' | .claude/hooks/SubagentStop.sh; echo "Exit code: $?"` (should exit 0 when no subagent_name)
- Automated: `echo '{"subagent_name":"unrelated-agent"}' | .claude/hooks/SubagentStop.sh; echo "Exit code: $?"` (should exit 0, graceful skip)
- Manual: STOP AND ASK LEX: "Please verify that SubagentStop.sh with a valid subagent_name (like 'shell-agent') calls the correct validator and blocks on failure."

---

#### Task 7: Verify SubagentStop.sh Validator Integration

**Description**: Ensure SubagentStop.sh correctly routes to the right validator for each subagent and handles failures.

**Required Reading**:
- @architecture/13-error-handling-recovery.md (lines 223-290) - SubagentStop hook specification
- @architecture/07-communication-architecture.md (lines 82-101) - Subagent report parsing

**Dependencies**: Task 6 complete, Phase 5a complete

**Implementation Steps**:
1. Read `SubagentStop.sh` current implementation
2. Verify case statement routes correctly:
   - `foundation-agent` → `validate-foundation.py`
   - `shell-agent` → `validate-parameters.py`
   - `dsp-agent` → `validate-dsp-components.py`
   - `gui-agent` → `validate-gui-bindings.py`
3. Verify it extracts subagent name from JSON input: `jq -r '.subagent_name'`
4. Verify it blocks workflow on validation failure (exit 2)
5. Verify it provides clear error messages on failure
6. Test with each subagent type

**Expected Output**:
- SubagentStop.sh routes each subagent to correct validator
- Validation failures block workflow with exit code 2
- Clear error messages indicate which validation failed
- Graceful skip for non-implementation subagents (troubleshooter, validator)

**Verification**:
- Automated: Create mock input JSON for each subagent: `echo '{"subagent_name":"shell-agent","plugin_name":"TestPlugin"}' | .claude/hooks/SubagentStop.sh`
- Automated: Verify correct validator is called (check output messages)
- Manual: STOP AND ASK LEX: "Please run a complete workflow with RadioMusic (or test plugin) through Stage 3 and verify that SubagentStop hook runs after shell-agent completes and calls validate-parameters.py."

---

#### Task 8: Implement PostToolUse Hook

**Description**: Create PostToolUse hook for real-time code quality validation (JUCE best practices, real-time safety).

**Required Reading**:
- @architecture/13-error-handling-recovery.md (lines 129-181) - PostToolUse hook specification
- @architecture/10-extension-architecture.md (lines 96-165) - Hook execution patterns
- @procedures/skills/juce-foundation.md - Real-time audio safety rules, parameter management

**Dependencies**: Task 6 complete

**Implementation Steps**:
1. Create `.claude/hooks/PostToolUse.sh`
2. Add shebang and executable permissions: `chmod +x`
3. Implement graceful skip pattern:
   ```bash
   #!/bin/bash

   INPUT=$(cat)
   TOOL_NAME=$(echo "$INPUT" | jq -r '.tool_name // empty' 2>/dev/null)

   # Check relevance FIRST - only validate on Write/Edit to plugin source files
   if [[ ! "$TOOL_NAME" =~ ^(Write|Edit)$ ]]; then
     echo "Hook not relevant for tool: $TOOL_NAME, skipping gracefully"
     exit 0
   fi

   FILE_PATH=$(echo "$INPUT" | jq -r '.tool_input.file_path // empty' 2>/dev/null)
   if [[ ! "$FILE_PATH" =~ plugins/.*/Source/.*\.(cpp|h)$ ]]; then
     echo "Hook not relevant for file: $FILE_PATH, skipping gracefully"
     exit 0
   fi
   ```
4. Extract file content from tool input
5. Run pattern matching checks:
   - **Real-time safety violations** in processBlock():
     - `new ` or `delete ` (heap allocation)
     - `std::mutex` or `.lock()` (blocking locks)
     - `File::` or `URL::` (I/O operations)
     - `std::cout` or `printf` (console output)
   - **JUCE best practices**:
     - APVTS parameter access (atomic, lock-free)
     - `ScopedNoDenormals` in processBlock
     - `prepareToPlay` buffer allocation
6. Report violations with severity:
   - ERROR: Real-time violations (block workflow)
   - WARNING: Best practice violations (advisory only)
7. Performance budget: Must complete in <2 seconds
8. Test with safe and unsafe code examples

**Expected Output**:
- `.claude/hooks/PostToolUse.sh` exists and is executable
- Checks relevance first (only plugin source files)
- Detects real-time safety violations (allocations, locks, I/O in processBlock)
- Detects missing best practices (no ScopedNoDenormals)
- Completes in <2 seconds
- Blocks workflow on ERROR, advisory on WARNING

**Verification**:
- Automated: Create test JSON input with Write tool editing PluginProcessor.cpp containing `new float[1024]` in processBlock
- Automated: Run hook, expect exit 1 with "ERROR: Heap allocation in processBlock"
- Automated: Create test with safe code, expect exit 0
- Manual: STOP AND ASK LEX: "Please write a test plugin processBlock with `std::mutex::lock()` and verify PostToolUse hook catches this real-time violation and blocks with clear error message."

---

#### Task 9: Implement SessionStart Hook

**Description**: Create SessionStart hook for environment validation (JUCE version, build tools, Python availability).

**Required Reading**:
- @architecture/13-error-handling-recovery.md (lines 377-425) - SessionStart hook specification
- @architecture/10-extension-architecture.md (lines 96-165) - Hook patterns

**Dependencies**: Task 6 complete

**Implementation Steps**:
1. Create `.claude/hooks/SessionStart.sh`
2. Add shebang and executable permissions: `chmod +x`
3. Implement environment checks:
   ```bash
   #!/bin/bash

   # SessionStart hook - Validate development environment
   # Runs once at session start, non-blocking warnings only

   echo "Validating development environment..."

   # Check Python 3 availability (needed for validators)
   if ! command -v python3 &> /dev/null; then
     echo "WARNING: python3 not found - validation scripts won't work" >&2
   fi

   # Check jq availability (needed for JSON parsing in hooks)
   if ! command -v jq &> /dev/null; then
     echo "WARNING: jq not found - hooks may fail" >&2
   fi

   # Check for JUCE (optional, informational only)
   if [ -d "/Applications/JUCE" ]; then
     echo "✓ JUCE found at /Applications/JUCE"
   else
     echo "INFO: JUCE not found at /Applications/JUCE (may be installed elsewhere)"
   fi

   # Check for CMake
   if command -v cmake &> /dev/null; then
     CMAKE_VERSION=$(cmake --version | head -1)
     echo "✓ $CMAKE_VERSION"
   else
     echo "WARNING: cmake not found - builds will fail" >&2
   fi

   # Check for Ninja (optional but recommended)
   if command -v ninja &> /dev/null; then
     echo "✓ ninja found (fast builds enabled)"
   else
     echo "INFO: ninja not found (builds will use default generator)"
   fi

   echo "Environment validation complete"
   exit 0  # Never block session start
   ```
4. Test on clean system and system with all tools

**Expected Output**:
- `.claude/hooks/SessionStart.sh` exists and is executable
- Checks for python3, jq, CMake, JUCE, Ninja
- Provides clear informational messages
- Never blocks (always exit 0)
- Runs in <5 seconds

**Verification**:
- Automated: Run SessionStart hook directly: `.claude/hooks/SessionStart.sh`
- Automated: Verify output shows tool availability
- Automated: Verify exit code is always 0
- Manual: STOP AND ASK LEX: "Please start a new Claude Code session and verify SessionStart hook runs automatically and shows environment status without blocking."

---

#### Task 10: Update hooks.json Configuration

**Description**: Ensure hooks.json registers all 6 hooks with correct lifecycle events.

**Required Reading**:
- @architecture/10-extension-architecture.md (lines 96-165) - Hook registration and configuration
- @architecture/13-error-handling-recovery.md (lines 48-127) - Complete hook inventory

**Dependencies**: Tasks 6, 8, 9 complete (all hooks implemented)

**Implementation Steps**:
1. Read current `.claude/hooks/hooks.json`
2. Verify registration for all 6 hooks:
   ```json
   {
     "hooks": {
       "PostToolUse": {
         "script": "PostToolUse.sh",
         "description": "Real-time code quality validation for JUCE best practices"
       },
       "SubagentStop": {
         "script": "SubagentStop.sh",
         "description": "Deterministic contract validation after each subagent"
       },
       "UserPromptSubmit": {
         "script": "UserPromptSubmit.sh",
         "description": "Auto-inject context (handoff files, PLUGINS.md)"
       },
       "Stop": {
         "script": "Stop.sh",
         "description": "Stage enforcement and workflow integrity verification"
       },
       "PreCompact": {
         "script": "PreCompact.sh",
         "description": "Preserve contracts before context compaction"
       },
       "SessionStart": {
         "script": "SessionStart.sh",
         "description": "Environment validation at session start"
       }
     }
   }
   ```
3. Verify all scripts exist and are executable
4. Test hook registration by starting new session

**Expected Output**:
- `hooks.json` registers all 6 hooks
- Each hook has clear description
- All hook scripts exist and are executable
- Claude Code recognizes all hooks

**Verification**:
- Automated: `cat .claude/hooks/hooks.json | jq '.hooks | keys'` shows all 6 hook names
- Automated: Verify all scripts exist: `for hook in $(jq -r '.hooks[].script' .claude/hooks/hooks.json); do test -x .claude/hooks/$hook && echo "✓ $hook" || echo "✗ $hook"; done`
- Manual: STOP AND ASK LEX: "Please start a new Claude Code session and verify SessionStart hook runs. Then trigger a Write operation to plugin source and verify PostToolUse hook runs."

---

### Phase 5c: Validator Subagent

#### Task 11: Verify Validator Subagent Implementation

**Description**: Confirm validator subagent exists and follows the specification with correct semantic checks for each stage.

**Required Reading**:
- @architecture/02-core-abstractions.md (lines 185-252) - Hybrid validation layer 2 specification
- @architecture/03-model-selection-extended-thinking-strategy.md - Model selection (Sonnet for quality)
- @architecture/07-communication-architecture.md (lines 82-101) - JSON report format

**Dependencies**: Phase 5a, 5b complete

**Implementation Steps**:
1. Verify `.claude/agents/validator.md` exists
2. Read validator.md and check frontmatter:
   ```yaml
   ---
   name: validator
   description: Independent semantic validation for quality and design decisions
   model: sonnet  # Quality over speed
   extended_thinking: false  # Not needed for validation
   tools: [Read, Grep, Bash]
   preconditions:
     - stage_complete: true
     - contracts_exist: true
   ---
   ```
3. Verify it has stage-specific validation for all 7 stages:
   - Stage 0: Architecture specification validation (research quality)
   - Stage 1: Planning validation (complexity scoring, phasing)
   - Stage 2: Foundation validation (CMakeLists.txt best practices)
   - Stage 3: Shell validation (parameter ranges, defaults, naming)
   - Stage 4: DSP validation (algorithm correctness, real-time safety, creative intent)
   - Stage 5: GUI validation (member order, aesthetic, bindings, accessibility)
   - Stage 6: Validation & Presets (CHANGELOG format, preset count, status update)
4. Verify JSON report format matches specification:
   ```json
   {
     "agent": "validator",
     "stage": <number>,
     "status": "PASS" | "FAIL",
     "checks": [
       {
         "name": "<check_identifier>",
         "passed": <boolean>,
         "message": "<descriptive message>",
         "severity": "error" | "warning" | "info"
       }
     ],
     "recommendation": "<what to do next>",
     "continue_to_next_stage": <boolean>
   }
   ```
5. Verify it supports `.validator-overrides.yaml` for false positives

**Expected Output**:
- `.claude/agents/validator.md` exists with complete specification
- Uses Sonnet model (quality over speed)
- Has semantic checks for all 7 stages
- Returns valid JSON reports
- Supports override mechanism for false positives

**Verification**:
- Automated: `test -f .claude/agents/validator.md && echo "EXISTS" || echo "MISSING"`
- Automated: `grep "model: sonnet" .claude/agents/validator.md && echo "✓ Uses Sonnet" || echo "✗ Wrong model"`
- Manual: STOP AND ASK LEX: "Please read .claude/agents/validator.md and verify it has detailed semantic checks for Stage 4 (DSP validation) including creative intent alignment, real-time safety, and parameter modulation."

---

#### Task 12: Test Validator Invocation from plugin-workflow

**Description**: Verify plugin-workflow correctly invokes validator subagent for complexity ≥4 plugins and parses JSON reports.

**Required Reading**:
- @procedures/skills/plugin-workflow.md - Validator invocation pattern
- @architecture/07-communication-architecture.md (lines 82-101) - Report parsing with error handling

**Dependencies**: Task 11 complete, Phase 2 complete (plugin-workflow exists)

**Implementation Steps**:
1. Read `.claude/skills/plugin-workflow/SKILL.md`
2. Find validator invocation logic (should be after Stage 4 or Stage 5 for complexity ≥4)
3. Verify it uses Task tool to dispatch validator:
   ```
   Task(
     subagent_type="validator",
     description="Validate Stage N completion",
     prompt=f"""
     Validate Stage {N} completion for {PluginName}.

     **Stage:** {N}
     **Plugin:** {PluginName}
     **Contracts:**
     - parameter-spec.md: {content}
     - architecture.md: {content}
     - plan.md: {content}

     **Expected outputs for Stage {N}:**
     {stage_specific_list}

     Return JSON validation report.
     """
   )
   ```
4. Verify it parses JSON report with robust error handling:
   - Extract JSON from markdown code blocks
   - Handle malformed JSON
   - Validate required fields exist
   - Check status field for PASS/FAIL
5. Verify it presents decision menu with findings:
   ```
   ✓ Validator reviewed Stage N implementation

   Status: PASS (2 warnings)
   - WARNING: No zero-length buffer check in processBlock
   - INFO: DSP algorithm matches creative intent

   Recommendation: DSP implementation solid, consider adding buffer check

   What's next?
   1. Continue to Stage N+1 (recommended)
   2. Address warnings first
   3. Review validator report details
   4. Other
   ```
6. Test with a complexity ≥4 plugin (or mock complexity in plan.md)

**Expected Output**:
- plugin-workflow invokes validator for complexity ≥4 plugins
- Passes complete contracts and expected outputs
- Parses JSON report robustly
- Presents findings in decision menu
- User decides whether to continue or address warnings

**Verification**:
- Automated: Read plugin-workflow SKILL.md and verify validator invocation exists
- Automated: Grep for `Task.*validator` or similar dispatch pattern
- Manual: STOP AND ASK LEX: "Please create a test plugin with complexity 4 (or temporarily modify RadioMusic plan.md to show complexity 4) and run through Stage 4. Verify that validator subagent is invoked automatically and presents its findings in a decision menu."

---

#### Task 13: Test validator Override Mechanism

**Description**: Verify validator respects `.validator-overrides.yaml` for false positive suppression.

**Required Reading**:
- @architecture/02-core-abstractions.md (lines 230-252) - Override mechanism specification

**Dependencies**: Task 11, 12 complete

**Implementation Steps**:
1. Read validator.md override mechanism section
2. Create test `.validator-overrides.yaml` in a test plugin:
   ```yaml
   overrides:
     - stage: 4
       check_type: "missing_dsp_component"
       pattern: "CustomReverbImpl"
       reason: "Using custom reverb implementation, not juce::dsp::Reverb"
       date: "2025-11-10"
   ```
3. Verify validator loads and parses override file
4. Verify matching checks are suppressed with logged reason
5. Test validator produces different reports with and without overrides

**Expected Output**:
- Validator loads `.validator-overrides.yaml` if it exists
- Matching checks show as suppressed in report:
  ```json
  {
    "name": "missing_dsp_component",
    "passed": true,
    "message": "Check suppressed: Using custom reverb implementation, not juce::dsp::Reverb",
    "severity": "info"
  }
  ```
- Non-matching checks still run normally
- Override file is optional (graceful skip if missing)

**Verification**:
- Automated: Create test plugin with override file
- Automated: Invoke validator on Stage 4 with missing component that's in override
- Automated: Verify check shows as suppressed
- Manual: STOP AND ASK LEX: "Please create a .validator-overrides.yaml in RadioMusic plugin with a Stage 4 override, then run validator on Stage 4. Verify the overridden check appears as suppressed in the report with the reason from the override file."

---

## Comprehensive Verification Criteria

### Automated Tests

```bash
# Test 1: Verify all validator scripts exist and are executable
ls -la .claude/hooks/validators/*.py
test -x .claude/hooks/validators/validate-parameters.py && echo "✓" || echo "✗"
test -x .claude/hooks/validators/validate-dsp-components.py && echo "✓" || echo "✗"
test -x .claude/hooks/validators/validate-gui-bindings.py && echo "✓" || echo "✗"
test -x .claude/hooks/validators/validate-foundation.py && echo "✓" || echo "✗"

# Test 2: Verify all hook scripts exist and are executable
ls -la .claude/hooks/*.sh
test -x .claude/hooks/PostToolUse.sh && echo "✓" || echo "✗"
test -x .claude/hooks/SubagentStop.sh && echo "✓" || echo "✗"
test -x .claude/hooks/UserPromptSubmit.sh && echo "✓" || echo "✗"
test -x .claude/hooks/Stop.sh && echo "✓" || echo "✗"
test -x .claude/hooks/PreCompact.sh && echo "✓" || echo "✗"
test -x .claude/hooks/SessionStart.sh && echo "✓" || echo "✗"

# Test 3: Verify hooks.json registers all 6 hooks
cat .claude/hooks/hooks.json | jq '.hooks | keys' | grep -q "PostToolUse" && echo "✓" || echo "✗"
cat .claude/hooks/hooks.json | jq '.hooks | keys' | grep -q "SubagentStop" && echo "✓" || echo "✗"
cat .claude/hooks/hooks.json | jq '.hooks | keys' | grep -q "SessionStart" && echo "✓" || echo "✗"

# Test 4: Verify validator subagent exists
test -f .claude/agents/validator.md && echo "✓" || echo "✗"
grep -q "model: sonnet" .claude/agents/validator.md && echo "✓" || echo "✗"

# Test 5: Test graceful skip pattern in hooks
echo '{}' | .claude/hooks/SubagentStop.sh
EXIT_CODE=$?
test $EXIT_CODE -eq 0 && echo "✓ SubagentStop graceful skip works" || echo "✗ Failed"

echo '{"tool_name":"Read"}' | .claude/hooks/PostToolUse.sh
EXIT_CODE=$?
test $EXIT_CODE -eq 0 && echo "✓ PostToolUse graceful skip works" || echo "✗ Failed"

# Test 6: Test validator script graceful skip (no active plugin)
cd /tmp && python3 /path/to/.claude/hooks/validators/validate-parameters.py
EXIT_CODE=$?
test $EXIT_CODE -eq 0 && echo "✓ validate-parameters graceful skip works" || echo "✗ Failed"

# Test 7: Verify CLAUDE.md updated with validation status
grep -q "Phase 5: Validation System (complete)" .claude/CLAUDE.md && echo "✓" || echo "✗"
```

### Manual Verification Checklist

- [ ] **Task 1 Verification:** Run each validator script manually with RadioMusic plugin and verify expected output
- [ ] **Task 2 Verification:** Create test plugin with mismatched parameter types and verify validator catches it
- [ ] **Task 3 Verification:** Remove DSP component from processBlock and verify validator detects missing component
- [ ] **Task 4 Verification:** Test incorrect member declaration order and verify validator catches crash risk
- [ ] **Task 5 Verification:** Test missing PRODUCT_NAME in CMakeLists.txt and verify validator error
- [ ] **Task 6 Verification:** Verify SubagentStop hook runs after shell-agent and calls correct validator
- [ ] **Task 8 Verification:** Write processBlock with `std::mutex::lock()` and verify PostToolUse blocks
- [ ] **Task 9 Verification:** Start new session and verify SessionStart shows environment status
- [ ] **Task 10 Verification:** Start new session and trigger Write to verify hooks run
- [ ] **Task 12 Verification:** Run complexity 4 plugin through Stage 4 and verify validator invoked
- [ ] **Task 13 Verification:** Test override mechanism suppresses false positive checks

### File Existence Verification

```bash
# Verify all expected files exist
ls -la .claude/hooks/validators/validate-parameters.py
ls -la .claude/hooks/validators/validate-dsp-components.py
ls -la .claude/hooks/validators/validate-gui-bindings.py
ls -la .claude/hooks/validators/validate-foundation.py
ls -la .claude/hooks/PostToolUse.sh
ls -la .claude/hooks/SubagentStop.sh
ls -la .claude/hooks/UserPromptSubmit.sh
ls -la .claude/hooks/Stop.sh
ls -la .claude/hooks/PreCompact.sh
ls -la .claude/hooks/SessionStart.sh
ls -la .claude/hooks/hooks.json
ls -la .claude/agents/validator.md
```

### Integration Tests

**Test 1: End-to-End Validation Flow**
1. Create test plugin with contracts (parameter-spec.md, architecture.md)
2. Run through Stage 3 (Shell)
3. Verify SubagentStop hook runs after shell-agent completes
4. Verify validate-parameters.py executes
5. Verify workflow blocks if validation fails
6. Fix validation error and verify workflow continues

**Test 2: Member Order Validation (Critical WebView Crash Prevention)**
1. Create test plugin with WebView UI
2. Intentionally declare members in wrong order (webView before relays)
3. Run through Stage 5 (GUI)
4. Verify validate-gui-bindings.py catches wrong order
5. Verify clear error message explains crash risk
6. Fix order and verify validation passes

**Test 3: Real-Time Safety Validation**
1. Create test plugin processBlock with heap allocation (`new float[1024]`)
2. Edit file (trigger PostToolUse hook)
3. Verify hook detects real-time violation
4. Verify workflow blocks with clear error message
5. Remove allocation and verify hook passes

**Test 4: Validator Invocation for Complex Plugins**
1. Create test plugin with complexity score 4
2. Run through Stage 4 (DSP)
3. Verify validator subagent invoked automatically
4. Verify JSON report parsed correctly
5. Verify decision menu presents findings
6. Choose to continue despite warnings

**Test 5: Override Mechanism**
1. Create .validator-overrides.yaml with Stage 4 override
2. Run validator on Stage 4 with matching false positive
3. Verify check suppressed with reason logged
4. Verify other checks still run normally

### Success Criteria

Phase 5 is COMPLETE when:

1. ✅ All 4 Python validation scripts exist, are executable, and handle edge cases gracefully
2. ✅ All 6 hook scripts exist, are executable, and follow graceful skip pattern
3. ✅ hooks.json registers all 6 hooks correctly
4. ✅ SubagentStop hook calls correct validators for each subagent
5. ✅ PostToolUse hook detects real-time safety violations in processBlock
6. ✅ SessionStart hook validates environment without blocking
7. ✅ validator subagent exists with semantic checks for all 7 stages
8. ✅ plugin-workflow invokes validator for complexity ≥4 plugins
9. ✅ Validator JSON reports parsed correctly with robust error handling
10. ✅ Override mechanism suppresses false positives with logged reasons
11. ✅ **Critical**: validate-gui-bindings.py catches incorrect member declaration order
12. ✅ All automated tests pass
13. ✅ All manual verification items checked
14. ✅ Integration tests demonstrate validation catches contract drift
15. ✅ CLAUDE.md updated with Phase 5 completion status

---

## Potential Issues & Mitigations

### Issue 1: Python Script Dependencies
**Problem:** Validator scripts depend on Python 3 and certain parsing libraries
**Mitigation:** SessionStart hook checks for Python 3 availability and warns if missing
**Fallback:** Scripts use only standard library (no external dependencies)

### Issue 2: Hook Performance
**Problem:** PostToolUse must complete in <2s to not disrupt workflow
**Mitigation:** Use grep/awk for pattern matching (faster than Python), limit scope to processBlock
**Fallback:** If performance issues, make PostToolUse advisory (warning) instead of blocking

### Issue 3: False Positives in Validation
**Problem:** Deterministic pattern matching may flag valid custom implementations
**Mitigation:** Implement `.validator-overrides.yaml` mechanism to suppress known false positives
**User Control:** User always has final decision via decision menus

### Issue 4: Validator Subagent Timeouts
**Problem:** Semantic validation may take 30-60s, risks timeout
**Mitigation:** Use Sonnet (faster than Opus), keep analysis focused on current stage only
**Fallback:** If timeout, log warning and continue (validation is advisory, not blocking)

### Issue 5: Hook Execution Order
**Problem:** Multiple hooks may fire in sequence, unclear dependencies
**Mitigation:** Each hook checks relevance first and skips gracefully if not applicable
**Documentation:** architecture/13-error-handling-recovery.md documents execution order clearly

### Issue 6: JSON Report Malformation
**Problem:** Validator may return invalid JSON, breaking parser
**Mitigation:** Implement robust parsing with try-catch, extract JSON from markdown blocks
**Fallback:** If parsing fails, log raw report and continue (advisory layer, not critical)

### Issue 7: Contract Files Missing During Validation
**Problem:** Validators expect contracts to exist, but they may be missing in early stages
**Mitigation:** All validators implement graceful skip when contracts don't exist yet
**Pattern:** Check file existence first, exit 0 if not applicable

---

## Notes for Next Phase

**Phase 6 (WebView UI System) Preparation:**
- Validation system is critical for Phase 6 because WebView member order validation prevents 90% of crashes
- validate-gui-bindings.py must be thoroughly tested before Phase 6 implementation
- ui-mockup skill should leverage validator for semantic UI checks (aesthetic alignment)
- Consider adding WebView-specific hook for resource provider validation

**Performance Monitoring:**
- Track hook execution times to ensure <2s budget
- Monitor validator subagent completion times (target 30-60s)
- Adjust invocation strategy if performance degrades

**Knowledge Base:**
- Document common false positives in troubleshooting/validation-false-positives.md
- Track override patterns to improve validator intelligence
- Collect examples of validation catching real bugs for documentation

**Future Enhancements (Post-Phase 7):**
- Machine learning model for semantic validation (replace pattern matching)
- Incremental validation (only changed files, not full plugin)
- Validation caching (skip if files unchanged)
- Visual validation report dashboard (HTML report generation)
