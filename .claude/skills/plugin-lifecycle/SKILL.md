---
name: plugin-lifecycle
description: Manage complete plugin lifecycle - install, uninstall, reset, destroy
allowed-tools:
  - Bash
  - Read
  - Edit # For PLUGINS.md updates
  - Write # For backup metadata
preconditions:
  - Varies by mode (see mode-specific preconditions)
---

# plugin-lifecycle Skill

**Purpose:** Manage the complete lifecycle of plugins from installation to removal with proper state tracking and safety features.

## Overview

This skill handles all plugin lifecycle operations on macOS:

- **Installation (Mode 1)**: Copy Release binaries to system folders (`~/Library/Audio/Plug-Ins/`)
- **Uninstallation (Mode 2)**: Clean removal from system folders (preserves source code)
- **Reset to Ideation (Mode 3)**: Remove implementation, keep idea/mockups (surgical rollback)
- **Destroy (Mode 4)**: Complete removal with backup (nuclear option)

All operations include proper permissions, cache clearing, state tracking, and safety features (confirmations, backups).

---

## Mode Dispatcher

This skill operates in different modes based on the invoking command:

| Mode | Operation | Command | Purpose |
|------|-----------|---------|---------|
| 1 | Installation | `/install-plugin` | Deploy to system folders |
| 2 | Uninstallation | `/uninstall` | Remove binaries, keep source |
| 3 | Reset to Ideation | `/reset-to-ideation` | Remove implementation, keep idea/mockups |
| 4 | Destroy | `/destroy` | Complete removal with backup |
| Menu | Interactive | `/clean` | Present menu, user chooses mode |

**Pattern:** Commands are thin routers that invoke this skill with a specific mode. The skill dispatches to the appropriate reference file for detailed implementation.

**Why this matters:**

DAWs scan system folders for plugins. Installing to the correct locations with proper permissions ensures your plugin appears in the DAW's plugin list.

**System folders (macOS):**

- **VST3**: `~/Library/Audio/Plug-Ins/VST3/`
- **AU (Audio Unit)**: `~/Library/Audio/Plug-Ins/Components/`
- **AAX** (Pro Tools): `~/Library/Application Support/Avid/Audio/Plug-Ins/` (future support)

---

## Installation Workflow

The complete installation process is documented in detail in the reference files:

1. **Build Verification** - Check Release binaries exist, offer to build if missing
2. **Product Name Extraction** - Extract PRODUCT_NAME from CMakeLists.txt
3. **Old Version Removal** - Remove existing installations to prevent conflicts
4. **Copy to System Folders** - Install VST3 and AU to macOS plugin directories
5. **Permissions Verification** - Set 755 permissions for DAW access
6. **Cache Clearing** - Clear Ableton Live and Logic Pro caches
7. **Verification** - Confirm installation with file checks and size validation
8. **PLUGINS.md Update** - Record installation status and locations

See **[references/installation-process.md](references/installation-process.md)** for complete implementation.

---

## Cache Management

DAWs cache plugin metadata for fast scanning. After installation/removal, caches must be cleared:

- **Ableton Live**: Delete `PluginCache.db` from all Live versions
- **Logic Pro**: Clear `AudioUnitCache/` and restart `AudioComponentRegistrar`
- **Other DAWs**: Reaper (no cache), Cubase (manual rescan), Pro Tools (future)

See **[references/cache-management.md](references/cache-management.md)** for detailed cache clearing procedures.

---

## Uninstallation Workflow

Complete uninstallation process:

1. **Locate Plugin Files** - Find installed VST3 and AU bundles
2. **Confirm Removal** - Ask user to confirm deletion
3. **Remove Files** - Delete from system folders (source code preserved)
4. **Clear Caches** - Same as installation
5. **Update PLUGINS.md** - Change status back to ✅ Working
6. **Confirmation** - Display uninstallation summary

See **[references/uninstallation-process.md](references/uninstallation-process.md)** for complete implementation.

---

## Reset to Ideation Workflow (Mode 3)

Surgical rollback that removes implementation but preserves ideation artifacts:

**What gets preserved:**
- Creative brief (the original idea)
- UI mockups (all versions)
- Parameter specifications

**What gets removed:**
- Source code (Source/ directory)
- Build configuration (CMakeLists.txt)
- Implementation docs (architecture.md, plan.md)
- Build artifacts and installed binaries

**Use case:** Implementation went wrong, but the concept and UI design are solid. Start fresh from Stage 0.

See **[references/mode-3-reset.md](references/mode-3-reset.md)** for complete implementation.

---

## Destroy Workflow (Mode 4)

Complete removal with backup for abandoned plugins:

**What gets removed:**
- Everything: source code, binaries, build artifacts, PLUGINS.md entry
- Optionally: troubleshooting docs mentioning the plugin

**Safety features:**
- Timestamped backup created before deletion
- Requires typing exact plugin name to confirm
- Blocks if status is 🚧 (protects in-progress work)

**Use case:** Abandoned experiment, complete failure, duplicate by mistake. Never using this plugin again.

See **[references/mode-4-destroy.md](references/mode-4-destroy.md)** for complete implementation.

---

## Interactive Menu (Mode: Menu)

When invoked via `/clean [PluginName]`, present interactive menu:

```
Plugin cleanup options for [PluginName]:

1. Uninstall - Remove binaries from system folders (keep source code)
2. Reset to ideation - Remove implementation, keep idea/mockups
3. Destroy - Complete removal with backup (IRREVERSIBLE except via backup)
4. Cancel

Choose (1-4): _
```

**Menu logic:**
- Read current plugin status from PLUGINS.md
- Show appropriate options based on status
- Route to selected mode
- Handle cancellation gracefully

---

## Error Handling

Common error scenarios with troubleshooting:

- **Build Files Not Found**: Guide to build Release mode or check build status
- **Permission Denied**: Create directories, fix permissions, check disk space
- **Cache Clear Failed**: Manual DAW-specific cache clearing instructions
- **Plugin Doesn't Appear in DAW**: Comprehensive troubleshooting checklist

See **[references/error-handling.md](references/error-handling.md)** for all error scenarios and fixes.

---

## Decision Menu After Installation

After successful installation:

```
✓ [PluginName] installed successfully

What's next?
1. Test in DAW (recommended) → Load in Logic/Ableton/Reaper
2. Create another plugin → /dream
3. Document this plugin → Create user guide/manual
4. Share plugin (export build) → Future: Package for distribution
5. Other

Choose (1-5): _
```

**Handle responses:**

- **Option 1:** Provide DAW testing guidance
- **Option 2:** Invoke `plugin-ideation` skill
- **Option 3:** Suggest creating user manual in `.ideas/`
- **Option 4:** Provide build export instructions (future feature)
- **Option 5:** Ask what they'd like to do

---

## Integration Points

**Invoked by:**

- `/install-plugin [PluginName]` → Mode 1 (Installation)
- `/uninstall [PluginName]` → Mode 2 (Uninstallation)
- `/reset-to-ideation [PluginName]` → Mode 3 (Reset)
- `/destroy [PluginName]` → Mode 4 (Destroy)
- `/clean [PluginName]` → Interactive menu
- `plugin-workflow` skill → After Stage 6 (offers installation)
- `plugin-improve` skill → After successful changes (offers reinstallation)
- Natural language: "Install [PluginName]", "Remove [PluginName]", "Clean up [PluginName]"

**Invokes:**

- None (terminal skill, doesn't invoke others)

**Updates:**

- `PLUGINS.md` → Status changes to 📦 Installed, adds installation metadata

**Creates:**

- System folder installations (non-git-tracked):
  - `~/Library/Audio/Plug-Ins/VST3/[Product].vst3`
  - `~/Library/Audio/Plug-Ins/Components/[Product].component`

**Blocks:**

- None (installation is optional, plugins can be tested without installing)

---

## Success Criteria

Installation is successful when:

- ✅ Both VST3 and AU files copied to system folders
- ✅ Permissions set correctly (755) on all files
- ✅ Caches cleared for all detected DAWs
- ✅ Verification shows recent timestamps (< 60 seconds ago)
- ✅ File sizes are reasonable (> 100 KB typically)
- ✅ PLUGINS.md updated with 📦 status and installation locations
- ✅ User knows next steps (restart DAW, rescan plugins)

**NOT required for success:**

- Plugin appearing in DAW immediately (may need rescan/restart)
- All DAW caches cleared (some DAWs may not be installed)
- Code signing (optional for local development)

---

## Notes for Claude

**When executing this skill:**

1. Always check for Release build first - offer to build if missing
2. Extract PRODUCT_NAME from CMakeLists.txt (may contain spaces)
3. Use quotes around paths with spaces: `"$PRODUCT_NAME"`
4. Remove old versions before installing (prevents conflicts)
5. Cache clearing is important but not blocking (warn if fails)
6. Verification checks should be comprehensive (permissions, timestamps, sizes)
7. PLUGINS.md status update is part of success criteria
8. Provide clear next steps after installation

**Common pitfalls:**

- Forgetting to quote paths with spaces (breaks with product names like "Tape Age")
- Not removing old versions (causes DAW confusion)
- Not clearing caches (DAW shows stale plugin metadata)
- Missing PLUGINS.md update (state tracking incomplete)
