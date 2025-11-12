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

**If no plugin name provided:**
1. Read PLUGINS.md and filter for plugins with status ✅ Working or 📦 Installed
2. Display numbered menu: "Which plugin would you like to improve?"
   - Example: 1. DriveVerb (📦 Installed), 2. CompressorPro (✅ Working), 3. Other (specify name)
3. Wait for user selection
4. After selection, proceed to "plugin name only" path

**If plugin name only (no description):**
1. Check for existing improvement briefs in plugins/[PluginName]/improvements/*.md
2. If briefs exist: Present menu with existing briefs + "Describe a new change" option
3. If no briefs: Prompt "What would you like to improve in [PluginName]? Describe the change:"
4. Wait for user response
5. After description provided, proceed to vagueness check

**If plugin name and description provided:**
1. Perform vagueness check (see Vagueness Detection section)
2. If request is vague: Present vagueness handling menu
3. If request is specific: Proceed to plugin-improve skill with Phase 0.5 investigation

**Skill invocation:**
Once plugin name and specific description are obtained, invoke the plugin-improve skill via Skill tool with:
- pluginName: [name]
- description: [specific change description]
- vagueness_resolution: [if applicable, which path user chose from vagueness menu]

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
