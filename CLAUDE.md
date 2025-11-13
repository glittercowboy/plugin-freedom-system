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
- **Milestone Language** → User-facing progress indicators (Build System Ready → Audio Engine Working → UI Integrated → Plugin Complete)
- **Contract Files** → Design Documents (creative-brief.md, parameter-spec.md, architecture.md, plan.md)
- **Checkpoint** → Progress Save Point (commit code, update state, present decision menu)
- **Subagent** → Specialized Worker (focused AI agent for specific implementation milestone)

**System Terms:**
- **Drift** → Design Mismatch (when mockup doesn't match creative brief)
- **Tier** → Investigation Complexity Level (how deep to analyze a problem: quick/moderate/deep)
- **Required Reading** → Critical Patterns (juce8-critical-patterns.md - common mistakes to avoid)

**Internal vs User-Facing:**
- **Internal:** Stage numbers (2-5) used for routing logic, never shown to users
- **User-facing:** Milestone names (Build System Ready, Audio Engine Working, etc.) shown in all menus and messages

When these terms appear in the system, the plain-language equivalent will be shown on first use:
- Example: "Setting up APVTS (Parameter System - handles your knobs and sliders)..."
- Example: "Running pluginval (Validation Tool - checks for crashes)..."
- Example: "Building VST3 (Plugin Format - how DAWs load your plugin)..."

## System Components

- **Scripts**: `scripts/` - Build and installation automation
  - build-and-install.sh - Centralized build automation (7-phase pipeline: validate, build, install, verify)
  - verify-backup.sh - Backup integrity verification (Phase 7)
- **Skills**: `.claude/skills/` - Each skill follows Anthropic's pattern with `SKILL.md`, `references/`, and `assets/` subdirectories
  - plugin-workflow, plugin-ideation, plugin-improve (enhanced with regression testing), ui-mockup, context-resume, plugin-testing, plugin-lifecycle, build-automation, troubleshooting-docs, deep-research, system-setup, workflow-reconciliation
- **Subagents**: `.claude/agents/` - research-planning-agent, foundation-shell-agent, dsp-agent, gui-agent, validation-agent, troubleshoot-agent
- **Commands**: `.claude/commands/` - /setup, /dream, /implement, /improve, /continue, /test, /install-plugin, /uninstall, /show-standalone, /doc-fix, /research
- **Hooks**: `.claude/hooks/` - Validation gates (PostToolUse, SubagentStop, UserPromptSubmit, Stop, PreCompact, SessionStart)
- **Knowledge Base**: `troubleshooting/` - Dual-indexed (by-plugin + by-symptom) problem solutions

## Contracts (Single Source of Truth)

- `plugins/[Name]/.ideas/` - creative-brief.md (vision), parameter-spec.md (parameters), architecture.md (DSP design), plan.md (implementation strategy)
- **State**: PLUGINS.md (all plugins), .continue-here.md (active workflow)
- **Templates**: Contract templates stored in skill assets (`.claude/skills/*/assets/`)

## Key Principles

1. **Contracts are immutable during implementation** - All stages reference the same specs (zero drift)
   - Technical enforcement via PostToolUse hook (blocks Edit/Write to contract files during Stages 2-4)
   - Checksum validation in SubagentStop hook (detects unauthorized modifications)
   - Cross-contract consistency validation (parameter counts, DSP components, complexity scores)
2. **Dispatcher pattern** - Each subagent runs in fresh context (no accumulation)
3. **Discovery through play** - Features found via slash command autocomplete and decision menus
4. **Instructed routing** - Commands expand to prompts, Claude invokes skills
5. **Required Reading injection** - Critical patterns (`juce8-critical-patterns.md`) are mandatory reading for all subagents to prevent repeat mistakes
6. **Proactive validation** - Errors caught early (dependencies at start, brief sync before Stage 2, silent failures at compile-time)

## Proactive Validation (Error Prevention)

The system prevents late-stage failures through multi-layer validation:

**Session Start (SessionStart hook):**
- Validates all dependencies before any work (Python, jq, CMake, Xcode, JUCE, git)
- Reports critical errors with actionable fix commands
- Prevents 10+ minutes of work before discovering missing dependencies

**Stage 0→2 Transition (brief sync):**
- Automatic update of creative brief from finalized mockup
- Ensures contracts reflect final UI design before Stage 2
- No manual reconciliation needed (mockup is source of truth)

**During Implementation (PostToolUse hook):**
- Contract immutability enforcement (blocks modifications to .ideas/*.md during Stages 2-4)
- Real-time safety checks (processBlock validation)
- Silent failure pattern detection (12+ known patterns from juce8-critical-patterns.md)
- Blocks commits with patterns that compile but fail at runtime

**After Subagent Completion (SubagentStop hook):**
- Contract checksum validation (verifies contracts unchanged during Stages 2-4)
- Cross-contract consistency checks (parameter counts, DSP components, references)
- Stage-specific deterministic validation (foundation-shell, DSP, GUI)

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

All implementation stages use the dispatcher pattern:

- Stage 0 → You **must** invoke research-planning-agent via Task tool (plugin-planning skill)
- Stage 2 → You **must** invoke foundation-shell-agent via Task tool (plugin-workflow skill)
- Stage 3 → You **must** invoke dsp-agent via Task tool (plugin-workflow skill)
- Stage 4 → You **must** invoke gui-agent via Task tool (plugin-workflow skill)

The orchestrating skills delegate to subagents, they do **not** implement directly.

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

## GUI-Optional Flow (Phase 7 Enhancement)

**Purpose:** Plugins can skip custom UI (Stage 4) and ship as "headless" plugins using DAW-provided parameter controls. Custom UI can be added later via `/improve`.

**Benefits:**
- **Time savings:** 14 minutes per plugin when custom UI isn't needed (12 min vs 26 min)
- **Faster iteration:** Test DSP immediately after Stage 3 without waiting for UI
- **Progressive enhancement:** Add GUI later when ready via `/improve [PluginName]`
- **Flexibility:** User decides when/if to build custom UI

**Workflow:**

After Stage 3 (DSP) completes, user chooses:
1. **Add custom UI** - Create WebView interface (existing behavior, 15 min)
2. **Ship headless** - Use DAW controls only (new option, 1 min)

**Headless path:**
- Generates minimal PluginEditor (shows plugin name, instructs to use DAW controls)
- Proceeds directly to Stage 5 (Validation)
- Plugin marked as v1.0.0 (Headless)
- All parameters exposed to DAW automation
- No custom UI overhead (smaller binary, faster compile)

**GUI addition via /improve:**
- `/improve [PluginName]` detects headless plugins
- Offers "Create custom UI" option
- Invokes ui-mockup → gui-agent (same as Stage 4)
- Version bumped to v1.1.0 (minor version - new feature)
- Backward compatible (existing automation/presets unaffected)

**State management:**
- New field `gui_type: "headless" | "webview"` in .continue-here.md
- Backward compatible (defaults to "webview" if field missing)
- Used by /improve to detect headless plugins

**Templates:**
- `plugins/TEMPLATE-HEADLESS-EDITOR/` - Minimal PluginEditor templates
- Automatically substituted with plugin name during generation

**Entry point:** After Stage 3 completion (GUI decision gate in plugin-workflow)

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
- **deep-research** (`.claude/skills/deep-research/`) - Multi-level problem investigation (3-level graduated protocol)
- **troubleshooting-docs** (`.claude/skills/troubleshooting-docs/`) - Knowledge base capture with dual-indexing
- **plugin-improve** (`.claude/skills/plugin-improve/`) - Version management with regression testing (enhanced)

### Commands

**Setup:**

- `/setup` - Validate and configure system dependencies (first-time setup)

**Lifecycle:**

- `/dream` - Ideate new plugin concept
- `/implement [Name]` - Build plugin through 5-stage workflow (0, 2-4)
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
- **ui-mockup finalization** auto-updates brief (treats mockup as source of truth for UI decisions)
- **plugin-lifecycle** manages deployment (install/uninstall with cache clearing)
