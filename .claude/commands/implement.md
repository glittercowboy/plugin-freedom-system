---
name: implement
description: Build plugin through implementation stages 2-6
argument-hint: [PluginName?]
allowed-tools: Bash(test:*)
---

# /implement

When user runs `/implement [PluginName?]`, invoke the plugin-workflow skill to build the plugin (stages 2-6 only).

<prerequisite>
  Planning (stages 0-1) must be completed first via `/plan` command.
</prerequisite>

## Preconditions

<preconditions enforcement="blocking">
  <decision_gate type="status_verification" source="PLUGINS.md">
    <allowed_state status="🚧 Stage 1" description="Planning complete">
      Start implementation at Stage 2
    </allowed_state>

    <allowed_state status="🚧 Stage 2-6" description="In progress">
      Resume implementation at current stage
    </allowed_state>

    <blocked_state status="💡 Ideated" action="redirect">
      <error_message>
        [PluginName] planning is not complete.

        Run /plan [PluginName] first to complete stages 0-1:
        - Stage 0: Research → architecture.md
        - Stage 1: Planning → plan.md

        Then run /implement to build (stages 2-6).
      </error_message>
    </blocked_state>

    <blocked_state status="🚧 Stage 0" action="redirect">
      <error_message>
        [PluginName] planning is not complete.

        Run /plan [PluginName] first to complete stages 0-1:
        - Stage 0: Research → architecture.md
        - Stage 1: Planning → plan.md

        Then run /implement to build (stages 2-6).
      </error_message>
    </blocked_state>

    <blocked_state status="✅ Working" action="redirect">
      <error_message>
        [PluginName] is already implemented and working.

        Use /improve [PluginName] to make changes or add features.
      </error_message>
    </blocked_state>
  </decision_gate>

  <decision_gate type="contract_verification" blocking="true">
    <required_contracts>
      <contract path="plugins/${PLUGIN_NAME}/.ideas/architecture.md" created_by="Stage 0"/>
      <contract path="plugins/${PLUGIN_NAME}/.ideas/plan.md" created_by="Stage 1"/>
      <contract path="plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" created_by="ideation"/>
    </required_contracts>

    <validation_command>
      # See .claude/skills/plugin-workflow/references/precondition-checks.sh for reusable check functions
      test -f "plugins/${PLUGIN_NAME}/.ideas/architecture.md" &amp;&amp; \
      test -f "plugins/${PLUGIN_NAME}/.ideas/plan.md" &amp;&amp; \
      test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md"
    </validation_command>

    <on_failure action="BLOCK">
      <error_message>
        ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
        ✗ BLOCKED: Missing planning artifacts
        ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

        Implementation requires complete planning contracts:

        Required contracts:
        [✓/✗] architecture.md - [exists/MISSING]
        [✓/✗] plan.md - [exists/MISSING]
        [✓/✗] parameter-spec.md - [exists/MISSING]

        HOW TO UNBLOCK:
        1. Run: /plan [PluginName]
           - Completes Stage 0 (Research) → architecture.md
           - Completes Stage 1 (Planning) → plan.md

        2. If parameter-spec.md missing:
           - Run: /dream [PluginName]
           - Create and finalize UI mockup
           - Finalization generates parameter-spec.md

        Once all contracts exist, /implement will proceed.
      </error_message>
    </on_failure>
  </decision_gate>
</preconditions>

## Behavior

**If no plugin name provided:**
1. Read PLUGINS.md and filter for plugins with status 🚧 Stage 1 or 🚧 Stage 2-6
2. Present numbered menu of eligible plugins with current stage
3. Wait for user selection

**If plugin name provided:**
1. Parse plugin name from arguments
2. Verify preconditions (status check, contract verification)
3. If preconditions pass: Invoke plugin-workflow skill via Skill tool
4. If preconditions fail: Display blocking error and stop

**Skill invocation:**
Invoke the plugin-workflow skill with the plugin name and starting stage. The skill handles stages 2-6 implementation using the subagent dispatcher pattern.

## The Implementation Stages

<skill_delegation>
  <command_responsibility>
    This command is a ROUTING LAYER:
    1. Verify preconditions (status check, contract verification)
    2. Invoke plugin-workflow skill via Skill tool
    3. Return control to user
  </command_responsibility>

  <skill_responsibility ref="plugin-workflow">
    The plugin-workflow skill orchestrates stages 2-6:

    Stage 2 (Foundation) → Stage 3 (Shell) → Stage 4 (DSP) → Stage 5 (GUI) → Stage 6 (Validation)

    Each stage uses specialized subagent, follows checkpoint protocol (commit, state update, decision menu).

    For stage details, see .claude/skills/plugin-workflow/SKILL.md
  </skill_responsibility>

  <handoff_point>
    Command completes immediately after invoking plugin-workflow skill.
    All subsequent interaction (stage progression, decision menus) happens within skill context.
  </handoff_point>
</skill_delegation>

## Decision Menus

<decision_menus>
  Plugin-workflow skill presents decision menus at each stage completion.
  Decision menu format documented in plugin-workflow skill references.
  Command does NOT present menus - this is skill's responsibility after delegation.
</decision_menus>

## Pause & Resume

If user pauses:
- .continue-here.md updated with current stage
- PLUGINS.md status updated
- Changes committed

Resume with `/continue [PluginName]`

## Output

<expected_output>
  By completion, plugin has:
  - Compiling VST3 + AU plugins
  - Working audio processing + functional WebView UI
  - Pluginval compliant with factory presets
  - Complete git history for all stages
</expected_output>

## Workflow Integration

Complete plugin development flow:
1. `/dream [PluginName]` - Creative brief + UI mockup
2. `/plan [PluginName]` - Research and planning (Stages 0-1)
3. `/implement [PluginName]` - Build plugin (Stages 2-6)
4. `/install-plugin [PluginName]` - Deploy to system folders
