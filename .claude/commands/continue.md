---
name: continue
description: Resume plugin development from checkpoint
---

# /continue

When user runs `/continue [PluginName?]`, invoke the context-resume skill.

## Behavior

**Without plugin name:**
Search for all `.continue-here.md` handoff files in:
- `plugins/[Name]/.continue-here.md` (implementation)
- `plugins/[Name]/.ideas/.continue-here.md` (ideation/mockup)

Present interactive menu:
```
Which plugin would you like to resume?

1. [PluginName1]
   Stage [N] ([StageName]) • Active development • [time] ago

2. [PluginName2]
   Mockup v[N] ready • Ready to build • [time] ago

3. [PluginName3]
   Creative brief complete • Not started • [time] ago
```

**With plugin name:**
Load handoff file for specific plugin directly.

## Handoff File Locations

Universal handoff system searches:
- **Implementation:** `plugins/[Name]/.continue-here.md`
- **Ideation/Mockup:** `plugins/[Name]/.ideas/.continue-here.md`

## What Gets Loaded

Context automatically loaded:
- Handoff file content (current state, completed work, next steps)
- Recent commits for this plugin
- Source files mentioned in handoff
- Research notes (if Stage 0-1)
- UI mockups (if applicable)

## After Loading

Present summary:
```
Resuming [PluginName] at Stage [N]...

Summary of completed work:
- [Stage 2] Foundation set up
- [Stage 3] Plugin loads in DAW
- [Stage 4.1] Core DSP implemented

Current status:
Working on Stage [N] ([Description]).
[Current state description]

Next steps:
1. [Primary next action]
2. [Alternative action]

Ready to continue?
```

Wait for confirmation, then resume workflow at exact continuation point.

## Error Handling

**No handoff files exist:**
```
No resumable work found.

Handoff files are created throughout the plugin lifecycle:
- After ideation (creative brief complete)
- After mockup creation (design ready)
- During implementation (plugin-workflow stages)
- After improvements (version releases)

Start new work:
- /dream - Explore plugin ideas
- /implement - Build new plugin
```

**Plugin doesn't have handoff:**
```
[PluginName] doesn't have a handoff file.

Possible reasons:
- Plugin is already complete (Stage 6 done)
- Plugin hasn't been started yet
- Development finished and handoff removed

Check status: Look in PLUGINS.md
Modify complete plugin: /improve [PluginName]
Start new plugin: /implement [PluginName]
```

## Routes To

**Skill:** context-resume

The skill handles:
- Reading `.continue-here.md` files
- Parsing current stage and status
- Summarizing completed work
- Loading relevant context
- Proposing next steps
- Continuing workflow from checkpoint
