---
name: improve
description: Modify completed plugins with versioning
argument-hint: "[PluginName] [description?]"
---

# /improve

When user runs `/improve [PluginName] [description?]`, route based on argument presence (see Routing section below).

## Preconditions

<preconditions enforcement="blocking">
  <status_verification target="PLUGINS.md" required="true">
    <check condition="plugin_exists">
      Plugin entry MUST exist in PLUGINS.md
    </check>

    <check condition="status_in(✅ Working, 📦 Installed)">
      Status MUST be ✅ Working OR 📦 Installed

      <on_violation status="🚧 In Development">
        REJECT with message:
        "[PluginName] is still in development (Stage [N]).
        Complete the workflow first with /continue [PluginName].
        Cannot use /improve on in-progress plugins."
      </on_violation>

      <on_violation status="💡 Ideated">
        REJECT with message:
        "[PluginName] is not implemented yet (Status: 💡 Ideated).
        Use /implement [PluginName] to build it first."
      </on_violation>

      <on_violation status="[any other status]">
        REJECT with message:
        "[PluginName] has status '[Status]' in PLUGINS.md.
        Only ✅ Working or 📦 Installed plugins can be improved."
      </on_violation>
    </check>
  </status_verification>
</preconditions>

## Routing

<routing_logic>
  <decision_gate type="argument_routing">
    <path condition="no_plugin_name">
      Read PLUGINS.md and filter for plugins with status ✅ Working OR 📦 Installed

      <menu_presentation>
        Display: "Which plugin would you like to improve?"

        [Numbered list of completed plugins with status emoji]

        Example:
        1. DriveVerb (📦 Installed)
        2. CompressorPro (✅ Working)
        3. Other (specify name)

        WAIT for user selection
      </menu_presentation>

      After selection, proceed to path condition="plugin_name_only"
    </path>

    <path condition="plugin_name_only">
      Check for existing improvement briefs in plugins/[PluginName]/improvements/*.md

      <decision_menu>
        IF briefs exist:
          Display:
          "What would you like to improve in [PluginName]?

          1. From existing brief: [brief-name-1].md
          2. From existing brief: [brief-name-2].md
          3. Describe a new change"

        IF no briefs:
          Display:
          "What would you like to improve in [PluginName]?

          Describe the change:"

        WAIT for user response
      </decision_menu>

      After description provided, proceed to vagueness_check
    </path>

    <path condition="plugin_name_and_description">
      Perform vagueness_check (see below)

      <vagueness_check>
        IF request is vague:
          Present vagueness handling menu (see Vagueness Detection section)
        ELSE IF request is specific:
          Proceed to plugin-improve skill with Phase 0.5 investigation
      </vagueness_check>
    </path>
  </decision_gate>

  <skill_invocation trigger="after_routing_complete">
    Once plugin name and specific description are obtained, invoke plugin-improve skill:

    <invocation_syntax>
      Use Skill tool: skill="plugin-improve"

      Pass context:
      - pluginName: [name]
      - description: [specific change description]
      - vagueness_resolution: [if applicable, which path user chose from vagueness menu]
    </invocation_syntax>
  </skill_invocation>
</routing_logic>

## Vagueness Detection

<vagueness_detection>
  Evaluate request specificity using criteria: feature name, action, acceptance criteria.

  Detection logic and examples documented in plugin-improve skill references.

  <handling>
    IF vague: Present choice menu (brainstorm OR implement with investigation)
    IF specific: Proceed directly to plugin-improve skill with Phase 0.5 investigation
  </handling>
</vagueness_detection>

## Plugin-Improve Workflow

<skill_workflow>
  The plugin-improve skill executes a complete improvement cycle:

  <workflow_overview>
    1. Investigation (Phase 0.5 root cause analysis)
    2. Approval gate (user confirmation)
    3. Version selection (PATCH/MINOR/MAJOR)
    4. Backup → Implement → Document → Build → Deploy
  </workflow_overview>

  <reference>
    See plugin-improve skill documentation for full 10-step workflow details.
  </reference>

  <output_guarantee>
    Changes are production-ready:
    - Versioned and backed up (backups/[Plugin]/v[X.Y.Z]/)
    - Built in Release mode
    - Installed to system folders
    - Documented in CHANGELOG
    - Git history preserved (commit + tag)
  </output_guarantee>
</skill_workflow>

## State Management

<state_management>
  State operations (file reads/writes, git operations) documented in plugin-improve skill references.
</state_management>
