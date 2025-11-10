---
name: plugin-improve
description: Fix bugs and add features with versioning and backups
allowed-tools:
  - Read
  - Write
  - Edit
  - Bash
  - Task # For deep-research (Tier 3)
preconditions:
  - Plugin status must be ✅ Working OR 📦 Installed
  - Plugin must NOT be 🚧 In Development
---

# plugin-improve Skill

**Purpose:** Make changes to completed plugins with versioning, backups, changelog automation, and root cause investigation.

## Precondition Checking

**Before starting, verify:**

1. Read PLUGINS.md:

```bash
grep "^### $PLUGIN_NAME$" PLUGINS.md
```

2. Check status:
   - If status = ✅ Working or 📦 Installed → OK to proceed
   - If status = 🚧 In Development → BLOCK with message:
     ```
     [PluginName] is still in development (Stage [N]).
     Complete the workflow first with /continue [PluginName].
     Cannot use /improve on in-progress plugins.
     ```
   - If status = 💡 Ideated → BLOCK with message:
     ```
     [PluginName] is not implemented yet (Status: 💡 Ideated).
     Use /implement [PluginName] to build it first.
     ```
   - If not found → BLOCK with message:
     ```
     Plugin [PluginName] not found in PLUGINS.md.
     ```

## Phase 0: Vagueness Detection

**Check if request is specific:**

Request IS specific if it has:

- Feature name (e.g., "resonance parameter", "bypass switch")
- Action (e.g., "add", "remove", "fix", "change from X to Y")
- Acceptance criteria (e.g., "range 0-1", "increase to 500px", "reduce by 3dB")

Request IS vague if lacking above:

- "improve the filters"
- "better presets"
- "UI feels cramped"
- "make it sound warmer"

**If vague, present choice:**

```
Your request is somewhat vague. How should I proceed?

1. Brainstorm approaches first (recommended) ← Explore options together
2. Implement with my best assumptions ← I'll investigate and propose a solution

Choose (1-2): _
```

**Handle responses:**

- Option 1 → Invoke `plugin-ideation` skill in improvement mode, then return here when ready
- Option 2 → Proceed to Phase 0.5 (Investigation)

## Phase 0.5: Investigation (3-Tier)

**Purpose:** Find root causes, prevent band-aid fixes

**Tier Selection:**

- **Tier 1 (5-10 min):** Cosmetic changes, simple fixes, obvious issues
- **Tier 2 (15-30 min):** Logic errors, parameter issues, integration bugs
- **Tier 3 (30-60 min):** Complex bugs, performance issues, architectural problems

**Tier 1: Basic Code Inspection**

Read relevant source files:

- PluginProcessor.cpp/h
- PluginEditor.cpp/h
- Relevant JUCE modules

Check for:

- Obvious typos or errors
- Known pattern matches
- Simple logic issues

**Tier 2: Root Cause Analysis**

Deeper investigation:

- Trace logic flow from symptom to cause
- Check integration points between components
- Review parameter definitions and usage
- Examine state management
- Check threading issues (processBlock vs GUI)

**Tier 3: Deep Research**

Invoke `deep-research` skill (Phase 7 - stub for now):

```
Complex issue detected. Invoking deep-research skill...

NOTE: deep-research skill will be implemented in Phase 7.
For now, performing manual comprehensive investigation.
```

For now, perform thorough manual investigation.

**Present findings:**

```markdown
## Investigation Findings

### Problem Analysis

[What's actually wrong and why it's happening]

### Root Cause

[Technical explanation of the underlying issue]

### Affected Files

- plugins/[Name]/Source/[File]:[Line]
- plugins/[Name]/Source/[File]:[Line]

### Recommended Approach

[How to fix it properly - not a workaround]

### Alternative Approaches

[Other valid options with trade-offs explained]

### Backward Compatibility

[Will this break existing presets/sessions?]

Proceed with recommended approach? (y/n): \_
```

**Wait for approval before implementing.**

If user says no, ask which alternative or if they want different approach.

## Phase 1: Pre-Implementation Checks

**Load current state:**

1. Read CHANGELOG.md:

```bash
cat plugins/[PluginName]/CHANGELOG.md
```

Extract current version (e.g., v1.2.3).

2. Read PLUGINS.md entry for additional context.

3. Check recent commits:

```bash
git log --oneline plugins/[PluginName]/ -10
```

**Determine version bump:**

Present choice:

```
Current version: v[X.Y.Z]

What type of change is this?
1. PATCH (v[X.Y.Z] → v[X.Y.Z+1]) - Bug fixes, cosmetic changes
2. MINOR (v[X.Y] → v[X.Y+1]) - New features, enhancements
3. MAJOR (v[X] → v[X+1]) - Breaking changes (presets won't load, parameters changed)

Choose (1-3): _
```

**If Major version selected, warn:**

```
⚠️ Major version bump will break compatibility.

Breaking changes include:
- Changed parameter IDs (presets won't load)
- Removed parameters (sessions will have missing automation)
- Changed state format (existing sessions corrupted)

Are you sure? This should be rare. (y/n): _
```

Calculate new version based on selection.

## Phase 2: Backup Creation

**Before ANY code changes:**

```bash
mkdir -p backups/
cp -r plugins/[PluginName] backups/[PluginName]-v[X.Y.Z]-$(date +%Y%m%d-%H%M%S)
```

Confirm backup created:

```
✓ Backup created: backups/[PluginName]-v[X.Y.Z]-[timestamp]

This backup can be restored if anything goes wrong.
```

## Phase 3: Implementation

**Make the approved changes:**

1. Modify source files according to investigation findings
2. Update build configuration if needed (CMakeLists.txt)
3. Adjust UI if required (PluginEditor.cpp)
4. Update parameter definitions if needed (PluginProcessor.cpp)

**Follow best practices:**

- Real-time safety in processBlock
- No allocations in audio thread
- Thread-safe parameter access
- JUCE API correctness

**Log changes as you go for CHANGELOG.**

## Phase 4: CHANGELOG Update

**Update CHANGELOG.md following Keep a Changelog format:**

Add new version entry at top:

```markdown
# Changelog

All notable changes to [PluginName] will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [X.Y.Z] - [YYYY-MM-DD]

### Added

- [New feature 1]
- [New feature 2]

### Changed

- [Modified behavior 1]
- [Modified behavior 2]

### Fixed

- [Bug fix 1]
- [Bug fix 2]

### Removed

- [Deprecated feature if any]

## [Previous Version] - [Date]

[Previous entries remain...]
```

**Sections to use:**

- **Added:** New features
- **Changed:** Changes to existing functionality
- **Deprecated:** Soon-to-be removed features
- **Removed:** Removed features
- **Fixed:** Bug fixes
- **Security:** Security fixes (rare for audio plugins)

## Phase 5: Build & Test

**1. Build plugin:**

Invoke `build-automation` skill (full build with installation):

```
Invoking build-automation skill to build and install updated plugin...
```

build-automation will:
- Run build script: `scripts/build-and-install.sh [PluginName]` (full build)
- Build VST3 and AU formats in parallel
- Install to system folders
- Clear DAW caches
- Verify installation

If build succeeds:
- build-automation displays success message with installation paths
- Returns control to plugin-improve
- Proceed to Phase 5, step 2 (Run tests)

If build fails:
- build-automation presents 4-option failure protocol:
  1. Investigate (troubleshooter agent)
  2. Show build log
  3. Show code
  4. Wait for manual fix
- After resolution and successful retry, returns to plugin-improve
- Proceed to Phase 5, step 2 (Run tests)

**Note:** Build failure handling is entirely managed by build-automation skill. plugin-improve does not need custom build error menus.

**2. Run tests:**

Invoke `plugin-testing` skill (Phase 1b Task 8):

Present test method choice:

```
Build successful. How would you like to test?

1. Automated stability tests (if Tests/ exists)
2. Build and run pluginval (recommended)
3. Manual DAW testing checklist
4. Skip testing (not recommended)

Choose (1-4): _
```

If tests fail, present investigation options.

## Phase 6: Git Workflow

**Stage changes:**

```bash
git add plugins/[PluginName]/
git add backups/[PluginName]-v[X.Y.Z]-[timestamp]/  # Include backup in git
```

**Commit with conventional format:**

```bash
# Format: improve: [PluginName] v[X.Y.Z] - [brief description]
# Example: improve: MicroGlitch v1.3.0 - add preset system

git commit -m "improve: [PluginName] v[X.Y.Z] - [description]"
```

**Tag release:**

```bash
git tag -a "v[X.Y.Z]" -m "[PluginName] v[X.Y.Z]"
```

Note: User handles actual git operations, we stage only.

**Confirm git ready:**

```
✓ Changes staged for commit
✓ Tag ready: v[X.Y.Z]

Git commit message:
  improve: [PluginName] v[X.Y.Z] - [description]

You can commit these changes when ready.
```

## Phase 7: Installation

**Present decision:**

```
Build and tests successful. Install to system folders?

1. Yes, install now (recommended for 📦 Installed plugins)
2. No, test from build folder first
3. Skip installation

Choose (1-3): _
```

**If user chooses 1:**

Invoke `plugin-lifecycle` skill (Phase 1b Task 9):

```
Installing [PluginName] v[X.Y.Z]...
```

**Update PLUGINS.md:**

Update version number:

```markdown
**Version:** [X.Y.Z]
**Last Updated:** [YYYY-MM-DD]
```

If status was ✅ Working and now installed, change to 📦 Installed.

## Phase 8: Completion

**Present decision menu:**

```
✓ [PluginName] v[X.Y.Z] complete

What's next?
1. Test in DAW (recommended)
2. Make another improvement
3. Create new plugin
4. Document this change
5. Other

Choose (1-5): _
```

**Handle responses:**

- Option 1 → Provide manual testing guidance
- Option 2 → Ask what to improve, restart workflow
- Option 3 → Suggest `/dream` or `/implement`
- Option 4 → Suggest creating documentation
- Option 5 → Ask what they'd like to do

## Breaking Change Detection

**Automatically detect breaking changes:**

If implementation modifies:

- Parameter IDs
- Parameter count (removed parameters)
- State format (APVTS structure)
- Audio routing (channel count)

Warn:

```
⚠️ Breaking change detected

This change will cause:
- Existing presets may not load correctly
- Session automation may break
- Users need to recreate presets

This requires a MAJOR version bump (v[X].0.0).

Proceed with breaking change? (y/n): _
```

If user confirms, force MAJOR version bump.

## Rollback Support

**If anything goes wrong:**

Provide rollback instructions:

```
To restore backup:

1. Remove modified version:
   rm -rf plugins/[PluginName]

2. Restore backup:
   cp -r backups/[PluginName]-v[X.Y.Z]-[timestamp] plugins/[PluginName]

3. Rebuild:
   cmake --build build

Backup location: backups/[PluginName]-v[X.Y.Z]-[timestamp]
```

## Version Bump Logic

**PATCH (v1.2.3 → v1.2.4):**

- Bug fixes
- Performance improvements
- Cosmetic UI changes
- Documentation updates
- No new features
- No breaking changes

**MINOR (v1.2 → v1.3):**

- New features added
- New parameters added
- UI enhancements
- Backward compatible
- Existing presets still work

**MAJOR (v1 → v2):**

- Breaking changes
- Parameter IDs changed
- Parameters removed
- State format changed
- Requires preset migration
- Should be rare

## Integration Points

**Invoked by:**

- `/improve` command
- Natural language: "Fix [plugin]", "Add [feature] to [plugin]"
- `plugin-ideation` skill (after improvement brief)

**Invokes:**

- `plugin-ideation` skill (if vague request, user chooses brainstorm)
- `deep-research` skill (Tier 3 investigation) - Phase 7
- `build-automation` skill (building) - Phase 4
- `plugin-testing` skill (validation)
- `plugin-lifecycle` skill (installation)

**Updates:**

- CHANGELOG.md (adds version entry)
- PLUGINS.md (version number, last updated)
- Source files (implementation changes)

**Creates:**

- Backup in `backups/[PluginName]-v[X.Y.Z]-[timestamp]/`
- Git tag `v[X.Y.Z]`

## Error Handling

**Build failure:**
Present investigation menu, wait for user decision.

**Test failure:**
Present investigation menu, don't proceed to installation.

**Breaking change detected:**
Warn user, require confirmation, force MAJOR version.

**Backup creation fails:**
STOP immediately, don't proceed with changes.

**Git operations fail:**
Log warning, continue (non-critical).

## Success Criteria

Improvement is successful when:

- Root cause investigated (not band-aid fix)
- Backup created before changes
- Changes implemented correctly
- CHANGELOG updated with version entry
- Build succeeds without errors
- Tests pass
- Git staged with conventional commit message
- PLUGINS.md updated
- User knows how to rollback if needed
