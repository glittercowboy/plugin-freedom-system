---
name: test
description: Validate plugins through automated tests
---

# /test

When user runs `/test [PluginName?] [mode?]`, invoke the plugin-testing skill or build-automation skill.

## Preconditions

<preconditions enforcement="blocking">
  <check target="PLUGINS.md" condition="plugin_exists">
    Plugin entry MUST exist in PLUGINS.md
  </check>
  <check target="PLUGINS.md::status" condition="not_equals(💡 Ideated)">
    Status MUST NOT be 💡 Ideated (requires implementation to test)
  </check>
  <rejection_message status="💡 Ideated">
[PluginName] is not implemented yet (Status: 💡 Ideated).
Use /implement [PluginName] to build it first.
  </rejection_message>
</preconditions>

<state_interactions>
  <reads>
    - PLUGINS.md (plugin status, test capabilities)
  </reads>
  <writes>
    - None (testing results logged to console, skills handle any state updates)
  </writes>
</state_interactions>

## Three Test Methods

**Mode: automated (plugin-testing skill)**
- **Requirement:** plugins/{PluginName}/Tests/ directory exists
- **Invocation:** Invoke the plugin-testing skill via Skill tool
- **Duration:** ~2 minutes
- **Tests:** Parameter stability (all combinations, edge cases), state save/restore (preset corruption), processing stability (buffer sizes, sample rates), thread safety (concurrent access), edge case handling (silence, extremes, denormals)

**Mode: build (build-automation skill)**
- **Requirement:** Always available (no dependencies)
- **Invocation:** Invoke the build-automation skill via Skill tool
- **Duration:** 5-10 minutes
- **Steps:** Build Release mode (VST3 + AU), run pluginval validation tool with strict settings (level 10), install to system folders, clear DAW caches

**Mode: manual (checklist only)**
- **Requirement:** Always available
- **Invocation:** Display checklist directly (no skill invocation)
- **Checklist items:** Load & initialize, audio processing, parameter testing, state management, performance, compatibility, stress testing

## Behavior

<behavior>
  <case arguments="none">
    List available plugins with test status from PLUGINS.md
  </case>
  <case arguments="plugin_only">
    Present test method decision menu (adapt based on Tests/ directory existence)
  </case>
  <case arguments="plugin_and_mode">
    Execute test directly via appropriate skill
  </case>
</behavior>

## Integration

This command is also auto-invoked by plugin-workflow after Stage 4 (DSP) and Stage 5 (GUI).

## Error Handling

<error_handling>
  <on_failure type="automated_tests|build|pluginval">
    Present standard failure menu:
    1. Investigate (trigger deep-research skill)
    2. Show me the code
    3. Show full output
    4. I'll fix it manually (or continue anyway for pluginval)
  </on_failure>
</error_handling>

## Output

After successful testing, display:
- Success message with test method
- Next step suggestions (manual DAW testing, /improve if issues found)
