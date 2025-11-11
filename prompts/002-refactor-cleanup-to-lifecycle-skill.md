<objective>
Refactor the plugin cleanup operations (destroy and reset-to-ideation) from standalone commands into the plugin-lifecycle skill, following the established pattern where commands are thin routers and skills contain the implementation logic. Create a unified /clean command that presents an interactive menu, similar to how /dream routes to plugin-ideation skill.
</objective>

<context>
This is part of the Plugin Freedom System, a JUCE plugin development framework.

Current state:
- Cleanup operations exist as separate commands (/destroy and /reset-to-ideation) with all logic embedded in command files
- Reference files already exist (.claude/skills/plugin-lifecycle/references/mode-3-reset.md and mode-4-destroy.md) that are copies of the command files
- These need to be properly integrated into the skill dispatcher pattern

Desired state:
- plugin-lifecycle skill acts as dispatcher/router for all lifecycle operations
- Reference files contain implementation details (following plugin-workflow pattern)
- /clean command presents interactive menu
- Individual commands remain as shortcuts

Pattern to follow: Commands are thin routers (like /dream, /implement, /improve) that invoke skills which contain the actual logic and decision trees. Skills have a main SKILL.md (dispatcher) and references/ directory with detailed implementation for each operation.

@.claude/commands/destroy.md - Current destroy command to be refactored
@.claude/commands/reset-to-ideation.md - Current reset command to be refactored
@.claude/skills/plugin-lifecycle/SKILL.md - Existing lifecycle skill to extend
@.claude/skills/plugin-lifecycle/references/mode-3-reset.md - Already exists, needs integration
@.claude/skills/plugin-lifecycle/references/mode-4-destroy.md - Already exists, needs integration
@.claude/skills/plugin-lifecycle/references/uninstallation-process.md - Existing uninstall reference
@.claude/skills/plugin-workflow/SKILL.md - Reference for dispatcher pattern
</context>

<requirements>
1. Integrate existing mode-3-reset.md and mode-4-destroy.md into the skill dispatcher pattern
2. Update `.claude/skills/plugin-lifecycle/SKILL.md` to add cleanup operations section with mode dispatcher
3. Rename reference files to match existing naming convention:
   - mode-3-reset.md → stays as-is (or rename to reset-to-ideation-process.md to match installation-process.md pattern)
   - mode-4-destroy.md → stays as-is (or rename to destroy-process.md to match pattern)
4. Create mode dispatcher in SKILL.md that routes to:
   - Mode 1: Installation (already exists → installation-process.md)
   - Mode 2: Uninstallation (already exists → uninstallation-process.md)
   - Mode 3: Reset to ideation (→ mode-3-reset.md)
   - Mode 4: Destroy (→ mode-4-destroy.md)
5. Create new `/clean` command that presents interactive menu and invokes skill with appropriate mode
6. Update individual commands (/destroy, /reset-to-ideation, /uninstall) to be thin routers that invoke skill with specific mode
7. Preserve all safety features (confirmations, backups, status checks)
8. Maintain the same user experience, just with better code organization
9. Follow the pattern of plugin-workflow where SKILL.md is the dispatcher and references/ contain implementation
</requirements>

<implementation>
The skill should handle the menu presentation and route to the appropriate operation based on user choice.
Each operation reference file should contain the complete logic currently in the command files.
The /clean command should be a simple router that invokes the plugin-lifecycle skill with mode: 'cleanup'.
Individual shortcut commands should invoke the skill with specific modes (mode: 'destroy', mode: 'reset', mode: 'uninstall').
Use the same decision menu pattern as plugin-ideation skill uses for its interactive flows.
</implementation>

<files_to_create>
1. `.claude/commands/clean.md` - New unified entry point command with interactive menu
</files_to_create>

<files_to_modify>
1. `.claude/skills/plugin-lifecycle/SKILL.md` - Add mode dispatcher section for all lifecycle operations (install, uninstall, reset, destroy)
2. `.claude/skills/plugin-lifecycle/references/mode-3-reset.md` - Verify content matches latest logic, update if needed
3. `.claude/skills/plugin-lifecycle/references/mode-4-destroy.md` - Verify content matches latest logic, update if needed
4. `.claude/commands/destroy.md` - Simplify to thin router that invokes skill with mode: 'destroy'
5. `.claude/commands/reset-to-ideation.md` - Simplify to thin router that invokes skill with mode: 'reset'
6. `.claude/commands/uninstall.md` - Update to invoke skill with mode: 'uninstall' (may already be correct)
</files_to_modify>

<optional_refactoring>
Consider renaming mode-3-reset.md and mode-4-destroy.md to match the pattern of other references (installation-process.md, uninstallation-process.md) for consistency, but this is optional if the mode-N naming is preferred.
</optional_refactoring>

<output>
Create and modify files with the refactored structure:
- Move all implementation logic to skill reference files
- Update SKILL.md with cleanup operations dispatcher
- Create /clean as primary command with menu
- Simplify existing commands to be shortcuts
- Preserve all safety features and user flows
</output>

<success_criteria>
- All cleanup operations work exactly as before from user perspective
- /clean command presents clear interactive menu
- Individual commands still work as shortcuts
- Logic is properly encapsulated in skill, not commands
- Follows established pattern of other skills/commands in the system
- All safety features (confirmations, backups) remain intact
</success_criteria>

<verification>
After refactoring:
1. Test /clean command shows menu with all options
2. Test /destroy still works as direct shortcut
3. Test /reset-to-ideation still works as direct shortcut
4. Verify backups are still created in correct locations
5. Verify all confirmation prompts still appear
6. Check that status checks (blocking on 🚧) still work
</verification>