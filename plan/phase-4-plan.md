# Phase 4 Implementation Plan

## Prerequisites Status

**Previous Phase(s) Required:** Phase 0, Phase 1, Phase 2, Phase 3

**Deliverables Check:**

### Phase 0 (Foundation & Contracts)
- ✅ `.claude/` directory structure - EXISTS
- ✅ `.claude/CLAUDE.md` navigation index - EXISTS
- ✅ `.claude/commands/` directory - EXISTS
- ✅ `.claude/skills/` directory - EXISTS
- ✅ `.claude/agents/` directory - EXISTS
- ✅ `PLUGINS.md` registry - EXISTS
- ✅ Contract templates in skill assets - EXISTS

### Phase 1 (Discovery System)
- ✅ All 9 slash commands - EXISTS
- ✅ All 7 core skills - EXISTS
- ✅ Interactive decision system - EXISTS
- ✅ Handoff file system - EXISTS

### Phase 2 (Workflow Engine)
- ✅ `.claude/skills/plugin-workflow/SKILL.md` - EXISTS
- ✅ Stage 0, 1, 6 implementations - EXISTS
- ✅ State management integration - EXISTS
- ✅ Git commit workflow - EXISTS

### Phase 3 (Implementation Subagents)
- ✅ `.claude/agents/foundation-agent.md` - EXISTS
- ✅ `.claude/agents/shell-agent.md` - EXISTS
- ✅ `.claude/agents/dsp-agent.md` - EXISTS
- ✅ `.claude/agents/gui-agent.md` - EXISTS
- ✅ `.claude/agents/validator.md` - EXISTS
- ✅ Dispatcher integration in plugin-workflow - EXISTS
- ✅ Test plugins (RadioMusic, TapeAge) - EXISTS

### Phase 4 (Build & Troubleshooting)
- ❌ `scripts/build-and-install.sh` - MISSING
- ❌ `.claude/skills/build-automation/` - MISSING
- ❌ `.claude/agents/troubleshooter.md` - MISSING

**Status:** Partial

---

## ⚠️ PREREQUISITE WARNING ⚠️

**Prerequisites PARTIAL. Phase 4 core deliverables are missing.**

**Missing deliverables:**
- `scripts/build-and-install.sh` - Centralized build script
- `.claude/skills/build-automation/` - Build orchestration skill
- `.claude/agents/troubleshooter.md` - Diagnostic research agent

**This plan assumes you want to implement Phase 4 now. All Phase 0-3 prerequisites are complete.**

---

## Phase Overview

Phase 4 implements the **Build & Troubleshooting System** - the critical infrastructure that transforms working code into installed, testable plugins. This phase provides:

1. **Centralized build automation** - Single script handles CMake, Ninja, parallel builds, installation, cache clearing
2. **Diagnostic capabilities** - Multi-level troubleshooting agent researches build failures and provides solutions
3. **Failure recovery** - Structured 4-option protocol handles build failures gracefully
4. **Production readiness** - Plugins can be built, installed, and tested in DAWs

**Why This Phase Is Critical:**
- Without build automation, every stage manually invokes CMake/Ninja (error-prone, inconsistent)
- Without troubleshooter, build failures are dead ends requiring manual research
- Without cache clearing, DAWs test stale versions (false negatives)
- Without verification, silent installation failures go undetected

This phase completes the "code → working plugin" pipeline, enabling full end-to-end development.

---

## Required Documentation

### Architecture Files (MUST READ IN ENTIRETY)

- **@architecture/11-build-automation-architecture.md** - Complete specification of build script phases, flags, logging, verification
- **@architecture/02-core-abstractions.md** (lines 74-109) - Subagent abstraction pattern for troubleshooter
- **@architecture/03-model-selection-extended-thinking-strategy.md** - Model selection for troubleshooter (Opus for deep research)
- **@architecture/07-communication-architecture.md** - Subagent report format and parsing
- **@architecture/13-error-handling-recovery.md** (lines 182-461) - Build failure taxonomy, recovery patterns, performance budgets
- **@architecture/04-component-architecture.md** - Integration points between skills and scripts

### Procedure Files (MUST READ IN ENTIRETY)

- **@procedures/scripts/build-and-install.md** - Exact implementation specification for build script
- **@procedures/skills/build-automation.md** - Build skill workflow, 4-option failure protocol, retry logic
- **@procedures/agents/troubleshooter.md** - Graduated depth protocol, research sources, report format
- **@procedures/skills/juce-foundation.md** - JUCE API patterns for build system verification
- **@procedures/core/interactive-decision-system.md** - Decision menu patterns for failure protocol
- **@procedures/webview/02-project-setup.md** - WebView-specific build configuration (CMake binary data)

---

## Sub-Phase Breakdown

Phase 4 is split into **3 sub-phases** for logical implementation order:

- **Phase 4a:** Build Script (`build-and-install.sh`) - Foundation infrastructure
- **Phase 4b:** troubleshooter Agent - Diagnostic capability
- **Phase 4c:** build-automation Skill - Orchestration layer with failure protocol

**Rationale:**
1. Build script is standalone (can be tested independently)
2. troubleshooter depends on nothing (research-only agent)
3. build-automation orchestrates both (final integration)

This allows testing each component in isolation before integration.

---

## Implementation Sequence

### Phase 4a: Build Script Implementation

#### Task 1: Create scripts/ Directory Structure

**Description**: Establish scripts directory with proper structure for build automation.

**Required Reading**:
- @architecture/09-file-system-design.md (sections: all)
- @architecture/11-build-automation-architecture.md (sections: Directory Structure)

**Dependencies**: None

**Implementation Steps**:
1. Create `scripts/` directory at repository root
2. Create `logs/` directory at repository root (build logs destination)
3. Update `.gitignore` to exclude `logs/` directory

**Expected Output**:
- `scripts/` directory exists
- `logs/` directory exists
- `.gitignore` contains `logs/`

**Verification**:
- Automated: `test -d scripts && test -d logs && grep -q "^logs/" .gitignore && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please confirm that `ls -la` shows `scripts/` and `logs/` directories at the root."

---

#### Task 2: Implement Build Script Core Structure

**Description**: Create `build-and-install.sh` with 7-phase pipeline skeleton and argument parsing.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: all)
- @architecture/11-build-automation-architecture.md (sections: 7-Phase Pipeline, Flags, Logging)

**Dependencies**: Task 1 complete

**Implementation Steps**:
1. Create `scripts/build-and-install.sh` file
2. Add shebang (`#!/bin/bash`) and `set -e` (exit on error)
3. Implement argument parsing (plugin name, `--dry-run`, `--no-install`, `--verbose`)
4. Implement 7 phase functions (stubs with echo statements):
   - `phase_1_preflight_validation()`
   - `phase_2_build()`
   - `phase_3_extract_product_name()`
   - `phase_4_remove_old_versions()`
   - `phase_5_install_new_versions()`
   - `phase_6_clear_daw_caches()`
   - `phase_7_verification()`
5. Implement main function that calls phases sequentially
6. Add color-coded output functions (`success()`, `warning()`, `error()`)
7. Add logging setup (create `logs/$PLUGIN_NAME/build_$(date +%Y%m%d_%H%M%S).log`)
8. Make script executable: `chmod +x scripts/build-and-install.sh`

**Expected Output**:
- `scripts/build-and-install.sh` exists and is executable
- Script accepts arguments: `[PluginName] [--dry-run] [--no-install] [--verbose]`
- All 7 phase functions defined
- Color output functions defined
- Logging infrastructure in place

**Verification**:
- Automated: `test -x scripts/build-and-install.sh && echo "PASS" || echo "FAIL"`
- Automated: `./scripts/build-and-install.sh 2>&1 | grep -q "Usage:" && echo "PASS" || echo "FAIL"` (should show usage without args)
- Manual: STOP AND ASK LEX: "Please run `./scripts/build-and-install.sh --dry-run TestPlugin` and confirm it shows 7 phase names with stub output."

---

#### Task 3: Implement Phase 1 - Pre-flight Validation

**Description**: Validate environment and plugin directory before build.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Phase 1)
- @architecture/13-error-handling-recovery.md (sections: Build Failures)

**Dependencies**: Task 2 complete

**Implementation Steps**:
1. Check plugin directory exists: `test -d "plugins/$PLUGIN_NAME"`
2. Check CMakeLists.txt exists: `test -f "plugins/$PLUGIN_NAME/CMakeLists.txt"`
3. Check PRODUCT_NAME in CMakeLists.txt: `grep -q "PRODUCT_NAME" "plugins/$PLUGIN_NAME/CMakeLists.txt"`
4. Check JUCE_DIR environment variable or JUCE installed
5. Check CMake available: `command -v cmake`
6. Check Ninja available: `command -v ninja`
7. If any check fails, call `error()` function and exit with code 1
8. If all pass, call `success()` function with confirmation message

**Expected Output**:
- Script validates all prerequisites before proceeding
- Clear error messages if prerequisites missing
- Exit code 1 on failure, continues on success

**Verification**:
- Automated: `./scripts/build-and-install.sh --dry-run NonExistentPlugin 2>&1 | grep -q "Error" && echo "PASS" || echo "FAIL"`
- Automated: `./scripts/build-and-install.sh --dry-run RadioMusic 2>&1 | grep -q "Pre-flight validation passed" && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please run `./scripts/build-and-install.sh --dry-run RadioMusic` and confirm it shows green success message for pre-flight validation."

---

#### Task 4: Implement Phase 2 - Parallel Build

**Description**: Build VST3 and AU formats in parallel using Ninja.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Phase 2)
- @architecture/11-build-automation-architecture.md (sections: Build Process, Performance)
- @procedures/skills/juce-foundation.md (sections: JUCE Build System)

**Dependencies**: Task 3 complete

**Implementation Steps**:
1. Create or validate build directory: `mkdir -p "plugins/$PLUGIN_NAME/build"`
2. Run CMake configuration with Ninja generator:
   ```bash
   cmake -S "plugins/$PLUGIN_NAME" \
         -B "plugins/$PLUGIN_NAME/build" \
         -G Ninja \
         -DCMAKE_BUILD_TYPE=Release
   ```
3. Build both formats in parallel:
   ```bash
   cmake --build "plugins/$PLUGIN_NAME/build" \
         --config Release \
         --target "${PLUGIN_NAME}_VST3" "${PLUGIN_NAME}_AU" \
         --parallel
   ```
4. If `--no-install` flag: exit after build with success message
5. Capture build output to log file (tee to both console and log)
6. Check exit code, call `error()` and exit if build fails

**Expected Output**:
- Build directory created at `plugins/[PluginName]/build/`
- VST3 and AU targets built in parallel
- Build output logged to `logs/[PluginName]/build_TIMESTAMP.log`
- Build artifacts exist at expected paths
- `--no-install` flag exits early with success

**Verification**:
- Automated: `./scripts/build-and-install.sh --no-install --dry-run RadioMusic && echo "PASS" || echo "FAIL"` (dry-run should show commands)
- Manual: STOP AND ASK LEX: "Please run `./scripts/build-and-install.sh --no-install RadioMusic` and confirm it builds successfully and exits WITHOUT installing. Check that `logs/RadioMusic/` contains a build log."

---

#### Task 5: Implement Phase 3 - Extract PRODUCT_NAME

**Description**: Extract display name from CMakeLists.txt for installation filenames.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Phase 3)
- @architecture/11-build-automation-architecture.md (sections: Product Name Handling)

**Dependencies**: Task 4 complete

**Implementation Steps**:
1. Parse PRODUCT_NAME from CMakeLists.txt:
   ```bash
   PRODUCT_NAME=$(grep 'PRODUCT_NAME' "plugins/$PLUGIN_NAME/CMakeLists.txt" | \
                  sed -E 's/.*PRODUCT_NAME[[:space:]]+"([^"]+)".*/\1/')
   ```
2. Validate extraction succeeded (PRODUCT_NAME not empty)
3. Handle spaces in product name (quote all subsequent uses)
4. Log extracted product name: `echo "Product name: $PRODUCT_NAME"`
5. If extraction fails, use directory name as fallback with warning

**Expected Output**:
- `PRODUCT_NAME` variable contains extracted value
- Works with names containing spaces (e.g., "My Plugin Name")
- Fallback to directory name if extraction fails
- Clear log message showing extracted name

**Verification**:
- Automated: `grep "PRODUCT_NAME" plugins/RadioMusic/CMakeLists.txt && echo "Can verify manually" || echo "SKIP - no PRODUCT_NAME"`
- Manual: STOP AND ASK LEX: "Please check if `plugins/RadioMusic/CMakeLists.txt` contains a PRODUCT_NAME line. If yes, run the build script with `--dry-run` and confirm it correctly extracts and displays the product name."

---

#### Task 6: Implement Phase 4 - Remove Old Versions

**Description**: Find and remove old plugin versions from system folders before installation.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Phase 4)
- @procedures/skills/plugin-lifecycle.md (sections: Installation Process, System Folders)

**Dependencies**: Task 5 complete

**Implementation Steps**:
1. Define system folder paths:
   ```bash
   VST3_DIR="$HOME/Library/Audio/Plug-Ins/VST3"
   AU_DIR="$HOME/Library/Audio/Plug-Ins/Components"
   ```
2. Search for existing VST3: `find "$VST3_DIR" -name "$PRODUCT_NAME.vst3" -type d`
3. Search for existing AU: `find "$AU_DIR" -name "$PRODUCT_NAME.component" -type d`
4. If found, remove with confirmation: `rm -rf [path]`
5. If `--dry-run`, show what would be removed without removing
6. Log all removals (or dry-run simulations)
7. Handle names with spaces correctly (quote paths)

**Expected Output**:
- Old plugin versions removed from system folders
- Clear log messages showing what was removed
- `--dry-run` shows what would be removed without actually removing
- Handles edge cases (no old versions, multiple versions)

**Verification**:
- Automated: N/A (requires existing installations)
- Manual: STOP AND ASK LEX: "Please run `./scripts/build-and-install.sh --dry-run RadioMusic` and confirm Phase 4 shows whether old versions exist and what would be removed."

---

#### Task 7: Implement Phase 5 - Install New Versions

**Description**: Copy built plugins to system folders with correct permissions.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Phase 5)
- @procedures/skills/plugin-lifecycle.md (sections: Installation Process, Permissions)

**Dependencies**: Task 6 complete

**Implementation Steps**:
1. Locate build artifacts:
   ```bash
   VST3_BUILD="plugins/$PLUGIN_NAME/build/[PluginName]_artefacts/Release/VST3/$PRODUCT_NAME.vst3"
   AU_BUILD="plugins/$PLUGIN_NAME/build/[PluginName]_artefacts/Release/AU/$PRODUCT_NAME.component"
   ```
2. Verify artifacts exist before copying
3. Copy VST3 to system folder: `cp -R "$VST3_BUILD" "$VST3_DIR/"`
4. Copy AU to system folder: `cp -R "$AU_BUILD" "$AU_DIR/"`
5. Set permissions: `chmod -R 755 "$VST3_DIR/$PRODUCT_NAME.vst3" "$AU_DIR/$PRODUCT_NAME.component"`
6. If `--dry-run`, show copy commands without executing
7. Log all installations with source and destination paths

**Expected Output**:
- VST3 copied to `~/Library/Audio/Plug-Ins/VST3/`
- AU copied to `~/Library/Audio/Plug-Ins/Components/`
- Permissions set to 755 (executable)
- Clear log messages showing installation paths

**Verification**:
- Automated: Test file existence after install (will implement in Task 10)
- Manual: STOP AND ASK LEX: "After running `./scripts/build-and-install.sh RadioMusic`, please check that `ls ~/Library/Audio/Plug-Ins/VST3/` and `ls ~/Library/Audio/Plug-Ins/Components/` show the installed plugin files."

---

#### Task 8: Implement Phase 6 - Clear DAW Caches

**Description**: Clear Ableton, Logic, and system audio unit caches to force plugin rescan.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Phase 6)
- @architecture/11-build-automation-architecture.md (sections: Cache Clearing)
- @procedures/skills/plugin-lifecycle.md (sections: Cache Clearing)

**Dependencies**: Task 7 complete

**Implementation Steps**:
1. Clear Ableton plugin scanner cache: `rm -rf ~/Library/Caches/Ableton/Plugin\ Scanner/*`
2. Clear system Audio Unit cache: `rm -rf ~/Library/Caches/AudioUnitCache/*`
3. Kill AudioComponentRegistrar process (macOS system process):
   ```bash
   killall -9 AudioComponentRegistrar 2>/dev/null || true
   ```
4. If `--dry-run`, show cache paths that would be cleared
5. Log all cache clearing operations
6. Handle missing cache directories gracefully (may not exist)

**Expected Output**:
- Ableton cache cleared
- Audio Unit cache cleared
- AudioComponentRegistrar process killed and will restart
- DAWs will rescan plugins on next launch
- No errors if cache directories don't exist

**Verification**:
- Automated: `./scripts/build-and-install.sh --dry-run RadioMusic 2>&1 | grep -q "cache" && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "After running the build script, please check that the cache directories `~/Library/Caches/Ableton/Plugin Scanner/` and `~/Library/Caches/AudioUnitCache/` are empty or recreated."

---

#### Task 9: Implement Phase 7 - Verification

**Description**: Verify successful installation with timestamp checks and file existence.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Phase 7)
- @architecture/11-build-automation-architecture.md (sections: Verification, Timestamp Checks)

**Dependencies**: Task 8 complete

**Implementation Steps**:
1. Check VST3 exists: `test -d "$VST3_DIR/$PRODUCT_NAME.vst3"`
2. Check AU exists: `test -d "$AU_DIR/$PRODUCT_NAME.component"`
3. Check VST3 timestamp (<60 seconds old):
   ```bash
   find "$VST3_DIR/$PRODUCT_NAME.vst3" -mtime -1m
   ```
4. Check AU timestamp (<60 seconds old)
5. Get file sizes for log: `du -sh [path]`
6. If any check fails, call `error()` with specific failure message
7. If all pass, call `success()` with summary (paths, sizes, timestamps)
8. Exit with code 0 on success, code 1 on failure

**Expected Output**:
- VST3 and AU files verified to exist
- Timestamps confirmed fresh (<60 seconds)
- File sizes logged
- Clear success or failure message
- Appropriate exit codes

**Verification**:
- Automated: Will be verified by full script test in Task 10
- Manual: STOP AND ASK LEX: "After running `./scripts/build-and-install.sh RadioMusic`, please confirm that the final output shows green success message with file paths, sizes, and recent timestamps."

---

#### Task 10: Test Complete Build Script

**Description**: End-to-end testing of build script with actual plugin.

**Required Reading**:
- @procedures/scripts/build-and-install.md (sections: Testing)
- @architecture/11-build-automation-architecture.md (sections: all)

**Dependencies**: Tasks 1-9 complete

**Implementation Steps**:
1. Test with `--dry-run` flag on RadioMusic:
   ```bash
   ./scripts/build-and-install.sh --dry-run RadioMusic
   ```
2. Verify dry-run shows all 7 phases without executing
3. Test with `--no-install` flag on RadioMusic:
   ```bash
   ./scripts/build-and-install.sh --no-install RadioMusic
   ```
4. Verify builds successfully and exits without installing
5. Test full build and install on RadioMusic:
   ```bash
   ./scripts/build-and-install.sh RadioMusic
   ```
6. Verify all 7 phases execute successfully
7. Check log file created in `logs/RadioMusic/`
8. Verify plugins installed to system folders
9. Test with non-existent plugin to verify error handling
10. Test with plugin that has spaces in PRODUCT_NAME (if available)

**Expected Output**:
- All 7 phases execute in order
- Logs written to correct location
- Plugins built, installed, and verified
- Error handling works correctly
- Edge cases handled gracefully

**Verification**:
- Automated: `./scripts/build-and-install.sh RadioMusic && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please run the following tests and confirm all pass:
  1. `./scripts/build-and-install.sh --dry-run RadioMusic` - shows all phases without executing
  2. `./scripts/build-and-install.sh --no-install RadioMusic` - builds without installing
  3. `./scripts/build-and-install.sh RadioMusic` - full build and install succeeds
  4. Check `~/Library/Audio/Plug-Ins/VST3/` contains RadioMusic.vst3
  5. Check `~/Library/Audio/Plug-Ins/Components/` contains RadioMusic.component
  6. Check `logs/RadioMusic/` contains timestamped build log"

---

### Phase 4b: troubleshooter Agent Implementation

#### Task 11: Create troubleshooter Agent Structure

**Description**: Create troubleshooter agent directory with YAML frontmatter and basic structure.

**Required Reading**:
- @procedures/agents/troubleshooter.md (sections: all)
- @architecture/02-core-abstractions.md (sections: Subagents)
- @architecture/03-model-selection-extended-thinking-strategy.md (sections: Model Selection, Extended Thinking)

**Dependencies**: Phase 4a complete (can be done in parallel)

**Implementation Steps**:
1. Create `.claude/agents/troubleshooter.md` file
2. Add YAML frontmatter:
   ```yaml
   ---
   agent: troubleshooter
   role: Deep research agent for build failures and JUCE problems
   model: opus  # Complex research requires strongest model
   extendedThinking: true  # Always enable for deep research
   tools:
     - Bash
     - Read
     - Grep
     - Glob
     - WebSearch
     - WebFetch
     - mcp__context7__search_juce_docs  # Official JUCE docs
   restrictions:
     - No Write or Edit tools (research only)
     - No Task tool (focused investigation)
   output_format: structured_markdown_report
   ---
   ```
3. Add agent description section
4. Add three sections: Purpose, Responsibilities, Graduated Depth Protocol
5. Document agent will use existing procedures/agents/troubleshooter.md as specification

**Expected Output**:
- `.claude/agents/troubleshooter.md` exists
- YAML frontmatter complete with correct model and tools
- Basic structure in place

**Verification**:
- Automated: `test -f .claude/agents/troubleshooter.md && echo "PASS" || echo "FAIL"`
- Automated: `grep -q "agent: troubleshooter" .claude/agents/troubleshooter.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please open `.claude/agents/troubleshooter.md` and confirm the YAML frontmatter specifies `model: opus` and `extendedThinking: true`."

---

#### Task 12: Implement troubleshooter Graduated Depth Protocol

**Description**: Implement 4-level research protocol (Quick → Local Docs → Context7 → Web).

**Required Reading**:
- @procedures/agents/troubleshooter.md (sections: Research Protocol)
- @procedures/skills/deep-research.md (sections: Graduated Depth Protocol)
- @architecture/03-model-selection-extended-thinking-strategy.md (sections: Extended Thinking Usage)

**Dependencies**: Task 11 complete

**Implementation Steps**:
1. Add Level 0 - Quick Assessment section:
   - Check if immediately obvious (syntax errors, common mistakes)
   - If yes: Answer directly, no research needed
   - If no: Proceed to Level 1
2. Add Level 1 - Local Troubleshooting Docs section:
   - Search `docs/troubleshooting/*.md` files
   - Use hierarchical search: exact match → fuzzy match → full-text
   - Filter by YAML front matter (problem_type, component, symptoms)
   - If high confidence match: Return solution
   - If no match: Proceed to Level 2
3. Add Level 2 - Context7 Official Docs section:
   - Query Context7 for JUCE component documentation
   - Look for API docs, migration guides, best practices
   - Check version compatibility (JUCE 8.0.9+)
   - If clear answer: Return solution
   - If unclear: Proceed to Level 3
4. Add Level 3 - Web Research section:
   - Multiple search query variations
   - Evaluate source credibility (JUCE forum > GitHub > Stack Overflow)
   - Cross-reference 2-3 sources
   - Check version compatibility
   - Always provide confidence level (HIGH/MEDIUM/LOW)
5. Add instruction: "STOP at earliest level with confident answer"
6. Add instruction: "Use extended thinking at ALL levels for complex problems"

**Expected Output**:
- Complete 4-level graduated protocol documented
- Clear decision criteria for each level
- Instructions to stop at earliest confident answer
- Extended thinking usage specified

**Verification**:
- Automated: `grep -q "Level 0" .claude/agents/troubleshooter.md && grep -q "Level 3" .claude/agents/troubleshooter.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the graduated depth protocol in `.claude/agents/troubleshooter.md` and confirm it has 4 levels (0-3) with clear criteria for progressing between levels."

---

#### Task 13: Implement troubleshooter Report Format

**Description**: Define structured report format for research findings.

**Required Reading**:
- @procedures/agents/troubleshooter.md (sections: Report Format)
- @architecture/07-communication-architecture.md (sections: Subagent → Dispatcher)
- @procedures/skills/deep-research.md (sections: Report Format)

**Dependencies**: Task 12 complete

**Implementation Steps**:
1. Add "Report Format" section with markdown template:
   ```markdown
   ## Research Report: [Problem Description]

   ### Problem Identified
   - **Error/Issue:** [exact error message or behavior]
   - **Context:** [JUCE version, component, file/line if applicable]
   - **Root Cause:** [technical explanation of WHY problem occurs]

   ### Research Path
   [Level 0 | Level 1 | Level 2 | Level 3]

   #### Research Findings
   - **[Source Name]:** [key findings, credibility assessment]
   - **[Source Name]:** [key findings, credibility assessment]

   ### Confidence Assessment
   - **Confidence Level:** [HIGH | MEDIUM | LOW]
   - **Reasoning:** [why this confidence level]
   - **Uncertainty:** [what remains unclear, if anything]

   ### Recommended Solution
   [Detailed implementation steps with technical rationale]

   **Why This Works:** [Technical explanation]

   ### Alternative Solutions (if applicable)
   [Other approaches with pros/cons]

   ### Prevention
   [How to avoid this problem in future]

   ### References
   - [Source 1 URL]
   - [Source 2 URL]

   ### Suggest Documentation
   [If solution is novel, suggest documenting with troubleshooting-docs skill]
   ```
2. Add instruction: Reports must be structured markdown (not JSON)
3. Add instruction: Always include confidence assessment
4. Add instruction: Never propose uncertain solutions as confident
5. Add instruction: Suggest `troubleshooting-docs` skill after novel solutions

**Expected Output**:
- Complete report template documented
- Clear structure for all required sections
- Instructions for confidence assessment
- Integration point with troubleshooting-docs skill

**Verification**:
- Automated: `grep -q "Research Report" .claude/agents/troubleshooter.md && grep -q "Confidence Assessment" .claude/agents/troubleshooter.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the report format section in `.claude/agents/troubleshooter.md` and confirm it includes sections for Problem, Research Findings, Confidence Assessment, and Recommended Solution."

---

#### Task 14: Add troubleshooter Research Rules

**Description**: Document research rules, version checking, and source evaluation.

**Required Reading**:
- @procedures/agents/troubleshooter.md (sections: Research Rules)
- @procedures/skills/deep-research.md (sections: Research Rules)
- @procedures/skills/juce-foundation.md (sections: API Verification)

**Dependencies**: Task 13 complete

**Implementation Steps**:
1. Add "Research Rules" section with key principles:
   - STOP when confident (don't over-research obvious problems)
   - Always explain WHY (technical reasoning, not just "try this")
   - Never propose uncertain solutions as confident (be honest about confidence)
   - Version numbers matter (check solution applies to JUCE 8.0.9+)
   - Evaluate source credibility (JUCE forum > official docs > GitHub > Stack Overflow)
   - Cross-reference multiple sources for complex problems
2. Add "API Version Checking" instructions:
   - Verify class/method exists in JUCE 8.0.9+
   - Check for deprecated APIs (use Context7)
   - Note migration paths if API changed between versions
3. Add "Source Credibility Assessment":
   - Tier 1: Official JUCE documentation, JUCE forum maintainer posts
   - Tier 2: JUCE GitHub issues/PRs, experienced forum users
   - Tier 3: Stack Overflow with upvotes, blog posts
   - Tier 4: Random forums, unverified sources (require cross-reference)
4. Add instruction: "Use extended thinking to reason through complex API interactions"

**Expected Output**:
- Complete research rules documented
- API version checking requirements specified
- Source credibility tiers defined
- Extended thinking usage reinforced

**Verification**:
- Automated: `grep -q "Research Rules" .claude/agents/troubleshooter.md && grep -q "Source Credibility" .claude/agents/troubleshooter.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the research rules in `.claude/agents/troubleshooter.md` and confirm they emphasize stopping when confident, version checking, and source credibility assessment."

---

#### Task 15: Add troubleshooter Example Scenarios

**Description**: Add 3 example scenarios showing graduated protocol in action.

**Required Reading**:
- @procedures/agents/troubleshooter.md (sections: all)
- @procedures/examples/ (pattern reference)

**Dependencies**: Task 14 complete

**Implementation Steps**:
1. Add "Example Scenarios" section
2. Add Example 1 - Level 0 Resolution:
   - **Problem:** "error: 'juce::AudioParameterFloat' was not declared"
   - **Analysis:** Missing include
   - **Resolution:** "Add `#include <juce_audio_processors/juce_audio_processors.h>`"
   - **Level:** 0 (immediately obvious)
3. Add Example 2 - Level 2 Resolution:
   - **Problem:** "WebView shows blank page, no console errors"
   - **Analysis:** Searches local docs (nothing), checks Context7 (finds resource provider requirement)
   - **Resolution:** "Implement resource provider pattern for JUCE 8"
   - **Level:** 2 (Context7)
4. Add Example 3 - Level 3 Resolution:
   - **Problem:** "AU validation fails with cryptic error code"
   - **Analysis:** Not in local docs, Context7 has no specifics, web search finds JUCE forum thread explaining macOS sandboxing issue
   - **Resolution:** "Add entitlements plist with audio-input permission"
   - **Level:** 3 (Web research)
5. Format each example with problem → research path → solution → level

**Expected Output**:
- 3 complete example scenarios
- Shows graduated protocol in practice
- Demonstrates when to stop at each level
- Realistic problems from JUCE development

**Verification**:
- Automated: `grep -q "Example Scenarios" .claude/agents/troubleshooter.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the example scenarios in `.claude/agents/troubleshooter.md` and confirm they show 3 examples resolved at different levels (0, 2, 3) of the graduated protocol."

---

#### Task 16: Test troubleshooter Agent

**Description**: Manual test of troubleshooter agent with simulated build error.

**Required Reading**:
- @procedures/agents/troubleshooter.md (sections: all)
- @architecture/07-communication-architecture.md (sections: Dispatcher → Subagent)

**Dependencies**: Task 15 complete

**Implementation Steps**:
1. Create test scenario: Simulate a build error (e.g., "missing JUCE module")
2. Manually invoke troubleshooter via Task tool:
   ```
   Invoke troubleshooter agent to research this build error:

   Error: Could not find JUCE module: juce_dsp
   Context: Building plugin "TestPlugin" with CMake
   CMakeLists.txt includes: juce_audio_processors, juce_audio_utils
   JUCE version: 8.0.9

   Please investigate using the graduated depth protocol and provide a structured report.
   ```
3. Verify agent returns structured markdown report (not JSON)
4. Verify report includes all required sections
5. Verify confidence assessment is clear
6. Verify solution is actionable
7. Check agent stopped at appropriate level (likely Level 0 or 2)

**Expected Output**:
- troubleshooter agent successfully invoked
- Returns structured markdown report
- Report follows template format
- Solution is clear and actionable
- Confidence level appropriate

**Verification**:
- Automated: N/A (manual invocation required)
- Manual: STOP AND ASK LEX: "Please manually invoke the troubleshooter agent using the Task tool with the test scenario above. Confirm that:
  1. The agent returns a structured markdown report
  2. The report includes Problem, Research Findings, Confidence Assessment, and Recommended Solution
  3. The confidence level is clearly stated (HIGH/MEDIUM/LOW)
  4. The solution is specific and actionable"

---

### Phase 4c: build-automation Skill Implementation

#### Task 17: Create build-automation Skill Structure

**Description**: Create build-automation skill directory with frontmatter and basic structure.

**Required Reading**:
- @procedures/skills/build-automation.md (sections: all)
- @architecture/04-component-architecture.md (sections: Skills)
- @architecture/10-extension-architecture.md (sections: Adding New Skills)

**Dependencies**: Phase 4a complete (build script must exist)

**Implementation Steps**:
1. Create `.claude/skills/build-automation/` directory
2. Create `.claude/skills/build-automation/SKILL.md` file
3. Add YAML frontmatter:
   ```yaml
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
   ```
4. Add sections: Purpose, Integration Points, Build Workflow, Failure Protocol

**Expected Output**:
- `.claude/skills/build-automation/` directory exists
- `SKILL.md` exists with correct frontmatter
- Basic structure in place

**Verification**:
- Automated: `test -d .claude/skills/build-automation && test -f .claude/skills/build-automation/SKILL.md && echo "PASS" || echo "FAIL"`
- Automated: `grep -q "skill: build-automation" .claude/skills/build-automation/SKILL.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please confirm that `.claude/skills/build-automation/SKILL.md` exists with YAML frontmatter."

---

#### Task 18: Implement build-automation Build Workflow

**Description**: Implement main build workflow with flag handling and logging.

**Required Reading**:
- @procedures/skills/build-automation.md (sections: Build Workflow)
- @architecture/11-build-automation-architecture.md (sections: Flags, Logging)

**Dependencies**: Task 17 complete

**Implementation Steps**:
1. Add "Build Workflow" section with step-by-step process:
   - **Input validation:** Verify plugin name provided, plugin directory exists
   - **Determine flags:** Check if `--no-install` needed (Stage 2) or full build (Stages 3-6)
   - **Invoke script:** Call `./scripts/build-and-install.sh [PluginName] [flags]`
   - **Monitor output:** Display build progress in real-time
   - **Capture exit code:** Check if build succeeded (exit 0) or failed (exit 1)
   - **Log file location:** Show path to log file in `logs/[PluginName]/`
2. Add instructions for flag usage:
   - Stage 2 (foundation): Always use `--no-install` (verify compilation only)
   - Stages 3-6: Full build with installation
   - User manual requests: Ask if they want `--dry-run` to preview
3. Add logging instructions:
   - Always show log file path after build
   - On failure, log contains full error output for troubleshooting
   - Logs timestamped for multiple builds
4. Add success path instructions:
   - Display success message with build time
   - Show installed plugin locations (VST3, AU)
   - Present continuation decision menu (see Task 19)

**Expected Output**:
- Complete build workflow documented
- Flag usage instructions clear
- Logging behavior specified
- Success and failure paths defined

**Verification**:
- Automated: `grep -q "Build Workflow" .claude/skills/build-automation/SKILL.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the Build Workflow section in `.claude/skills/build-automation/SKILL.md` and confirm it covers input validation, flag determination, script invocation, and logging."

---

#### Task 19: Implement build-automation 4-Option Failure Protocol

**Description**: Implement structured failure handling with 4 user options.

**Required Reading**:
- @procedures/skills/build-automation.md (sections: Failure Protocol)
- @procedures/core/interactive-decision-system.md (sections: Decision Menu Patterns)
- @architecture/13-error-handling-recovery.md (sections: Build Failures)

**Dependencies**: Task 18 complete

**Implementation Steps**:
1. Add "Failure Protocol" section with decision menu:
   ```markdown
   When build fails, present this decision menu:

   ⚠️ Build failed

   What would you like to do?
   1. Investigate - Run troubleshooter agent to research the error (recommended)
   2. Show me the build log - Display full build output for manual review
   3. Show me the code - Open relevant source files where error occurred
   4. Wait - I'll fix it manually and tell you to resume
   5. Other

   Choose (1-5): _
   ```
2. Add instructions for each option:
   - **Option 1 (Investigate):**
     - Extract error from build log (last 50 lines or first error)
     - Invoke troubleshooter agent via Task tool
     - Pass: error message, context (plugin name, JUCE version, file/line)
     - Wait for troubleshooter report
     - Display report with findings and recommended solution
     - Ask: "Should I apply this solution?" (Yes/No/Modify)
   - **Option 2 (Show build log):**
     - Read log file from `logs/[PluginName]/build_TIMESTAMP.log`
     - Display last 100 lines (or full log if < 100 lines)
     - After display, re-present decision menu
   - **Option 3 (Show code):**
     - Parse error for file/line reference
     - Read and display relevant source file with context (±10 lines)
     - After display, re-present decision menu
   - **Option 4 (Wait):**
     - Display message: "Please fix the issue manually. When ready, say 'resume build' or 'retry build'."
     - Exit skill, await user continuation
   - **Option 5 (Other):**
     - Accept free-form user input
     - Interpret request and act accordingly
3. Add instruction: "NEVER auto-retry without user decision"
4. Add instruction: "After Option 1 applies solution, offer to retry build"
5. Add instruction: "Re-present menu after Options 2-3 (iterative debugging)"

**Expected Output**:
- Complete 4-option failure protocol documented
- Each option has clear implementation instructions
- Decision menu format specified
- Iterative debugging flow defined
- Integration with troubleshooter agent specified

**Verification**:
- Automated: `grep -q "Failure Protocol" .claude/skills/build-automation/SKILL.md && grep -q "Investigate" .claude/skills/build-automation/SKILL.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the Failure Protocol section in `.claude/skills/build-automation/SKILL.md` and confirm it includes a 4-option decision menu with detailed instructions for each option."

---

#### Task 20: Implement build-automation Success Protocol

**Description**: Implement success handling with verification and decision menu.

**Required Reading**:
- @procedures/skills/build-automation.md (sections: Success Protocol)
- @procedures/core/interactive-decision-system.md (sections: Checkpoint Menus)

**Dependencies**: Task 19 complete

**Implementation Steps**:
1. Add "Success Protocol" section with verification steps:
   - Confirm build script exit code 0
   - Verify log shows "Phase 7: Verification" success
   - Check installed plugin locations from script output
2. Add success message template:
   ```markdown
   ✓ Build successful

   Built and installed:
   - VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3
   - AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component

   Build time: [duration]
   Log: logs/[PluginName]/build_TIMESTAMP.log
   ```
3. Add context-aware decision menu (varies by invoking stage):
   - **From Stage 2 (foundation):** "Foundation verified" → Continue to Stage 3 / Review code / Pause
   - **From Stage 3 (shell):** "Shell built" → Continue to Stage 4 / Test in DAW / Pause
   - **From Stage 4 (DSP):** "DSP built" → Run tests (automatic) / Continue to Stage 5 / Pause
   - **From Stage 5 (GUI):** "GUI built" → Run tests (automatic) / Continue to Stage 6 / Pause
   - **From Stage 6 (validation):** "Plugin complete" → Install / Test / Improve / Pause
   - **From plugin-improve:** "Update built" → Test changes / Continue improving / Done
4. Add instruction: "Always return control to invoking skill after success"

**Expected Output**:
- Success protocol documented with verification
- Success message template provided
- Context-aware decision menus specified
- Clear return-to-caller instructions

**Verification**:
- Automated: `grep -q "Success Protocol" .claude/skills/build-automation/SKILL.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the Success Protocol section in `.claude/skills/build-automation/SKILL.md` and confirm it includes verification steps, success message template, and context-aware decision menus."

---

#### Task 21: Add build-automation Integration Examples

**Description**: Document integration with plugin-workflow and plugin-improve.

**Required Reading**:
- @procedures/skills/build-automation.md (sections: Integration)
- @procedures/skills/plugin-workflow.md (sections: Stages 2-6)
- @procedures/skills/plugin-improve.md (sections: Phase 5)

**Dependencies**: Task 20 complete

**Implementation Steps**:
1. Add "Integration Examples" section
2. Add Example 1 - plugin-workflow Stage 2:
   ```markdown
   **Scenario:** foundation-agent completes, plugin-workflow needs to verify compilation

   **Invocation:**
   - Skill: plugin-workflow
   - Stage: 2 (Foundation)
   - Action: Invoke build-automation with `--no-install` flag

   **Success Path:**
   - Builds without installing
   - Verifies compilation succeeds
   - Returns to plugin-workflow
   - plugin-workflow presents Stage 2 completion menu

   **Failure Path:**
   - Build fails (likely CMake configuration issue)
   - build-automation presents 4-option menu
   - User chooses "Investigate"
   - troubleshooter diagnoses CMake issue
   - Solution applied
   - Retry build
   - Success → return to plugin-workflow
   ```
3. Add Example 2 - plugin-workflow Stages 3-6:
   ```markdown
   **Scenario:** dsp-agent completes Phase 4.2, plugin-workflow needs full build + install

   **Invocation:**
   - Skill: plugin-workflow
   - Stage: 4, Phase: 2
   - Action: Invoke build-automation (no flags)

   **Success Path:**
   - Builds VST3 + AU
   - Installs to system folders
   - Clears caches
   - Verifies installation
   - Returns to plugin-workflow
   - plugin-workflow auto-invokes plugin-testing

   **Failure Path:**
   - Build fails (likely DSP code error)
   - build-automation presents 4-option menu
   - User chooses "Show me the code"
   - Reviews error in processBlock()
   - Fixes manually
   - Says "resume build"
   - Retry build
   - Success → return to plugin-workflow
   ```
4. Add Example 3 - plugin-improve:
   ```markdown
   **Scenario:** plugin-improve applies bug fix, needs to rebuild

   **Invocation:**
   - Skill: plugin-improve
   - Phase: 5 (Build & Test)
   - Action: Invoke build-automation (no flags)

   **Success Path:**
   - Builds and installs updated plugin
   - Returns to plugin-improve
   - plugin-improve continues to Phase 6 (CHANGELOG)

   **Failure Path:**
   - Build fails (regression from fix)
   - build-automation presents 4-option menu
   - User chooses "Investigate"
   - troubleshooter identifies issue
   - Solution applied
   - Retry build
   - Success → return to plugin-improve
   ```

**Expected Output**:
- 3 integration examples documented
- Covers different invoking contexts
- Shows both success and failure paths
- Demonstrates skill-to-skill flow

**Verification**:
- Automated: `grep -q "Integration Examples" .claude/skills/build-automation/SKILL.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review the Integration Examples section in `.claude/skills/build-automation/SKILL.md` and confirm it shows 3 examples covering plugin-workflow Stage 2, Stages 3-6, and plugin-improve integration."

---

#### Task 22: Test build-automation Skill

**Description**: Manual test of build-automation skill invocation and failure handling.

**Required Reading**:
- @procedures/skills/build-automation.md (sections: all)
- @architecture/04-component-architecture.md (sections: Workflows)

**Dependencies**: Tasks 17-21 complete

**Implementation Steps**:
1. Create test prompt for direct invocation:
   ```markdown
   Please invoke the build-automation skill to build RadioMusic plugin.

   Context: This is a manual test of the build-automation skill.
   Expected: Build succeeds, shows success message with installation paths.
   ```
2. Verify skill invokes build script correctly
3. Verify success message matches template
4. Verify decision menu presented (or return to caller instruction)
5. Create intentional build failure scenario (e.g., introduce syntax error in RadioMusic)
6. Invoke build-automation again
7. Verify 4-option failure menu presented
8. Choose "Investigate" option
9. Verify troubleshooter agent invoked
10. Verify report displayed and solution offered
11. Fix issue and verify retry succeeds

**Expected Output**:
- build-automation skill successfully builds plugin
- Success message clear and informative
- Failure protocol works as specified
- troubleshooter integration works
- Retry logic functions correctly

**Verification**:
- Automated: N/A (manual skill invocation required)
- Manual: STOP AND ASK LEX: "Please perform the following tests:
  1. Invoke build-automation skill manually to build RadioMusic (should succeed)
  2. Introduce a syntax error in RadioMusic source code
  3. Invoke build-automation skill again (should fail)
  4. Verify 4-option failure menu appears
  5. Choose 'Investigate' option
  6. Verify troubleshooter agent is invoked and returns a report
  7. Fix the syntax error manually
  8. Say 'retry build'
  9. Verify build now succeeds

  Confirm all steps work as expected."

---

#### Task 23: Integrate build-automation into plugin-workflow

**Description**: Update plugin-workflow skill to use build-automation at appropriate stages.

**Required Reading**:
- @procedures/skills/plugin-workflow.md (sections: Stages 2-6)
- @procedures/skills/build-automation.md (sections: Integration)
- @architecture/04-component-architecture.md (sections: Component Relationships)

**Dependencies**: Tasks 1-22 complete

**Implementation Steps**:
1. Open `.claude/skills/plugin-workflow/SKILL.md` for editing
2. Locate Stage 2 (Foundation) build verification section
3. Replace manual build commands with build-automation invocation:
   ```markdown
   After foundation-agent completes:
   1. Invoke build-automation skill with `--no-install` flag
   2. If success: Continue to Stage 2 completion menu
   3. If failure: build-automation handles failure protocol, return here after resolution
   ```
4. Locate Stage 3 (Shell) build section
5. Replace with build-automation invocation (no flags)
6. Locate Stage 4 (DSP) build sections (may have phases 4.1, 4.2, etc.)
7. Replace with build-automation invocation after each phase
8. Locate Stage 5 (GUI) build section
9. Replace with build-automation invocation
10. Locate Stage 6 (Validation) build section
11. Replace with build-automation invocation (final build before release)
12. Add note: "build-automation skill handles all build failures with 4-option protocol"
13. Remove any manual build error handling code (now delegated to build-automation)

**Expected Output**:
- plugin-workflow stages 2-6 use build-automation skill
- Manual build commands removed
- Error handling delegated to build-automation
- Clean separation of concerns (workflow orchestration vs build execution)

**Verification**:
- Automated: `grep -q "build-automation" .claude/skills/plugin-workflow/SKILL.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review `.claude/skills/plugin-workflow/SKILL.md` and confirm that Stages 2-6 now invoke the build-automation skill instead of running build commands directly. Also confirm that build error handling has been removed (delegated to build-automation)."

---

#### Task 24: Integrate build-automation into plugin-improve

**Description**: Update plugin-improve skill to use build-automation in Phase 5.

**Required Reading**:
- @procedures/skills/plugin-improve.md (sections: Phase 5)
- @procedures/skills/build-automation.md (sections: Integration)

**Dependencies**: Task 23 complete

**Implementation Steps**:
1. Open `.claude/skills/plugin-improve/SKILL.md` for editing (if exists, otherwise note for future)
2. Locate Phase 5 (Build & Test) section
3. Replace manual build commands with build-automation invocation:
   ```markdown
   After implementation changes complete:
   1. Invoke build-automation skill (no flags)
   2. If success: Continue to Phase 6 (CHANGELOG)
   3. If failure: build-automation handles failure protocol
      - If fix requires code changes, return to Phase 3 (Implementation)
      - If fix is configuration, apply and retry
   ```
4. Remove any manual build error handling
5. Add note: "Build failures during improvements handled by build-automation's troubleshooter integration"

**Expected Output**:
- plugin-improve Phase 5 uses build-automation skill
- Manual build commands removed
- Clean integration maintained

**Verification**:
- Automated: N/A (plugin-improve skill may be partially implemented)
- Manual: STOP AND ASK LEX: "Please check if `.claude/skills/plugin-improve/SKILL.md` exists and has Phase 5 build integration. If it exists, confirm it now uses build-automation skill. If it doesn't exist yet, note that this integration will be needed when plugin-improve is fully implemented in Phase 7."

---

#### Task 25: Update CLAUDE.md Navigation Index

**Description**: Document Phase 4 components in CLAUDE.md navigation index.

**Required Reading**:
- @architecture/02-core-abstractions.md (sections: Navigation Index)
- @architecture/09-file-system-design.md (sections: CLAUDE.md)

**Dependencies**: Tasks 1-24 complete

**Implementation Steps**:
1. Open `.claude/CLAUDE.md` for editing
2. Locate "Scripts" section (or add if missing)
3. Add build script reference:
   ```markdown
   - `scripts/build-and-install.sh` - Centralized build automation (7-phase pipeline)
   ```
4. Locate "Skills" section
5. Add build-automation skill reference:
   ```markdown
   - `build-automation` - Build orchestration with failure protocol (`scripts/build-and-install.sh`)
   ```
6. Locate "Agents" section
7. Add troubleshooter agent reference:
   ```markdown
   - `troubleshooter` - Diagnostic research (graduated depth protocol: Local → Context7 → Web)
   ```
8. Verify formatting consistency with existing entries
9. Keep descriptions concise (WHERE + WHAT, not HOW)

**Expected Output**:
- CLAUDE.md includes Phase 4 components
- Descriptions are navigation-focused (not detailed instructions)
- Formatting consistent with existing entries

**Verification**:
- Automated: `grep -q "build-and-install.sh" .claude/CLAUDE.md && grep -q "build-automation" .claude/CLAUDE.md && grep -q "troubleshooter" .claude/CLAUDE.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please open `.claude/CLAUDE.md` and confirm that it now references:
  1. `scripts/build-and-install.sh` in the Scripts section
  2. `build-automation` skill in the Skills section
  3. `troubleshooter` agent in the Agents section

  Verify descriptions are concise and navigation-focused."

---

#### Task 26: Update PLUGINS.md State Machine Integration

**Description**: Verify PLUGINS.md registry is compatible with build-automation workflow.

**Required Reading**:
- @architecture/06-state-architecture.md (sections: Plugin Lifecycle)
- @procedures/skills/build-automation.md (sections: Integration)

**Dependencies**: Task 25 complete

**Implementation Steps**:
1. Open `PLUGINS.md` for review
2. Verify status legend includes required states:
   - 💡 Ideated
   - 🚧 Stage N (in development)
   - ✅ Working (validated, not installed)
   - 📦 Installed (in system folders)
3. Verify RadioMusic and TapeAge are properly tracked
4. Add note in PLUGINS.md (if not present):
   ```markdown
   ## Build Status Notes

   - Builds managed by `build-automation` skill
   - Build logs: `logs/[PluginName]/build_TIMESTAMP.log`
   - Installed plugins: `~/Library/Audio/Plug-Ins/`
   ```
5. Verify no conflicts with build-automation's state updates

**Expected Output**:
- PLUGINS.md compatible with build-automation workflow
- Build status notes added
- Existing plugins properly tracked

**Verification**:
- Automated: `grep -q "Status Legend" PLUGINS.md && grep -q "🚧" PLUGINS.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please open `PLUGINS.md` and confirm:
  1. Status legend includes 💡, 🚧, ✅, 📦 states
  2. RadioMusic and TapeAge are tracked with current status
  3. Build status notes section exists (or is not needed yet)"

---

#### Task 27: Create Phase 4 Verification Checklist

**Description**: Create comprehensive checklist for Phase 4 completion verification.

**Required Reading**:
- @architecture/11-build-automation-architecture.md (sections: all)
- @procedures/scripts/build-and-install.md (sections: all)
- @procedures/skills/build-automation.md (sections: all)
- @procedures/agents/troubleshooter.md (sections: all)

**Dependencies**: Tasks 1-26 complete

**Implementation Steps**:
1. Create `plan/phase-4-verification.md` file
2. Add automated test section with bash commands
3. Add manual verification checklist with specific tests
4. Add integration verification section
5. Add file existence verification section
6. Structure as executable checklist

**Expected Output**:
- Complete verification checklist document
- Mix of automated and manual tests
- Covers all Phase 4 deliverables

**Verification**:
- Automated: `test -f plan/phase-4-verification.md && echo "PASS" || echo "FAIL"`
- Manual: STOP AND ASK LEX: "Please review `plan/phase-4-verification.md` and confirm it provides a comprehensive checklist for verifying Phase 4 completion. This will be executed in Task 28."

---

#### Task 28: Execute Phase 4 Verification

**Description**: Run complete Phase 4 verification checklist.

**Required Reading**:
- `plan/phase-4-verification.md` (created in Task 27)

**Dependencies**: Tasks 1-27 complete

**Implementation Steps**:
1. Execute all automated tests from verification checklist
2. Perform all manual verification steps
3. Document any failures or issues
4. Verify integration points with plugin-workflow
5. Verify integration points with plugin-improve
6. Test end-to-end workflow: `/implement TestPhase4Plugin` through Stage 2 (foundation)
7. Verify build-automation invoked correctly
8. Verify troubleshooter can be invoked on failure
9. Create verification report with PASS/FAIL for each item

**Expected Output**:
- All verification items executed
- Pass/fail status for each item
- Any issues documented
- Phase 4 confirmed complete or issues identified

**Verification**:
- Automated: Run all automated tests from checklist
- Manual: STOP AND ASK LEX: "Please execute the complete Phase 4 verification checklist from `plan/phase-4-verification.md`. For each item, confirm PASS or note FAIL with details. Key tests to perform:
  1. `./scripts/build-and-install.sh --dry-run RadioMusic` - Shows all 7 phases
  2. `./scripts/build-and-install.sh --no-install RadioMusic` - Builds without installing
  3. `./scripts/build-and-install.sh RadioMusic` - Full build and install succeeds
  4. Invoke build-automation skill manually - Works correctly
  5. Simulate build failure - 4-option menu appears
  6. Choose 'Investigate' - troubleshooter agent invoked
  7. Start new plugin workflow - build-automation integrated at Stage 2

  Report results for each test."

---

## Comprehensive Verification Criteria

### Automated Tests

```bash
# Test 1: Directory structure
test -d scripts && test -d logs && echo "PASS: Directories exist" || echo "FAIL: Missing directories"

# Test 2: Build script exists and is executable
test -x scripts/build-and-install.sh && echo "PASS: Build script executable" || echo "FAIL: Script missing or not executable"

# Test 3: Build script dry-run (RadioMusic must exist)
./scripts/build-and-install.sh --dry-run RadioMusic 2>&1 | grep -q "Phase 7" && echo "PASS: All 7 phases shown" || echo "FAIL: Dry-run failed"

# Test 4: Build script --no-install flag
./scripts/build-and-install.sh --no-install RadioMusic 2>&1 | grep -q "Build complete" && echo "PASS: No-install works" || echo "FAIL: No-install failed"

# Test 5: Full build and install (requires RadioMusic to be buildable)
./scripts/build-and-install.sh RadioMusic && echo "PASS: Full build succeeded" || echo "FAIL: Full build failed"

# Test 6: Log file creation
ls logs/RadioMusic/build_*.log &>/dev/null && echo "PASS: Log file exists" || echo "FAIL: No log file"

# Test 7: troubleshooter agent exists
test -f .claude/agents/troubleshooter.md && echo "PASS: troubleshooter exists" || echo "FAIL: troubleshooter missing"

# Test 8: troubleshooter has graduated protocol
grep -q "Level 0" .claude/agents/troubleshooter.md && grep -q "Level 3" .claude/agents/troubleshooter.md && echo "PASS: Protocol complete" || echo "FAIL: Protocol incomplete"

# Test 9: build-automation skill exists
test -d .claude/skills/build-automation && test -f .claude/skills/build-automation/SKILL.md && echo "PASS: build-automation exists" || echo "FAIL: build-automation missing"

# Test 10: build-automation has failure protocol
grep -q "Failure Protocol" .claude/skills/build-automation/SKILL.md && grep -q "Investigate" .claude/skills/build-automation/SKILL.md && echo "PASS: Failure protocol exists" || echo "FAIL: Failure protocol missing"

# Test 11: plugin-workflow integration
grep -q "build-automation" .claude/skills/plugin-workflow/SKILL.md && echo "PASS: plugin-workflow integrated" || echo "FAIL: plugin-workflow not integrated"

# Test 12: CLAUDE.md updated
grep -q "build-and-install.sh" .claude/CLAUDE.md && grep -q "build-automation" .claude/CLAUDE.md && grep -q "troubleshooter" .claude/CLAUDE.md && echo "PASS: CLAUDE.md updated" || echo "FAIL: CLAUDE.md incomplete"

# Test 13: Installed plugins exist (after Test 5 succeeds)
test -d ~/Library/Audio/Plug-Ins/VST3/RadioMusic.vst3 && test -d ~/Library/Audio/Plug-Ins/Components/RadioMusic.component && echo "PASS: Plugins installed" || echo "FAIL: Plugins not installed"

# Test 14: Cache clearing (check caches are fresh after build)
# Note: This checks that caches were cleared (will be empty or very recent)
find ~/Library/Caches/AudioUnitCache -mmin -5 2>/dev/null | wc -l && echo "PASS: Caches cleared recently" || echo "SKIP: No recent cache activity"
```

### Manual Verification Checklist

- [ ] Run `./scripts/build-and-install.sh --dry-run RadioMusic` and confirm it shows all 7 phase names without executing
- [ ] Run `./scripts/build-and-install.sh --no-install RadioMusic` and confirm it builds successfully and exits WITHOUT installing
- [ ] Run `./scripts/build-and-install.sh RadioMusic` and confirm full build and install succeeds
- [ ] Check `logs/RadioMusic/` directory contains timestamped build log with full output
- [ ] Check `~/Library/Audio/Plug-Ins/VST3/RadioMusic.vst3` exists
- [ ] Check `~/Library/Audio/Plug-Ins/Components/RadioMusic.component` exists
- [ ] Open `.claude/agents/troubleshooter.md` and confirm it has:
  - YAML frontmatter with `model: opus` and `extendedThinking: true`
  - 4-level graduated depth protocol (Level 0-3)
  - Structured report format template
  - Research rules and source credibility assessment
  - 3 example scenarios
- [ ] Open `.claude/skills/build-automation/SKILL.md` and confirm it has:
  - YAML frontmatter with invocation points
  - Complete build workflow section
  - 4-option failure protocol with decision menu
  - Success protocol with context-aware menus
  - 3 integration examples
- [ ] Manually invoke build-automation skill to build RadioMusic (should succeed and show success message)
- [ ] Introduce a syntax error in RadioMusic source code
- [ ] Manually invoke build-automation skill again (should fail and show 4-option menu)
- [ ] Choose "Investigate" option and verify troubleshooter agent is invoked
- [ ] Verify troubleshooter returns structured markdown report with diagnosis
- [ ] Fix syntax error and say "retry build" - verify build now succeeds
- [ ] Open `.claude/skills/plugin-workflow/SKILL.md` and confirm Stages 2-6 invoke build-automation skill
- [ ] Open `.claude/CLAUDE.md` and confirm it references all Phase 4 components (build script, build-automation skill, troubleshooter agent)
- [ ] Open `PLUGINS.md` and confirm RadioMusic and TapeAge are tracked with current status

### Integration Tests

#### Test 1: plugin-workflow Stage 2 Integration
**Steps:**
1. Start new plugin workflow: `/implement TestPhase4` (assuming creative brief exists)
2. Allow Stage 0 and Stage 1 to complete
3. Verify foundation-agent completes Stage 2
4. **Critical:** Verify build-automation skill is invoked with `--no-install` flag
5. Verify build succeeds without installation
6. Verify Stage 2 completion menu appears

**Expected:** build-automation seamlessly integrates into workflow

#### Test 2: Build Failure Handling
**Steps:**
1. Introduce intentional error in test plugin (e.g., syntax error)
2. Continue workflow to next build stage
3. Verify build-automation detects failure
4. Verify 4-option failure menu appears
5. Choose "Investigate" option
6. Verify troubleshooter agent invoked with correct context
7. Verify structured report returned
8. Fix error manually
9. Say "retry build"
10. Verify build succeeds and workflow continues

**Expected:** Complete failure → research → fix → retry cycle works

#### Test 3: Full Build and Install
**Steps:**
1. Use existing working plugin (RadioMusic or TapeAge)
2. Invoke build-automation manually
3. Verify all 7 phases execute
4. Verify VST3 and AU built
5. Verify old versions removed
6. Verify new versions installed
7. Verify caches cleared
8. Verify plugins accessible in DAW

**Expected:** End-to-end build pipeline works

### File Existence Verification

```bash
# Phase 4a: Build Script
ls -la scripts/build-and-install.sh
ls -la logs/

# Phase 4b: troubleshooter Agent
ls -la .claude/agents/troubleshooter.md

# Phase 4c: build-automation Skill
ls -la .claude/skills/build-automation/
ls -la .claude/skills/build-automation/SKILL.md

# Integration
grep "build-automation" .claude/skills/plugin-workflow/SKILL.md
grep "build-and-install.sh" .claude/CLAUDE.md
grep "troubleshooter" .claude/CLAUDE.md

# Build Artifacts (after successful build)
ls -la ~/Library/Audio/Plug-Ins/VST3/RadioMusic.vst3
ls -la ~/Library/Audio/Plug-Ins/Components/RadioMusic.component
```

### Success Criteria

Phase 4 is COMPLETE when:

1. ✅ `scripts/build-and-install.sh` exists, is executable, and implements all 7 phases
2. ✅ Build script supports `--dry-run`, `--no-install`, and `--verbose` flags
3. ✅ Build script builds VST3 and AU in parallel using Ninja
4. ✅ Build script clears DAW caches (Ableton, Logic, AU cache)
5. ✅ Build script verifies installation with timestamp checks (<60 seconds)
6. ✅ Build logs written to `logs/[PluginName]/build_TIMESTAMP.log`
7. ✅ `.claude/agents/troubleshooter.md` exists with complete 4-level graduated depth protocol
8. ✅ troubleshooter configured with `model: opus` and `extendedThinking: true`
9. ✅ troubleshooter returns structured markdown reports (not JSON)
10. ✅ `.claude/skills/build-automation/SKILL.md` exists with complete workflow
11. ✅ build-automation implements 4-option failure protocol (Investigate / Show log / Show code / Wait)
12. ✅ build-automation integrates troubleshooter agent via "Investigate" option
13. ✅ build-automation presents context-aware success menus
14. ✅ plugin-workflow Stages 2-6 invoke build-automation skill (not manual build commands)
15. ✅ CLAUDE.md references all Phase 4 components (navigation index complete)
16. ✅ PLUGINS.md compatible with build-automation workflow
17. ✅ Manual test: build-automation successfully builds RadioMusic
18. ✅ Manual test: Intentional build failure triggers 4-option menu
19. ✅ Manual test: "Investigate" option successfully invokes troubleshooter
20. ✅ Manual test: troubleshooter returns actionable diagnostic report
21. ✅ Manual test: Complete plugin workflow uses build-automation at correct stages
22. ✅ All automated tests pass (14 tests from Automated Tests section)
23. ✅ All manual verification items checked (15 items from Manual Verification Checklist)
24. ✅ All integration tests pass (3 tests from Integration Tests section)

---

## Potential Issues & Mitigations

### Issue 1: Build Script Permissions
**Problem:** Build script not executable after creation
**Symptoms:** `Permission denied` when running script
**Mitigation:** Always `chmod +x scripts/build-and-install.sh` immediately after creation (Task 2)

### Issue 2: PRODUCT_NAME Extraction Failure
**Problem:** CMakeLists.txt doesn't have PRODUCT_NAME or uses non-standard format
**Symptoms:** Script fails at Phase 3 with empty PRODUCT_NAME
**Mitigation:** Fallback to directory name with warning (implemented in Task 5)

### Issue 3: Ninja Not Installed
**Problem:** User doesn't have Ninja build system
**Symptoms:** CMake fails with "Generator 'Ninja' not found"
**Mitigation:** Phase 1 validation checks for Ninja, provides clear installation instructions

### Issue 4: AudioComponentRegistrar Won't Die
**Problem:** Process protected or doesn't exist
**Symptoms:** `killall` fails with error
**Mitigation:** Use `killall -9 ... 2>/dev/null || true` to suppress non-critical errors (Task 8)

### Issue 5: troubleshooter Over-Researches
**Problem:** Agent goes to Level 3 web research for obvious problems
**Symptoms:** 10+ minute research for simple syntax error
**Mitigation:** Graduated protocol with explicit "STOP when confident" instruction (Task 12)

### Issue 6: troubleshooter Returns JSON Instead of Markdown
**Problem:** Agent uses subagent report pattern (JSON) instead of research report pattern (markdown)
**Symptoms:** Report parsing fails, information not displayed to user
**Mitigation:** Explicit "structured markdown (not JSON)" instruction in agent frontmatter (Task 11)

### Issue 7: Build Failure Loop
**Problem:** User chooses "Wait" but never says "resume", workflow stuck
**Symptoms:** Workflow exits, user confused about how to continue
**Mitigation:** Clear exit message with resume instructions in Option 4 (Task 19)

### Issue 8: Integration Breaks Existing Workflows
**Problem:** plugin-workflow or plugin-improve already has working build logic
**Symptoms:** Integration creates redundancy or conflicts
**Mitigation:** Carefully review and replace (not add to) existing build logic (Tasks 23-24)

### Issue 9: Cache Clearing Too Aggressive
**Problem:** Clearing all cache files affects other plugins
**Symptoms:** User complaints about DAW rescanning all plugins
**Mitigation:** This is expected behavior (necessary for detection), document in script comments

### Issue 10: Build Logs Accumulate
**Problem:** Multiple builds create many log files
**Symptoms:** `logs/` directory grows large over time
**Mitigation:** Not addressed in Phase 4 (future enhancement: log rotation/cleanup)

### Issue 11: Parallel Build Resource Exhaustion
**Problem:** Building VST3 + AU simultaneously uses too much CPU/memory on slower machines
**Symptoms:** Build fails with out-of-memory or system freezes
**Mitigation:** Ninja automatically limits parallelism based on system, but could add `--parallel N` flag in future

### Issue 12: WebView Plugin Binary Data
**Problem:** Build script doesn't handle `juce_add_binary_data` for WebView plugins
**Symptoms:** WebView plugins build but don't load UI resources
**Mitigation:** JUCE handles this automatically in CMakeLists.txt, build script doesn't need special handling (verified in architecture docs)

---

## Notes for Next Phase

### Phase 5 Dependencies (Validation System)

Phase 5 will build on Phase 4's infrastructure:

1. **Hook Integration:** SubagentStop hook will call build-automation after each subagent completion
2. **Validation Scripts:** Will parse build logs from `logs/` directory for automated validation
3. **Build Verification:** PostToolUse hook may check for real-time safety violations before building
4. **Error Recovery:** Validation failures will use same 4-option failure protocol pattern

**Preparation Needed:**
- Build logs must be machine-parseable (already implemented with structured output)
- troubleshooter reports must be machine-readable for pattern recognition (already markdown with clear structure)
- build-automation success/failure must be detectable by hooks (exit codes already implemented)

### Phase 6 Dependencies (WebView UI System)

Phase 6 will extend build system for WebView plugins:

1. **CMake Binary Data:** Build script already handles standard JUCE CMake (no changes needed)
2. **Resource Verification:** May add Phase 7 check for `ui/public/` directory in WebView plugins
3. **UI Build Artifacts:** Build logs should show binary data generation (already logged)

**Preparation Needed:**
- Consider adding `--verify-webview` flag to check for WebView-specific build artifacts
- May need WebView-specific troubleshooting docs (troubleshooter will use existing graduated protocol)

### Future Enhancements (Post-Phase 7)

Consider for future versions:

1. **Build Caching:** Incremental builds for faster iteration (CMake already provides this)
2. **Log Rotation:** Automatic cleanup of old build logs (e.g., keep last 10)
3. **Build Metrics:** Track build times, success rates, common failures
4. **Windows Support:** Extend build script for Windows (WebView2, Visual Studio)
5. **Linux Support:** Add Linux build targets (WebKit, GCC)
6. **Build Notifications:** macOS notifications for long builds
7. **Parallel Plugin Builds:** Build multiple plugins simultaneously
8. **Build Profiles:** Debug, Release, RelWithDebInfo configurations
9. **Custom Build Targets:** Support non-standard JUCE targets
10. **Build Analytics:** Track which subagents cause most build failures

---

## Meta

**Phase:** 4 (Build & Troubleshooting System)
**Sub-Phases:** 3 (4a: Build Script, 4b: troubleshooter Agent, 4c: build-automation Skill)
**Total Tasks:** 28
**Estimated Implementation Time:** 6-8 hours (assuming RadioMusic plugin builds successfully)
**Prerequisites:** Phase 0-3 complete (confirmed)
**Deliverables:** 3 (build script, troubleshooter agent, build-automation skill)
**Integration Points:** 2 (plugin-workflow, plugin-improve)
**Documentation Updates:** 1 (CLAUDE.md)

**Task Breakdown:**
- Phase 4a (Build Script): Tasks 1-10 (~3-4 hours)
- Phase 4b (troubleshooter Agent): Tasks 11-16 (~1-2 hours)
- Phase 4c (build-automation Skill): Tasks 17-28 (~2-3 hours)

**Critical Success Factors:**
1. Build script must handle all edge cases (spaces in names, missing PRODUCT_NAME, etc.)
2. troubleshooter must stop at appropriate research level (not over-research)
3. build-automation failure protocol must be intuitive and effective
4. Integration with plugin-workflow must be seamless (no workflow disruption)

**Risk Assessment:**
- **Low Risk:** Build script implementation (well-specified, testable)
- **Low Risk:** troubleshooter agent (research-only, no side effects)
- **Medium Risk:** build-automation integration (depends on plugin-workflow structure)
- **Medium Risk:** Failure protocol UX (user must understand 4 options clearly)

---

## Implementation Notes

### Best Practices for Implementation

1. **Test Incrementally:** After each task, run verification before proceeding
2. **Use RadioMusic:** Test with RadioMusic plugin (known working state)
3. **Keep Backups:** Before integrating into plugin-workflow, backup existing version
4. **Document Decisions:** If implementation differs from spec, document why
5. **Real Failures:** Test with actual build failures (not just simulated)

### Common Pitfalls to Avoid

1. **Don't Skip Verification:** Each task has verification for a reason
2. **Don't Auto-Retry:** build-automation must NEVER retry without user decision
3. **Don't Over-Complicate:** Follow specifications exactly, resist adding features
4. **Don't Break Existing Workflows:** Integration should enhance, not disrupt
5. **Don't Ignore Edge Cases:** Spaces in names, missing fields, etc. must work

### Debug Strategies

If build script fails:
1. Run with `--dry-run` to see commands without executing
2. Check log file for full error output
3. Test each phase independently (comment out later phases)
4. Verify CMakeLists.txt has required fields (PRODUCT_NAME, etc.)

If troubleshooter fails:
1. Check YAML frontmatter for correct model/tools
2. Verify Context7 MCP is available
3. Test graduated protocol manually (invoke agent with known error)
4. Check report format matches template

If build-automation fails:
1. Test build script independently first
2. Check skill invocation matches documented pattern
3. Verify decision menu format is correct (inline numbered list)
4. Test failure protocol with intentional error

---

## Final Checklist Before Phase 5

Before declaring Phase 4 complete and moving to Phase 5:

- [ ] All 28 tasks completed with verification
- [ ] All automated tests pass (14 tests)
- [ ] All manual verification items checked (15 items)
- [ ] All integration tests pass (3 tests)
- [ ] Build script works with RadioMusic plugin
- [ ] troubleshooter agent returns actionable reports
- [ ] build-automation skill handles success and failure paths
- [ ] plugin-workflow successfully uses build-automation
- [ ] CLAUDE.md updated with Phase 4 components
- [ ] Phase 4 verification document created and executed
- [ ] No regressions in existing Phase 0-3 functionality
- [ ] Ready for Phase 5 (Validation System) implementation

**Sign-off:** Phase 4 implementation plan complete and ready for execution.
