<objective>
Fix outdated stub references in the plugin workflow system that are preventing proper execution.
The router is trying to call non-existent stub functions while the actual implementations exist and are documented.
This is blocking the workflow from progressing past Stage 1.
</objective>

<context>
The Plugin Freedom System has fully implemented subagents for Stages 2-5, but the workflow dispatcher still references stub functions that don't exist.
This is a critical bug preventing `/implement` and `/continue` commands from working.

Analysis already completed shows:
- Router calls executeStage2FoundationStub() etc. which don't exist
- Actual implementations use Task tool to invoke real subagents
- Implementation status incorrectly shows "Phase 2: Workflow Engine (next)"
</context>

<requirements>
1. Fix the stage dispatcher switch statement to call actual implementations
2. Update implementation status to reflect completed workflow engine
3. Correct terminology inconsistency (Phases vs Stages)
4. Ensure all changes maintain consistency with existing patterns
</requirements>

<fixes_needed>

## 1. Router Logic Fix
Location: `.claude/skills/plugin-workflow/SKILL.md` lines 177-183

Current (broken):
```javascript
case 2:
  return executeStage2FoundationStub(pluginName) // Phase 2b: stub
case 3:
  return executeStage3ShellStub(pluginName) // Phase 2b: stub
case 4:
  return executeStage4DSPStub(pluginName) // Phase 2b: stub
case 5:
  return executeStage5GUIStub(pluginName) // Phase 2b: stub
```

Should be:
```javascript
case 2:
  return executeStage2Foundation(pluginName)
case 3:
  return executeStage3Shell(pluginName)
case 4:
  return executeStage4DSP(pluginName)
case 5:
  return executeStage5GUI(pluginName)
```

## 2. Implementation Status Update
Location: `.claude/CLAUDE.md` line 30

Current:
```markdown
- Phase 2: Workflow Engine (next)
```

Should be:
```markdown
- ✓ Phase 2: Workflow Engine (complete)
```

## 3. Terminology Fix
Location: `.claude/skills/plugin-workflow/SKILL.md` line 5

Current:
```yaml
- Task # For subagents (Phases 2-5)
```

Should be:
```yaml
- Task # For subagents (Stages 2-5)
```
</fixes_needed>

<implementation>
1. Use the Edit tool to update each location precisely
2. Preserve all surrounding code and comments
3. Ensure function names match the documented implementations
4. The actual stage implementations are already documented in the same file (lines 829+)
</implementation>

<verification>
After making changes, verify:
1. Check that the dispatcher cases match function naming convention
2. Confirm no "stub" references remain in active code paths
3. Verify implementation status accurately reflects system state
4. Run `grep -n "Stub\|stub" .claude/skills/plugin-workflow/SKILL.md` to ensure only acceptable stub references remain
</verification>

<success_criteria>
- Workflow can progress from Stage 1 to Stage 2 without errors
- Router calls real implementations, not non-existent stubs
- Documentation accurately reflects implementation status
- Terminology is consistent throughout (Stages for workflow progression)
</success_criteria>