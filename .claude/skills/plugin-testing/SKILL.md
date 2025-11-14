---
name: plugin-testing
description: Validates audio plugins through automated tests, pluginval, or manual DAW testing. Use when testing completed plugins (Stage 5+), after bug fixes, or when user mentions test, validate, validation, pluginval, stability, automated tests, run tests, check plugin, or quality assurance
allowed-tools:
  - Read
  - Bash
  - Task # For deep-research on failures
---

# plugin-testing Skill

**Purpose:** Catch crashes, parameter issues, and state bugs in 2 minutes with automated tests.

## Workflow Overview

Three test modes: (1) Automated (~2 min), (2) Build + Pluginval (~5-10 min) ⭐ RECOMMENDED, (3) Manual DAW (~30-60 min). See decision menu for details.

## Phase 1: Detect Plugin and Mode Selection

<decision_gate>
**Complete all prerequisite checks before presenting mode selection:**

1. Parse plugin name from user input (from `/test [Name]` or natural language)
2. Read `PLUGINS.md` and verify plugin exists
3. Verify plugin status is NOT 💡 (implementation must exist)
4. Check for Tests/ directory: `test -d "plugins/{PLUGIN_NAME}/Tests/"`

Present mode selection menu (see `assets/decision-menu-templates.md#mode-selection`) and WAIT for user selection (1-4). Do not proceed to Phase 2 until mode selected.
</decision_gate>

**Parse shorthand commands:**
- `/test [PluginName] build` → Jump to Mode 2
- `/test [PluginName] quick` → Jump to Mode 1
- `/test [PluginName] manual` → Jump to Mode 3

## Phase 2: Execute Test Mode

### Mode 1: Automated Testing

**Track your progress:**
```
Mode 1 Progress:
- [ ] Step 1: Check for Tests/ directory
- [ ] Step 2: Read test specifications
- [ ] Step 3: Build and execute tests
- [ ] Step 4: Parse test results
- [ ] Step 5: Present results and next steps
```

<critical_sequence>
**Execute these steps in exact order:**

<step id="check_tests">
**Step 1: Check for Tests/ Directory**

Verify Tests/ directory exists:
```bash
test -d "plugins/{PLUGIN_NAME}/Tests/"
```

**VALIDATION GATE**: If Tests/ directory does not exist:
1. Inform user that automated tests require test infrastructure
2. Present decision menu from `assets/decision-menu-templates.md#missing-tests`
3. WAIT for user selection
4. **STOP - Do not proceed to Step 2** (user must select Mode 2 or cancel)

If Tests/ directory exists, proceed to Step 2.
</step>

<step id="read_spec">
**Step 2: Read Test Specifications**

Read `references/test-specifications.md` for detailed test implementations before proceeding.
</step>

<step id="build_tests">
**Step 3: Build and Execute Tests**

Build and run tests (see `references/test-specifications.md#execution`).
</step>

<step id="parse_results">
**Step 4: Parse Test Results**

Parse test output and generate report using template from `assets/report-templates.md#mode1-results`.
</step>

<step id="present_results">
**Step 5: Present Results**

Present test results and post-test decision menu (see `assets/decision-menu-templates.md#post-test-mode1`). WAIT for user selection.
</step>
</critical_sequence>

### Mode 2: Build + Pluginval

**Track your progress:**
```
Mode 2 Progress:
- [ ] Step 1: Prerequisites check (pluginval installation)
- [ ] Step 2: Build Release binaries (VST3 + AU)
- [ ] Step 3: Run pluginval at strictness level 10
- [ ] Step 4: Parse validation results
- [ ] Step 5: Present results and next steps
```

<critical_sequence>
**Execute these steps in exact order:**

<step id="prereq_check">
**Step 1: Prerequisites Check**

Read `references/pluginval-guide.md#installation-check` for implementation details.

Check for pluginval installation using script from reference file.

**VALIDATION GATE**: If pluginval not found:
1. Present installation decision menu (see `assets/decision-menu-templates.md#pluginval-install`)
2. WAIT for user selection
3. If user chooses to install: Provide installation instructions and wait for confirmation
4. If user cancels: STOP workflow and return to main menu
5. **STOP - Do not proceed to Step 2 until pluginval is confirmed installed**

Verify pluginval is accessible before proceeding to Step 2.
</step>

<step id="build_release">
**Step 2: Build Release Binaries**

Build in Release mode (see `references/pluginval-guide.md#build-process`):

Locate binaries:
```bash
VST3_PATH="build/plugins/{PLUGIN_NAME}/{PLUGIN_NAME}_artefacts/Release/VST3/{PRODUCT_NAME}.vst3"
AU_PATH="build/plugins/{PLUGIN_NAME}/{PLUGIN_NAME}_artefacts/Release/AU/{PRODUCT_NAME}.component"
```
</step>

<step id="validate_plugins">
**Step 3: Run Pluginval**

Validate both formats at strictness level 10 (see `references/pluginval-guide.md#execution`).
</step>

<step id="parse_results">
**Step 4: Parse Results**

Parse pluginval output (see `references/pluginval-guide.md#parsing-output`) and generate report using template from `assets/report-templates.md#mode2-results`.
</step>

<step id="present_results">
**Step 5: Present Results and Next Steps**

Present post-test decision menu (see `assets/decision-menu-templates.md#post-test-mode2`) and WAIT for user selection.
</step>
</critical_sequence>

### Mode 3: Manual DAW Testing

**Track your progress:**
```
Mode 3 Progress:
- [ ] Step 1: Read DAW testing guide
- [ ] Step 2: Generate customized checklist (parameter-specific)
- [ ] Step 3: Present checklist with testing instructions
- [ ] Step 4: Collect user test results (pass/fail per item)
- [ ] Step 5: Present next steps
```

<critical_sequence>
**Execute these steps in exact order:**

<step id="read_guide">
**Step 1: Read DAW Testing Guide**

Read `references/manual-testing-guide.md` for complete manual testing procedures.
</step>

<step id="generate_checklist">
**Step 2: Generate Customized Checklist**

Generate customized checklist from `parameter-spec.md` tailored to plugin's specific parameters and features.
</step>

<step id="present_checklist">
**Step 3: Present Checklist**

Present checklist to user with instructions for manual testing in their DAW.

Inform user to report back with results (pass/fail per item).
</step>

<step id="collect_results">
**Step 4: Collect Results**

WAIT for user to complete manual testing and provide results.

Parse user's feedback and generate completion report.
</step>

<step id="present_menu">
**Step 5: Present Next Steps**

Present post-test decision menu (see `assets/decision-menu-templates.md#post-test-mode3`) and WAIT for user selection.
</step>
</critical_sequence>

## Phase 3: Failure Investigation (Option 1)

<delegation_rule>
**When user chooses "Investigate failures":**

<sequence>
1. Read `references/troubleshooting.md` for each failed test
2. Provide initial fix recommendations from troubleshooting docs
3. Offer code inspection option

4. **For non-trivial issues:**

   **Non-trivial issues include:**
   - Errors not documented in troubleshooting.md
   - Multiple interconnected test failures (3+ tests failing together)
   - JUCE API-specific problems (APVTS, processBlock, threading)
   - Issues requiring cross-file analysis (DSP + parameters + state management)
   - Platform-specific crashes or validation failures

   <handoff_protocol>
   Delegate to `deep-research` skill via Task tool:

   ```
   Task tool parameters:
   - task: "Investigate [test_name] failure in [PluginName]

     Context:
     - Test failed: [specific failure message]
     - Plugin type: [from creative-brief.md]
     - Relevant code: [file paths from investigation]

     Goal: Find root cause and provide specific fix"
   - subagent_type: "research-planning-agent"
   ```

   After deep-research completes:
   1. Read deep-research return message completely
   2. Verify root cause identified (not just symptoms)
   3. Extract specific fix recommendations
   4. THEN present findings to user with decision menu

   **Never attempt to fix complex issues without delegation to deep-research.**
   </handoff_protocol>
</sequence>
</delegation_rule>

## Phase 4: Log Test Results and Checkpoint

<state_requirement>
**After completing any test mode (1, 2, or 3):**

**Requirement 1: Save Test Log**

Save detailed test log to: `logs/{PLUGIN_NAME}/test_[timestamp].log`

Use format from `assets/report-templates.md#test-log-format`

**Requirement 2: Update .continue-here.md**

Update `.continue-here.md`:
- Set stage: "testing_complete"
- Set next_step based on test results (Stage 4 if passed, investigation if failed)
- Record last_tested: [timestamp]
- Record test_mode: [1/2/3]

**Requirement 3: Update PLUGINS.md**

Update `PLUGINS.md` for {PLUGIN_NAME}:
- Set test_status: "✅ passed" or "❌ failed"
- Record last_tested: [date]
- Record test_mode_used: [Mode 1/2/3]

**Requirement 4: Commit Changes**

Commit test results and state updates:
```bash
git add logs/{PLUGIN_NAME}/test_[timestamp].log .continue-here.md PLUGINS.md
git commit -m "test({PLUGIN_NAME}): {test_mode} validation {RESULT}"
```

**Requirement 5: Check Workflow Mode**

Read `.claude/preferences.json` to determine workflow mode:

- If express mode with auto_test=true: Auto-progress to installation (skip decision menu)
- If manual mode OR auto_test=false: Present post-test decision menu (see templates)

**Complete all five requirements before proceeding.**

**Error Handling**: If any requirement fails (file write error, missing plugin entry), report the specific error to the user and abort state update. Do not present the decision menu with incomplete state.

VERIFY all state files updated and committed successfully before presenting next steps.
</state_requirement>

## Success Criteria

Testing is successful when:

- ✅ Tests run without crashes (even if some fail, process completes)
- ✅ All tests pass OR failures are documented with clear explanations
- ✅ User understands what failed and why (no mystery errors)
- ✅ Logs saved for future reference (`logs/[PluginName]/`)
- ✅ User knows next step (install, fix issues, continue workflow)
- ✅ Test results stored in PLUGINS.md (test date, pass/fail, mode used)

**NOT required for success:**

- 100% pass rate (failures are learning opportunities)
- Fixing all issues immediately (user can defer fixes)
- Running all 3 test modes (one mode is sufficient for validation)

## Integration Points

**Invoked by:**

**Commands:**
- `/test [PluginName]` → Mode selection menu
- `/test [PluginName] build` → Direct to Mode 2 (pluginval)
- `/test [PluginName] quick` → Direct to Mode 1 (automated)
- `/test [PluginName] manual` → Direct to Mode 3 (DAW testing)

**Skills:**
- `plugin-workflow` → After Stages 4, 5, 6 (validation checkpoints)
- `plugin-improve` → After implementing bug fixes or features

**Natural Language:**
- "Test [PluginName]"
- "Run validation on [PluginName]"
- "Check [PluginName] for crashes"

**Invokes:**

- `deep-research` skill → When user chooses "Investigate failures"

**Creates:**

- Test logs in `logs/{PLUGIN_NAME}/test_[timestamp].log`
- Build artifacts in `build/plugins/{PLUGIN_NAME}/`

**Updates:**

- `.continue-here.md` → Testing checkpoint
- `PLUGINS.md` → Test status


## Reference Documentation

- **Test specifications:** `references/test-specifications.md` - Detailed implementation of 5 automated tests
- **Pluginval guide:** `references/pluginval-guide.md` - Industry-standard validation details
- **Manual testing guide:** `references/manual-testing-guide.md` - DAW testing methodology
- **Troubleshooting:** `references/troubleshooting.md` - Common issues and fixes

## Template Assets

- **Manual testing checklist:** `assets/manual-testing-checklist.md` - DAW testing template
