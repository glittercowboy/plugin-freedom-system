# PLUGIN FREEDOM SYSTEM - Plugin Development System

## System Components
- Skills: `.claude/skills/` - plugin-workflow, plugin-ideation, plugin-improve, ui-mockup, context-resume, plugin-testing, plugin-lifecycle
- Subagents: `.claude/agents/` - foundation-agent, shell-agent, dsp-agent, gui-agent, validator, troubleshooter
- Commands: `.claude/commands/` - /dream, /implement, /improve, /continue, /test, /install-plugin, /show-standalone, /troubleshoot-juce, /doc-fix
- Hooks: `.claude/hooks/` - Validation gates (PostToolUse, SubagentStop, UserPromptSubmit, Stop, PreCompact, SessionStart)

## Contracts (Single Source of Truth)
- `plugins/[Name]/.ideas/` - creative-brief.md (vision), parameter-spec.md (parameters), architecture.md (DSP design), plan.md (implementation strategy)
- State: PLUGINS.md (all plugins), .continue-here.md (active workflow)
- Templates: `templates/` - Contract boilerplate for skills to use

## Key Principles
1. **Contracts are immutable during implementation** - All stages reference the same specs (zero drift)
2. **Dispatcher pattern** - Each subagent runs in fresh context (no accumulation)
3. **State machine protection** - Only ONE plugin can be 🚧 at a time
4. **Discovery through play** - Features found via slash command autocomplete and decision menus
5. **Instructed routing** - Commands expand to prompts, Claude invokes skills

## Workflow Entry Points
- New plugin: `/dream` → `/implement`
- Resume work: `/continue [PluginName]`
- Modify existing: `/improve [PluginName]`
- Test plugin: `/test [PluginName]`

## Implementation Status
- ✓ Phase 0: Foundation & Contracts (complete)
- Phase 1: Discovery System (next)
