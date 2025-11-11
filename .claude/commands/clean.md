---
name: clean
description: Interactive plugin cleanup menu - uninstall, reset, or destroy
args: "[PluginName]"
---

# /clean - Plugin Cleanup Menu

**Purpose:** Present interactive menu for plugin cleanup operations (uninstall, reset to ideation, or complete destruction).

## Behavior

When user runs `/clean [PluginName]`, invoke the plugin-lifecycle skill with mode: 'menu' to present cleanup options.

## Interactive Menu

```
Plugin cleanup options for [PluginName]:

Current status: [Status from PLUGINS.md]

1. Uninstall - Remove binaries from system folders (keep source code)
   → Removes: VST3, AU binaries
   → Keeps: Source code, contracts, mockups
   → Status change: 📦 Installed → ✅ Working

2. Reset to ideation - Remove implementation, keep idea/mockups
   → Removes: Source/, CMakeLists.txt, implementation docs
   → Keeps: creative-brief.md, mockups/, parameter-spec.md
   → Status change: [Any] → 💡 Ideated
   → Backup: Created in backups/rollbacks/

3. Destroy - Complete removal with backup (IRREVERSIBLE except via backup)
   → Removes: Everything (source, binaries, PLUGINS.md entry)
   → Backup: Created in backups/destroyed/
   → Requires: Typing plugin name to confirm

4. Cancel

Choose (1-4): _
```

## Menu Logic

The skill determines which options to show based on current plugin status:

- **If 📦 Installed:** Show all options (1, 2, 3, 4)
- **If ✅ Working:** Show options 2, 3, 4 (skip uninstall)
- **If 💡 Ideated:** Show only option 3 (destroy), options 1-2 N/A
- **If 🚧 Stage N:** Block menu, show message:
  ```
  Cannot clean plugin in development (Status: 🚧 Stage N)

  Complete the workflow first:
  - /continue [PluginName] - Resume and finish
  - Or manually change status in PLUGINS.md if workflow abandoned
  ```

## Routing

Based on user selection, invoke plugin-lifecycle skill with specific mode:

- **Option 1:** Mode 2 (Uninstallation) → See uninstallation-process.md
- **Option 2:** Mode 3 (Reset) → See mode-3-reset.md
- **Option 3:** Mode 4 (Destroy) → See mode-4-destroy.md
- **Option 4:** Cancel → Exit gracefully

## Example Usage

```bash
# Present menu
/clean TapeAge

# User sees status and options
# Chooses option 2 (Reset to ideation)
# Skill routes to mode-3-reset.md implementation
# Implementation removed, idea/mockups preserved
```

## Routes To

`plugin-lifecycle` skill (mode: 'menu')

## Related Commands

- `/uninstall [Name]` - Direct shortcut to Mode 2
- `/reset-to-ideation [Name]` - Direct shortcut to Mode 3
- `/destroy [Name]` - Direct shortcut to Mode 4

This command provides the menu, while individual commands bypass the menu for power users.
