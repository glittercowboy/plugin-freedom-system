# build-automation

---
skill: build-automation
description: Orchestrates plugin builds using centralized build script with comprehensive failure handling
category: core
invoked_by:
  - plugin-workflow (Stages 2-6)
  - plugin-improve (Phase 5)
  - plugin-lifecycle (via build verification)
invokes:
  - scripts/build-and-install.sh
  - troubleshooter agent (on failure, if user chooses "Investigate")
model: sonnet  # Default for orchestration
tools:
  - Bash
  - Read
  - Task
output:
  - Build success/failure status
  - Log file path
  - Decision menu on failure
---

## Purpose

The build-automation skill orchestrates plugin builds by invoking the centralized build script (`scripts/build-and-install.sh`) and providing comprehensive failure handling through a structured 4-option protocol. It bridges the gap between workflow stages and the build system, handling both success and failure paths gracefully.

## Integration Points

This skill is invoked by:
- **plugin-workflow**: After each stage completion (Stages 2-6) to build and verify changes
- **plugin-improve**: After implementation changes (Phase 5) to rebuild with modifications
- **plugin-lifecycle**: During installation verification to ensure plugins are buildable

This skill invokes:
- **scripts/build-and-install.sh**: Centralized 7-phase build pipeline
- **troubleshooter agent**: Deep research agent (via Task tool) when user chooses "Investigate" option

## Build Workflow

When invoked, the build-automation skill follows this workflow:

### 1. Input Validation

- Verify plugin name provided
- Check plugin directory exists: `test -d "plugins/$PLUGIN_NAME"`
- Validate CMakeLists.txt present: `test -f "plugins/$PLUGIN_NAME/CMakeLists.txt"`

### 2. Determine Build Flags

Context-aware flag selection:

- **Stage 2 (Foundation)**: Always use `--no-install` flag (verify compilation only, no installation)
- **Stages 3-6 (Shell/DSP/GUI/Validation)**: Full build with installation (no flags)
- **plugin-improve**: Full build with installation (no flags)
- **User manual invocation**: Ask if they want `--dry-run` to preview commands

### 3. Invoke Build Script

Execute build script with appropriate flags:

```bash
./scripts/build-and-install.sh [PluginName] [flags]
```

Display build progress in real-time using Bash tool.

### 4. Monitor Build Output

- Capture stdout and stderr
- Display progress messages to user
- Watch for error indicators
- Track build duration

### 5. Capture Exit Code

Check build script exit code:
- **Exit 0**: Build succeeded → proceed to Success Protocol
- **Exit 1**: Build failed → proceed to Failure Protocol

### 6. Log File Location

Always show log file path after build attempt:

```
Build log: logs/[PluginName]/build_TIMESTAMP.log
```

User can review full build output from log file if needed.

## Failure Protocol

When build fails (exit code 1), present this structured decision menu:

```
⚠️ Build failed

What would you like to do?
1. Investigate - Run troubleshooter agent to research the error (recommended)
2. Show me the build log - Display full build output for manual review
3. Show me the code - Open relevant source files where error occurred
4. Wait - I'll fix it manually and tell you to resume
5. Other

Choose (1-5): _
```

### Option 1: Investigate (Recommended)

**Purpose**: Automated diagnosis using troubleshooter agent

**Implementation**:
1. Extract error from build log (last 50 lines or first error indicator)
2. Parse error for key information:
   - Error message (exact text)
   - File/line reference (if available)
   - Error type (CMake config, compilation, linker, etc.)
3. Invoke troubleshooter agent via Task tool:
   ```markdown
   Invoke troubleshooter agent to research this build error:

   Error: [extracted error message]
   Context: Building plugin "[PluginName]" with CMake
   JUCE version: 8.0.9
   File: [file path:line number if available]

   Please investigate using the graduated depth protocol and provide a structured report.
   ```
4. Wait for troubleshooter report (structured markdown)
5. Display report to user with findings and recommended solution
6. Ask: "Should I apply this solution?" (Yes/No/Modify)
   - **Yes**: Apply solution, then ask "Retry build now?"
   - **No**: Return to failure menu
   - **Modify**: Accept user modifications, then ask "Retry build now?"

**IMPORTANT**: NEVER auto-retry without explicit user confirmation.

### Option 2: Show Build Log

**Purpose**: Display full build output for manual analysis

**Implementation**:
1. Read log file: `logs/[PluginName]/build_TIMESTAMP.log`
2. Display last 100 lines (or full log if < 100 lines)
3. Format for readability (preserve colors if possible)
4. After display, re-present decision menu (iterative debugging)

### Option 3: Show Code

**Purpose**: Display source files where error occurred

**Implementation**:
1. Parse build error for file/line reference
   - CMake errors: Extract from CMakeLists.txt references
   - Compilation errors: Extract from compiler output (e.g., "Source/PluginProcessor.cpp:42")
   - Linker errors: Show relevant CMakeLists.txt and header files
2. Read relevant source file using Read tool
3. Display file with context (±10 lines around error line if line number available)
4. If no file reference parseable: "Error location not found in build output. Try Option 2 (Show build log)."
5. After display, re-present decision menu (iterative debugging)

### Option 4: Wait (Manual Fix)

**Purpose**: User will fix issue manually outside workflow

**Implementation**:
1. Display exit message:
   ```
   Pausing workflow. Please fix the issue manually.

   When ready to retry the build, say:
   - "resume build" or "retry build" - Retry build with same flags
   - "continue workflow" - Skip build and continue to next stage (not recommended)

   Build log: logs/[PluginName]/build_TIMESTAMP.log
   ```
2. Exit skill, return control to invoking skill/workflow
3. Await user continuation command

### Option 5: Other

**Purpose**: Accept free-form user input for custom actions

**Implementation**:
1. Prompt: "What would you like to do?"
2. Accept free-text response
3. Interpret request and act accordingly:
   - Build-related: Execute custom build command
   - Code-related: Read/display requested files
   - Investigation: Perform custom research
   - Continue: Proceed as requested

## Success Protocol

When build succeeds (exit code 0), follow this workflow:

### 1. Verify Success

- Confirm build script exit code 0
- Check log contains "Phase 7: Verification" success message
- Extract installed plugin locations from script output

### 2. Display Success Message

Use this template:

```
✓ Build successful

Built and installed:
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component

Build time: [duration]
Log: logs/[PluginName]/build_TIMESTAMP.log
```

For `--no-install` builds (Stage 2):

```
✓ Build successful (compilation verified, not installed)

Built artifacts:
- VST3: plugins/[PluginName]/build/[PluginName]_artefacts/Release/VST3/[ProductName].vst3
- AU: plugins/[PluginName]/build/[PluginName]_artefacts/Release/AU/[ProductName].component

Build time: [duration]
Log: logs/[PluginName]/build_TIMESTAMP.log
```

### 3. Context-Aware Decision Menu

Present menu based on invoking context:

#### From Stage 2 (Foundation)

```
Foundation verified. What's next?
1. Continue to Stage 3 (Shell/Parameters)
2. Review generated code
3. Pause workflow
```

#### From Stage 3 (Shell)

```
Shell built successfully. What's next?
1. Continue to Stage 4 (DSP)
2. Test in DAW now
3. Review parameter code
4. Pause workflow
```

#### From Stage 4 (DSP)

```
DSP built successfully. What's next?
1. Run automated tests (recommended)
2. Continue to Stage 5 (GUI)
3. Test in DAW now
4. Pause workflow
```

#### From Stage 5 (GUI)

```
GUI built successfully. What's next?
1. Run automated tests (recommended)
2. Continue to Stage 6 (Validation)
3. Test in DAW now
4. Pause workflow
```

#### From Stage 6 (Validation)

```
Plugin complete! What's next?
1. Test in DAW (recommended)
2. View in standalone mode (/show-standalone)
3. Install to system (/install-plugin)
4. Make improvements (/improve)
```

#### From plugin-improve

```
Update built successfully. What's next?
1. Test changes in DAW
2. Continue improving
3. Done (commit changes)
```

### 4. Return to Invoking Skill

After user makes decision, return control to the skill that invoked build-automation:
- **plugin-workflow**: Returns to stage dispatcher to continue or pause
- **plugin-improve**: Returns to improvement workflow to continue or finalize
- **Manual invocation**: Exits skill, displays final status

**IMPORTANT**: Always return control to invoking skill after success. Don't continue workflow autonomously.

## Integration Examples

### Example 1: plugin-workflow Stage 2 (Foundation)

**Scenario**: foundation-agent completes, plugin-workflow needs to verify compilation

**Invocation**:
- Skill: plugin-workflow
- Stage: 2 (Foundation)
- Action: Invoke build-automation with `--no-install` flag

**Success Path**:
1. build-automation invokes: `./scripts/build-and-install.sh --no-install [PluginName]`
2. Build succeeds (compilation verified)
3. Displays success message with artifact locations
4. Presents Stage 2 completion menu
5. Returns to plugin-workflow with status: SUCCESS

**Failure Path**:
1. Build fails (likely CMake configuration issue)
2. build-automation presents 4-option failure menu
3. User chooses "Investigate"
4. troubleshooter diagnoses: "Missing juce_dsp module in CMakeLists.txt"
5. User confirms: "Apply solution"
6. CMakeLists.txt updated to include juce_dsp
7. User confirms: "Retry build"
8. Build succeeds
9. Returns to plugin-workflow with status: SUCCESS

### Example 2: plugin-workflow Stages 3-6 (Implementation Stages)

**Scenario**: dsp-agent completes Phase 4.2, plugin-workflow needs full build + install

**Invocation**:
- Skill: plugin-workflow
- Stage: 4 (DSP), Phase: 2
- Action: Invoke build-automation (no flags - full build)

**Success Path**:
1. build-automation invokes: `./scripts/build-and-install.sh [PluginName]`
2. Build script executes all 7 phases:
   - Phase 1: Pre-flight validation
   - Phase 2: Parallel build (VST3 + AU)
   - Phase 3: Extract PRODUCT_NAME
   - Phase 4: Remove old versions
   - Phase 5: Install new versions
   - Phase 6: Clear DAW caches
   - Phase 7: Verification
3. All phases succeed
4. Displays success message with installation paths
5. Presents Stage 4 completion menu: "Run automated tests / Continue to Stage 5 / Test in DAW / Pause"
6. Returns to plugin-workflow with status: SUCCESS
7. plugin-workflow auto-invokes plugin-testing skill (if user chose "Run tests")

**Failure Path**:
1. Build fails at Phase 2 (compilation error in DSP code)
2. Error: "plugins/TestPlugin/Source/PluginProcessor.cpp:87: error: 'gain' was not declared in this scope"
3. build-automation presents 4-option failure menu
4. User chooses "Show me the code"
5. Displays PluginProcessor.cpp lines 77-97 with error highlighted
6. User sees issue: used 'gain' instead of 'gainValue'
7. User says: "Wait" (will fix manually)
8. build-automation exits with: "When ready, say 'retry build'"
9. User fixes code manually
10. User says: "retry build"
11. plugin-workflow re-invokes build-automation
12. Build succeeds
13. Returns to plugin-workflow with status: SUCCESS

### Example 3: plugin-improve (Bug Fix or Feature Addition)

**Scenario**: plugin-improve applies bug fix to existing plugin, needs to rebuild

**Invocation**:
- Skill: plugin-improve
- Phase: 5 (Build & Test)
- Action: Invoke build-automation (no flags - full build)

**Success Path**:
1. build-automation invokes: `./scripts/build-and-install.sh [PluginName]`
2. Build and installation succeed
3. Displays success message
4. Presents improvement completion menu: "Test changes / Continue improving / Done"
5. Returns to plugin-improve with status: SUCCESS
6. plugin-improve continues to Phase 6 (CHANGELOG update)

**Failure Path**:
1. Build fails (regression from bug fix)
2. Error: "plugins/MyPlugin/Source/PluginProcessor.cpp:142: error: cannot convert 'int' to 'float' in assignment"
3. build-automation presents 4-option failure menu
4. User chooses "Investigate"
5. troubleshooter agent invoked:
   ```
   Error: cannot convert 'int' to 'float' in assignment
   Context: Building plugin "MyPlugin" after bug fix
   File: PluginProcessor.cpp:142
   ```
6. troubleshooter returns report:
   ```
   ## Research Report: Type Conversion Error

   ### Problem Identified
   - **Error:** cannot convert 'int' to 'float' in assignment
   - **Context:** Line 142 attempts to assign integer result to float parameter
   - **Root Cause:** Changed return type of helper function but didn't update assignment

   ### Research Path
   Level 0 (Quick Assessment)

   ### Confidence Assessment
   - **Confidence Level:** HIGH
   - **Reasoning:** Error message is explicit, solution straightforward

   ### Recommended Solution
   Cast result to float: `paramValue = static_cast<float>(calculateValue());`
   Or change calculateValue() return type to float.

   **Why This Works:** Explicit type conversion satisfies compiler type checker.
   ```
7. User reviews report: "Apply solution"
8. Code updated with cast
9. User confirms: "Retry build"
10. Build succeeds
11. Returns to plugin-improve with status: SUCCESS

## Error Handling Rules

### Never Auto-Retry

**CRITICAL**: build-automation must NEVER automatically retry a failed build without explicit user decision. Always present failure menu and await user choice.

Bad (DON'T DO):
```
Build failed. Retrying with different flags...
```

Good (DO THIS):
```
⚠️ Build failed

What would you like to do?
1. Investigate...
```

### Preserve Context Between Retries

When user says "retry build" after manual fixes:
1. Use same flags as original build attempt
2. Preserve same invoking context (Stage 2, plugin-improve, etc.)
3. Return to same decision point after success/failure

### Handle Missing Dependencies

If build script fails with dependency errors (CMake, Ninja, JUCE not found):
1. Display specific missing dependency
2. Provide installation command (e.g., "Install with: brew install ninja")
3. After user installs, offer: "Retry build now?"

### Parse Errors Intelligently

Extract meaningful error information for troubleshooter:
- **CMake errors**: Configuration issues, missing modules, path problems
- **Compilation errors**: Syntax, type mismatches, missing declarations
- **Linker errors**: Missing symbols, library issues
- **Installation errors**: Permission issues, path conflicts

Pass full context to troubleshooter, not just error message.

## Performance Considerations

### Build Duration Tracking

Always display build duration in success/failure messages:
- Start timer before invoking build script
- Stop timer after exit
- Format as "Build time: 2m 34s" or "Build time: 45s"

### Log File Management

- Build logs accumulate in `logs/[PluginName]/`
- Each build creates timestamped log: `build_20250110_143022.log`
- No automatic cleanup (user can manually delete old logs)
- Always reference most recent log file

### Parallel Builds

Build script handles parallelization (Ninja builds VST3 + AU simultaneously). build-automation skill doesn't need to manage this - just invoke script and monitor output.

## Testing & Debugging

### Manual Invocation

Skill can be invoked manually for testing:

```
Please invoke the build-automation skill to build RadioMusic plugin.

Context: Manual test of build system.
Expected: Full build and installation.
```

### Simulated Failures

To test failure protocol:
1. Introduce intentional syntax error in plugin source
2. Invoke build-automation
3. Verify 4-option menu appears
4. Test each option (Investigate, Show log, Show code, Wait)
5. Verify troubleshooter integration works
6. Fix error and retry
7. Verify success path works

### Integration Testing

To test workflow integration:
1. Start new plugin workflow: `/implement TestPlugin`
2. Progress to Stage 2 (Foundation)
3. Verify build-automation invoked with `--no-install`
4. Verify success returns to workflow
5. Progress to Stage 3+ and verify full build invoked
6. Introduce failure and verify handling
7. Verify workflow resumes after fix

## Common Issues

### Issue: "Build script not found"

**Cause**: build-automation invoked before Phase 4a complete

**Solution**: Verify `scripts/build-and-install.sh` exists and is executable

### Issue: "Log file not created"

**Cause**: `logs/` directory doesn't exist or permissions issue

**Solution**: Build script should create `logs/` directory in Phase 1

### Issue: "troubleshooter returns JSON instead of markdown"

**Cause**: Agent using wrong report format

**Solution**: troubleshooter YAML frontmatter must specify: `output_format: structured_markdown_report`

### Issue: "Build succeeds but plugins not in DAW"

**Cause**: Cache clearing didn't work or DAW already running

**Solution**:
1. Verify Phase 6 executed (cache clearing)
2. Restart DAW completely (not just rescan)
3. Check installation paths manually

### Issue: "Retry build uses wrong flags"

**Cause**: Not preserving context between attempts

**Solution**: Store original invocation context (stage, flags) at beginning of skill execution, reuse on retry

## Future Enhancements

Potential additions for later phases:

1. **Build caching**: Incremental builds for faster iteration
2. **Parallel plugin builds**: Build multiple plugins simultaneously
3. **Build profiles**: Debug, Release, RelWithDebInfo
4. **Build notifications**: macOS notifications for long builds
5. **Build analytics**: Track success rates, common failures, build times
6. **Custom build targets**: Support non-standard JUCE targets
7. **Windows/Linux support**: Extend build system beyond macOS

---

**Skill Status**: Ready for integration
**Last Updated**: 2025-01-10
**Dependencies**: scripts/build-and-install.sh, .claude/agents/troubleshooter.md
