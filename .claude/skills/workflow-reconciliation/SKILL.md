---
name: workflow-reconciliation
description: Reconcile workflow state files to ensure checkpoints are properly updated. Use when state files are out of sync, after subagent completion without handoff, when switching workflows, or when user runs /reconcile command.
---

# workflow-reconciliation

Detect current workflow context, validate state file currency, and remediate gaps to prevent checkpoint amnesia.

## When to Invoke This Skill

- User explicitly runs `/reconcile [PluginName?]`
- Other skills detect state drift (plugin-workflow, ui-mockup)
- After subagent completion if handoff file missing
- When switching between workflows

**NOT invoked** for normal checkpoint completion (plugin-workflow handles that internally). ONLY for detected drift or explicit /reconcile command.

## Core Responsibilities

<orchestration_pattern>
  This skill follows a 5-phase pattern:

  1. Context Detection → Identify workflow, stage, plugin
  2. Rule Loading → Load expected state for workflow+stage
  3. Gap Analysis → Compare filesystem vs expected state
  4. Report Generation → Show gaps with proposed fixes
  5. Remediation → Execute user's chosen fix strategy

  <progress_tracking>
    Copy this checklist to track reconciliation progress:

    ```
    Reconciliation Progress:
    - [ ] Phase 1: Context Detection
    - [ ] Phase 2: Rule Loading
    - [ ] Phase 3: Gap Analysis
    - [ ] Phase 4: Report Generation
    - [ ] Phase 5: Remediation Execution
    ```
  </progress_tracking>
</orchestration_pattern>

## Phase 1: Context Detection

<context_detection enforcement="blocking">
  <required_analysis>
    Detect current workflow context by analyzing:

    <detection_method priority="1" source=".continue-here.md">
      IF .continue-here.md exists in current plugin directory:
        - Extract YAML frontmatter (workflow, stage, status, phase)
        - This is authoritative source of workflow state
    </detection_method>

    <detection_method priority="2" source="PLUGINS.md">
      IF plugin name provided as argument:
        - Read PLUGINS.md entry for plugin
        - Extract status emoji (💡 Ideated, 🚧 Stage N, ✅ Working, 📦 Installed)
    </detection_method>

    <detection_method priority="3" source="filesystem_analysis">
      IF no handoff file found:
        - Analyze files created/modified in session
        - Infer workflow from file patterns:
          - .ideas/creative-brief.md → plugin-ideation
          - .ideas/mockups/v*-ui.html → ui-mockup
          - Source/*.cpp changes + CHANGELOG.md → plugin-improve
          - plugins/*/CMakeLists.txt → plugin-workflow (Stage 1+)
    </detection_method>
  </required_analysis>

  <validation>
    BEFORE proceeding to gap analysis:
    - MUST identify workflow name
    - MUST identify current stage/phase
    - MUST identify plugin name (if workflow is plugin-specific)

    IF unable to detect context: BLOCK with error showing detected values and suggest providing plugin name.
  </validation>
</context_detection>

## Phase 2: Rule Loading

<reconciliation_rules>
  <rule_loading>
    1. Use jq to extract ONLY needed section from assets/reconciliation-rules.json
       - Example: `jq '.["plugin-workflow"].stages["2"]' reconciliation-rules.json`
       - Do NOT load entire 90-line file into context
    2. Lookup workflow name from context detection
    3. Lookup stage (for plugin-workflow) or phase (for other workflows) from context detection
    4. Extract reconciliation rule for current workflow + stage/phase
    5. If workflow not found in reconciliation-rules.json: BLOCK with error "Unknown workflow: {name}"
  </rule_loading>

  <rule_application>
    For current workflow and stage, validate:
    - All state_files exist and are current
    - All required_files exist
    - PLUGINS.md status matches expected
    - Git commit exists for this stage completion
  </rule_application>
</reconciliation_rules>

## Phase 3: Gap Analysis

<gap_analysis enforcement="blocking">
  <validation_sequence enforce_order="true">
    <check order="1" category="contract_immutability" required="true">
      For contract files (.ideas/*.md):
        - Check if workflow is plugin-workflow AND stage is 1, 2, or 3
        - If YES: WARN user that contracts are immutable during implementation
        - Suggest completing current stage or rolling back to Stage 0
        - BLOCK remediation of contract files during Stages 1-3
    </check>

    <check order="2" category="file_existence" required="true">
      Use parallel Read tool calls to check all required_files simultaneously.
      For each required_file in reconciliation rule:
        - Check file exists at expected path (relative to plugins/{PluginName}/)
        - Example: CMakeLists.txt → plugins/{PluginName}/CMakeLists.txt
        - Record as GAP if missing
    </check>

    <check order="3" category="state_file_currency" required="true">
      Read .continue-here.md YAML frontmatter:
        - Extract: stage, phase, status, workflow, last_updated
        - Compare to detected context
        - Record as GAP if mismatch

      Read PLUGINS.md entry for plugin:
        - Extract status emoji
        - Compare to expected status from reconciliation rule
        - Record as GAP if mismatch
    </check>

    <check order="4" category="git_status" required="true">
      Run git status:
        - Identify unstaged changes (modified, deleted)
        - Identify staged but uncommitted changes
        - Identify untracked files matching required_files pattern
        - Record as GAP if uncommitted changes exist
    </check>
  </validation_sequence>

  <gap_aggregation>
    Aggregate all gaps into structured report:
    {
      "file_existence_gaps": [
        {"file": "CMakeLists.txt", "status": "missing", "expected_path": "plugins/{PluginName}/CMakeLists.txt"}
      ],
      "state_currency_gaps": [
        {"file": ".continue-here.md", "field": "stage", "current": "2", "expected": "3"}
      ],
      "git_status_gaps": [
        {"file": "Source/PluginProcessor.cpp", "status": "modified", "staged": false}
      ]
    }
  </gap_aggregation>

  <validation>
    MUST complete all 4 check categories before generating report.
    IF any check fails to execute: BLOCK with error.
  </validation>
</gap_analysis>

## Phase 4: Report Generation

<reconciliation_report>
  <report_generation>
    1. Aggregate gap analysis results
    2. Generate proposed actions based on gaps found
    3. Format report with visual dividers
    4. Display report to user
  </report_generation>

  <checkpoint_protocol>
    After displaying report, MUST present decision menu and WAIT.

    <decision_menu format="inline_numbered_list" forbidden_tool="AskUserQuestion">
      Present options based on gap severity:

      <menu_options category="no_gaps_found">
        1. All good - return to workflow (recommended)
        2. Show me the state files anyway
        3. Force reconciliation (update timestamps)
        4. Other
      </menu_options>

      <menu_options category="minor_gaps" condition="only_timestamp_drift">
        1. Fix automatically - Update timestamps and commit
        2. Show me the diffs first
        3. Update .continue-here.md only
        4. Skip reconciliation
        5. Other
      </menu_options>

      <menu_options category="major_gaps" condition="missing_files_or_uncommitted">
        1. Fix everything automatically - Create/update files and commit
        2. Show me the diffs first - Preview before committing
        3. Fix files only (no commit) - Update files but don't commit
        4. Update .continue-here.md only - Minimal checkpoint
        5. Skip reconciliation - I'll handle manually
        6. Other
      </menu_options>

    </decision_menu>

    <blocking_wait>
      WAIT for user response - NEVER auto-proceed.
    </blocking_wait>
  </checkpoint_protocol>
</reconciliation_report>

## Phase 5: Remediation Execution

<remediation_strategies>
  <shared_error_handling>
    For all strategies: If git operations fail, display error and return to decision menu. For persistent issues, suggest /research.
  </shared_error_handling>

  Based on user's menu choice, execute appropriate strategy:

  <strategy id="fix_everything_automatically">
    1. Update all state files with current context
    2. Create missing required files (if possible)
    3. Stage all changes: git add [files]
    4. Commit with workflow-appropriate message
    5. Verify commit succeeded: git log -1 --oneline
    6. Confirm completion
  </strategy>

  <strategy id="show_diffs_first">
    1. For each file to be updated: show clear diff preview
    2. Wait for user confirmation
    3. Return to decision menu with modified options:
       - Proceed with commit
       - Abort reconciliation
       - Other
  </strategy>

  <strategy id="fix_files_only">
    1. Update state files (.continue-here.md, PLUGINS.md)
    2. Verify files written successfully and YAML is valid
    3. Do NOT stage or commit
    4. Confirm files updated
  </strategy>

  <strategy id="update_handoff_only">
    1. Update .continue-here.md with current context
    2. Verify file written successfully and YAML is valid
    3. Leave other files unchanged
    4. Confirm minimal checkpoint complete
  </strategy>

  <strategy id="skip_reconciliation">
    1. Exit without changes
    2. Warn user to reconcile manually before workflow resume
  </strategy>
</remediation_strategies>

## Reference Files

- [reconciliation-rules.json](assets/reconciliation-rules.json) - Workflow-specific expectations (includes commit message templates)
- [handoff-formats.md](references/handoff-formats.md) - .continue-here.md structure per workflow
- [reconciliation-examples.md](assets/reconciliation-examples.md) - Example reports and outputs

## Success Criteria

Reconciliation succeeds when:

**State Files**:
- .continue-here.md exists with current workflow, stage/phase, and timestamp
- PLUGINS.md status emoji matches expected state for current stage
- All required_files from reconciliation rule exist at expected paths

**Git Status**:
- No uncommitted workflow artifacts (all tracked files clean or committed)
- No staged but uncommitted changes
- Latest commit message follows workflow-appropriate convention

**Workflow Continuity**:
- Workflow can resume without context loss
- No checkpoint amnesia at workflow boundaries
- State files pass reconciliation-rules.json validation
