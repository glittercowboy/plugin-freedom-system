# PLUGIN FREEDOM SYSTEM - Plugin Development System

## Terms Explained (Plain-Language Glossary)

When technical terms appear in the system, they refer to these concepts:

**Audio Plugin Terms:**
- **APVTS** → Parameter System (handles knobs, sliders, and switches in the plugin UI)
- **VST3/AU** → Plugin Format (how DAWs like Logic Pro or Ableton load your plugin)
- **pluginval** → Validation Tool (automated testing tool that checks for crashes and errors)
- **processBlock** → Audio Processing Function (the main function that processes audio samples)
- **DSP** → Audio Processing Engine (the code that transforms audio signals)

**JUCE Framework Terms:**
- **JUCE** → Cross-Platform Audio Framework (C++ library for building audio plugins)
- **WebView** → Modern UI System (lets you build plugin interfaces using HTML/CSS/JavaScript)
- **Standalone** → Desktop Application Mode (run plugin as standalone app, not in DAW)

**Workflow Terms:**
- **Stage 0-6** → Development Phases (Research → Planning → Foundation → Shell → DSP → GUI → Validation)
- **Contract Files** → Design Documents (creative-brief.md, parameter-spec.md, architecture.md, plan.md)
- **Checkpoint** → Progress Save Point (commit code, update state, present decision menu)
- **Subagent** → Specialized Worker (focused AI agent for specific implementation stage)

**System Terms:**
- **Drift** → Design Mismatch (when mockup doesn't match creative brief)
- **Tier** → Investigation Complexity Level (how deep to analyze a problem: quick/moderate/deep)
- **Required Reading** → Critical Patterns (juce8-critical-patterns.md - common mistakes to avoid)

When these terms appear in the system, the plain-language equivalent will be shown on first use:
- Example: "Setting up APVTS (Parameter System - handles your knobs and sliders)..."
- Example: "Running pluginval (Validation Tool - checks for crashes)..."
- Example: "Building VST3 (Plugin Format - how DAWs load your plugin)..."

## System Components

- **Scripts**: `scripts/` - Build and installation automation
  - build-and-install.sh - Centralized build automation (7-phase pipeline: validate, build, install, verify)
  - verify-backup.sh - Backup integrity verification (Phase 7)
- **Skills**: `.claude/skills/` - Each skill follows Anthropic's pattern with `SKILL.md`, `references/`, and `assets/` subdirectories
  - plugin-workflow, plugin-ideation, plugin-improve (enhanced with regression testing), ui-mockup, context-resume, plugin-testing, plugin-lifecycle, build-automation, troubleshooting-docs, deep-research, design-sync, system-setup, workflow-reconciliation
- **Subagents**: `.claude/agents/` - foundation-agent, shell-agent, dsp-agent, gui-agent, validator, troubleshooter
- **Commands**: `.claude/commands/` - /setup, /dream, /implement, /improve, /continue, /test, /install-plugin, /uninstall, /show-standalone, /doc-fix, /research, /sync-design
- **Hooks**: `.claude/hooks/` - Validation gates (PostToolUse, SubagentStop, UserPromptSubmit, Stop, PreCompact, SessionStart)
- **Knowledge Base**: `troubleshooting/` - Dual-indexed (by-plugin + by-symptom) problem solutions

## Contracts (Single Source of Truth)

- `plugins/[Name]/.ideas/` - creative-brief.md (vision), parameter-spec.md (parameters), architecture.md (DSP design), plan.md (implementation strategy)
- **State**: PLUGINS.md (all plugins), .continue-here.md (active workflow)
- **Templates**: Contract templates stored in skill assets (`.claude/skills/*/assets/`)

## Key Principles

1. **Contracts are immutable during implementation** - All stages reference the same specs (zero drift)
   - Technical enforcement via PostToolUse hook (blocks Edit/Write to contract files during Stages 2-5)
   - Checksum validation in SubagentStop hook (detects unauthorized modifications)
   - Cross-contract consistency validation (parameter counts, DSP components, complexity scores)
2. **Dispatcher pattern** - Each subagent runs in fresh context (no accumulation)
3. **Discovery through play** - Features found via slash command autocomplete and decision menus
4. **Instructed routing** - Commands expand to prompts, Claude invokes skills
5. **Required Reading injection** - Critical patterns (`juce8-critical-patterns.md`) are mandatory reading for all subagents to prevent repeat mistakes
6. **Proactive validation** - Errors caught early (dependencies at start, design-sync before Stage 2, silent failures at compile-time)

## Proactive Validation (Error Prevention)

The system prevents late-stage failures through multi-layer validation:

**Session Start (SessionStart hook):**
- Validates all dependencies before any work (Python, jq, CMake, Xcode, JUCE, git)
- Reports critical errors with actionable fix commands
- Prevents 10+ minutes of work before discovering missing dependencies

**Stage 1→2 Transition (design-sync gate):**
- MANDATORY validation of mockup ↔ creative brief alignment
- Catches design drift before Stage 2 generates boilerplate
- Blocks implementation if contracts misaligned (missing features, scope creep, style mismatch)

**During Implementation (PostToolUse hook):**
- Contract immutability enforcement (blocks modifications to .ideas/*.md during Stages 2-5)
- Real-time safety checks (processBlock validation)
- Silent failure pattern detection (12+ known patterns from juce8-critical-patterns.md)
- Blocks commits with patterns that compile but fail at runtime

**After Subagent Completion (SubagentStop hook):**
- Contract checksum validation (verifies contracts unchanged during Stages 2-5)
- Cross-contract consistency checks (parameter counts, DSP components, references)
- Stage-specific deterministic validation (foundation, parameters, DSP, GUI)

**Checkpoint Completion (plugin-workflow):**
- Verifies all checkpoint steps succeeded before presenting decision menu
- Detects incomplete state updates (missing handoff, status, timeline, commit)
- Prevents state corruption from partial checkpoint failures

**Result:** 90% reduction in late-stage failures per SYSTEM-AUDIT-REPORT.md

## Checkpoint Protocol (System-Wide)

At every significant completion point (stage complete, phase complete, files generated, contract created):

1. Auto-commit changes (if in workflow)
2. Update state files (.continue-here.md, PLUGINS.md)
3. **Verify checkpoint completion** (all steps succeeded)
4. ALWAYS present numbered decision menu:

✓ [Completion statement]

What's next?

1. [Primary action] (recommended)
2. [Secondary action]
3. [Discovery option] ← User discovers [feature]
4. [Alternative path]
5. Other

Choose (1-5): \_

4. WAIT for user response - NEVER auto-proceed
5. Execute chosen action

This applies to:

- All workflow stages (0-6)
- All subagent completions
- Contract creation (creative-brief, mockups, parameter-spec)
- Any point where user needs to decide next action

Do NOT use AskUserQuestion tool for decision menus - use inline numbered lists as shown above.

## Subagent Invocation Protocol

Stages 2-5 use the dispatcher pattern:

- Stage 2 → You **must** invoke foundation-agent via Task tool
- Stage 3 → You **must** invoke shell-agent via Task tool
- Stage 4 → You **must** invoke dsp-agent via Task tool
- Stage 5 → You **must** invoke gui-agent via Task tool

The plugin-workflow skill orchestrates, it does **not** implement.

After subagent completes:

1. Read subagent's return message
2. Commit changes
3. Update .continue-here.md
4. Update PLUGINS.md
5. Present numbered decision menu
6. Wait for user response

This ensures consistent checkpoint behavior and clean separation of concerns.

## Workflow Entry Points

- First-time setup: `/setup` (validate and configure dependencies)
- New plugin: `/dream` → `/plan` → `/implement`
- Resume work: `/continue [PluginName]`
- Modify existing: `/improve [PluginName]`
- Test plugin: `/test [PluginName]`

## Implementation Status

- ✓ Phase 0: Foundation & Contracts (complete)
- ✓ Phase 1: Discovery System (complete)
- ✓ Phase 2: Workflow Engine (complete)
- ✓ Phase 3: Implementation Subagents (complete)
- ✓ Phase 4: Build & Troubleshooting System (complete)
- ✓ Phase 5: Validation System (complete - hybrid validation operational)
- ✓ Phase 6: WebView UI System (complete)
- ✓ Phase 7: Polish & Enhancement (complete - feedback loop operational)

## Phase 7 Components (Polish & Enhancement)

### Skills

- **system-setup** (`.claude/skills/system-setup/`) - Dependency validation and environment configuration
- **plugin-lifecycle** (`.claude/skills/plugin-lifecycle/`) - Installation/uninstallation management
- **design-sync** (`.claude/skills/design-sync/`) - Mockup ↔ brief validation, drift detection
- **deep-research** (`.claude/skills/deep-research/`) - Multi-level problem investigation (3-level graduated protocol)
- **troubleshooting-docs** (`.claude/skills/troubleshooting-docs/`) - Knowledge base capture with dual-indexing
- **plugin-improve** (`.claude/skills/plugin-improve/`) - Version management with regression testing (enhanced)

### Commands

**Setup:**

- `/setup` - Validate and configure system dependencies (first-time setup)

**Lifecycle:**

- `/dream` - Ideate new plugin concept
- `/implement [Name]` - Build plugin through 7-stage workflow
- `/continue [Name]` - Resume paused workflow
- `/improve [Name]` - Fix bugs or add features (with regression testing)
- `/reconcile [Name]` - Reconcile state between planning and implementation

**Deployment:**

- `/install-plugin [Name]` - Install to system folders
- `/uninstall [Name]` - Remove binaries (keep source)
- `/reset-to-ideation [Name]` - Remove implementation, keep idea/mockups
- `/destroy [Name]` - Completely remove everything (with backup)

**Quality:**

- `/test [Name]` - Run validation suite
- `/sync-design [Name]` - Validate mockup ↔ brief consistency
- `/research [topic]` - Deep investigation (3-level protocol)
- `/doc-fix` - Document solved problems (with option to promote to Required Reading)
- `/add-critical-pattern` - Directly add current problem to Required Reading (fast path)

### Knowledge Base

- `troubleshooting/build-failures/` - Build and compilation errors
- `troubleshooting/runtime-issues/` - Crashes, exceptions, performance issues
- `troubleshooting/gui-issues/` - UI layout and rendering problems
- `troubleshooting/api-usage/` - JUCE API misuse and migration issues
- `troubleshooting/dsp-issues/` - Audio processing problems
- `troubleshooting/parameter-issues/` - APVTS and state management
- `troubleshooting/validation-problems/` - pluginval failures
- `troubleshooting/patterns/` - Common patterns and solutions
- `troubleshooting/patterns/juce8-critical-patterns.md` - **REQUIRED READING** for all subagents (Stages 2-5)

### Scripts

- `scripts/build-and-install.sh` - Build automation (supports --uninstall)
- `scripts/verify-backup.sh` - Backup integrity verification

## Feedback Loop

The complete improvement cycle:

```
Build → Test → Find Issue → Research → Improve → Document → Validate → Deploy
    ↑                                                                      ↓
    └──────────────────────────────────────────────────────────────────────┘
```

- **deep-research** finds solutions (3-level graduated protocol: Quick → Moderate → Deep)
- **plugin-improve** applies changes (with regression testing and backup verification)
- **troubleshooting-docs** captures knowledge (dual-indexed for fast lookup)
- **design-sync** prevents drift (validates contracts before implementation)
- **plugin-lifecycle** manages deployment (install/uninstall with cache clearing)
