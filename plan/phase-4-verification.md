# Phase 4 Verification Checklist

**Phase:** 4c (Build & Troubleshooting System)
**Components:** build-and-install.sh, troubleshooter agent, build-automation skill
**Date:** 2025-01-10

---

## Automated Tests

Run these bash commands to verify Phase 4 deliverables:

```bash
# Test 1: Directory structure
test -d scripts && test -d logs && echo "✓ PASS: Directories exist" || echo "✗ FAIL: Missing directories"

# Test 2: Build script exists and is executable
test -x scripts/build-and-install.sh && echo "✓ PASS: Build script executable" || echo "✗ FAIL: Script missing or not executable"

# Test 3: Build script dry-run (RadioMusic must exist)
./scripts/build-and-install.sh --dry-run RadioMusic 2>&1 | grep -q "Phase 7" && echo "✓ PASS: All 7 phases shown" || echo "✗ FAIL: Dry-run failed"

# Test 4: Build script --no-install flag
./scripts/build-and-install.sh --no-install RadioMusic 2>&1 | grep -q "Build" && echo "✓ PASS: No-install option works" || echo "✗ FAIL: No-install failed"

# Test 5: troubleshooter agent exists
test -f .claude/agents/troubleshooter.md && echo "✓ PASS: troubleshooter exists" || echo "✗ FAIL: troubleshooter missing"

# Test 6: troubleshooter has graduated protocol
grep -q "Level 0" .claude/agents/troubleshooter.md && grep -q "Level 3" .claude/agents/troubleshooter.md && echo "✓ PASS: Protocol complete" || echo "✗ FAIL: Protocol incomplete"

# Test 7: build-automation skill exists
test -d .claude/skills/build-automation && test -f .claude/skills/build-automation/SKILL.md && echo "✓ PASS: build-automation exists" || echo "✗ FAIL: build-automation missing"

# Test 8: build-automation has failure protocol
grep -q "Failure Protocol" .claude/skills/build-automation/SKILL.md && grep -q "Investigate" .claude/skills/build-automation/SKILL.md && echo "✓ PASS: Failure protocol exists" || echo "✗ FAIL: Failure protocol missing"

# Test 9: plugin-workflow integration
grep -q "build-automation" .claude/skills/plugin-workflow/SKILL.md && echo "✓ PASS: plugin-workflow integrated" || echo "✗ FAIL: plugin-workflow not integrated"

# Test 10: plugin-improve integration
grep -q "build-automation" .claude/skills/plugin-improve/SKILL.md && echo "✓ PASS: plugin-improve integrated" || echo "✗ FAIL: plugin-improve not integrated"

# Test 11: CLAUDE.md updated
grep -q "build-and-install.sh" .claude/CLAUDE.md && grep -q "build-automation" .claude/CLAUDE.md && grep -q "troubleshooter" .claude/CLAUDE.md && echo "✓ PASS: CLAUDE.md updated" || echo "✗ FAIL: CLAUDE.md incomplete"

# Test 12: PLUGINS.md has build management section
grep -q "Build Management" PLUGINS.md && echo "✓ PASS: PLUGINS.md updated" || echo "✗ FAIL: PLUGINS.md missing build section"

# Test 13: foundation-agent no longer builds
! grep -q "../../build-and-install.sh" .claude/agents/foundation-agent.md && echo "✓ PASS: foundation-agent refactored" || echo "✗ FAIL: foundation-agent still builds"

# Test 14: shell-agent no longer builds
! grep -q "../../build-and-install.sh" .claude/agents/shell-agent.md && echo "✓ PASS: shell-agent refactored" || echo "✗ FAIL: shell-agent still builds"

# Test 15: dsp-agent no longer builds
! grep -q "../../build-and-install.sh" .claude/agents/dsp-agent.md && echo "✓ PASS: dsp-agent refactored" || echo "✗ FAIL: dsp-agent still builds"

# Test 16: gui-agent no longer builds
! grep -q "../../build-and-install.sh" .claude/agents/gui-agent.md && echo "✓ PASS: gui-agent refactored" || echo "✗ FAIL: gui-agent still builds"
```

---

## Manual Verification Checklist

### Build Script Verification

- [ ] Run `./scripts/build-and-install.sh --dry-run RadioMusic` and confirm it shows all 7 phase names without executing
- [ ] Run `./scripts/build-and-install.sh --no-install RadioMusic` and confirm it builds successfully and exits WITHOUT installing
- [ ] Run `./scripts/build-and-install.sh RadioMusic` and confirm full build and install succeeds
- [ ] Check `logs/RadioMusic/` directory contains timestamped build log with full output
- [ ] Check `~/Library/Audio/Plug-Ins/VST3/RadioMusic.vst3` exists (if full build run)
- [ ] Check `~/Library/Audio/Plug-Ins/Components/RadioMusic.component` exists (if full build run)

### troubleshooter Agent Verification

- [ ] Open `.claude/agents/troubleshooter.md` and confirm it has:
  - YAML frontmatter with `model: opus` and `extendedThinking: true`
  - 4-level graduated depth protocol (Level 0-3)
  - Structured report format template
  - Research rules and source credibility assessment
  - 3 example scenarios at different research levels

### build-automation Skill Verification

- [ ] Open `.claude/skills/build-automation/SKILL.md` and confirm it has:
  - YAML frontmatter with invocation points (plugin-workflow, plugin-improve)
  - Complete build workflow section with flag handling
  - 4-option failure protocol with decision menu (Investigate / Show log / Show code / Wait)
  - Success protocol with context-aware menus for different stages
  - 3 integration examples (Stage 2, Stages 3-6, plugin-improve)

### Integration Verification

- [ ] Open `.claude/skills/plugin-workflow/SKILL.md` and confirm:
  - Stage 2 section references build-automation skill after foundation-agent completes
  - Prompts to subagents no longer include "build and install" instructions
  - Build error handling delegated to build-automation

- [ ] Open `.claude/skills/plugin-improve/SKILL.md` and confirm:
  - Phase 5 invokes build-automation skill instead of manual cmake commands
  - Build failure protocol references build-automation's 4-option menu

- [ ] Open `.claude/CLAUDE.md` and confirm it references:
  - `scripts/build-and-install.sh` in Scripts section
  - `build-automation` skill in Skills list
  - `troubleshooter` agent in Subagents list
  - Phase 4 marked as complete in Implementation Status

- [ ] Open `PLUGINS.md` and confirm:
  - Status legend includes all required states (💡, 🚧, ✅, 📦)
  - RadioMusic and TapeAge are tracked with current status
  - Build Management section exists with log and installation paths

### Subagent Refactoring Verification

- [ ] Open `.claude/agents/foundation-agent.md` and confirm:
  - No build script invocation (../../build-and-install.sh removed)
  - Self-validation only checks file creation (no build verification)
  - Note explaining build handled by plugin-workflow

- [ ] Open `.claude/agents/shell-agent.md` and confirm:
  - "Build and Test" section removed
  - Self-validation only checks code (no build verification)
  - Note explaining build handled by plugin-workflow

- [ ] Open `.claude/agents/dsp-agent.md` and confirm:
  - "Build and Test" section removed
  - Self-validation only checks code (no build verification)
  - Note explaining build handled by plugin-workflow

- [ ] Open `.claude/agents/gui-agent.md` and confirm:
  - "Build and Test" section removed
  - Self-validation only checks code and files (no DAW testing)
  - Note explaining build handled by plugin-workflow

---

## Integration Tests

### Test 1: Full Build with RadioMusic

**Purpose:** Verify end-to-end build pipeline works

**Steps:**
1. Ensure RadioMusic plugin exists in `plugins/RadioMusic/`
2. Run: `./scripts/build-and-install.sh RadioMusic`
3. Observe all 7 phases execute
4. Verify build log created: `ls logs/RadioMusic/build_*.log`
5. Verify plugins installed:
   - `ls ~/Library/Audio/Plug-Ins/VST3/RadioMusic.vst3`
   - `ls ~/Library/Audio/Plug-Ins/Components/RadioMusic.component`

**Expected:** Build succeeds, plugins installed, log file created

**Result:** [ ] PASS / [ ] FAIL
**Notes:**

---

### Test 2: Build Script Error Handling

**Purpose:** Verify build script handles non-existent plugin

**Steps:**
1. Run: `./scripts/build-and-install.sh NonExistentPlugin`
2. Verify script exits with error about missing plugin directory
3. Verify error message is clear and actionable

**Expected:** Script fails gracefully with clear error message

**Result:** [ ] PASS / [ ] FAIL
**Notes:**

---

### Test 3: troubleshooter Agent Invocation (Manual)

**Purpose:** Verify troubleshooter can be invoked and returns structured report

**Steps:**
1. Create test prompt:
   ```
   Please invoke the troubleshooter agent to research this error:

   Error: Could not find JUCE module: juce_dsp
   Context: Building plugin "TestPlugin" with CMake
   CMakeLists.txt includes: juce_audio_processors, juce_audio_utils
   JUCE version: 8.0.9
   ```
2. Verify agent returns structured markdown report (not JSON)
3. Verify report includes all required sections:
   - Problem Identified
   - Research Path (Level 0-3)
   - Confidence Assessment
   - Recommended Solution
4. Verify confidence level is clearly stated (HIGH/MEDIUM/LOW)
5. Verify solution is specific and actionable

**Expected:** troubleshooter returns complete, actionable report

**Result:** [ ] PASS / [ ] FAIL
**Notes:**

---

### Test 4: Subagent Separation of Concerns

**Purpose:** Verify subagents no longer build (code generation only)

**Steps:**
1. Review foundation-agent.md, shell-agent.md, dsp-agent.md, gui-agent.md
2. Confirm none contain build script invocations
3. Confirm all have notes about build handled externally
4. Confirm success criteria focus on code/file creation, not build artifacts

**Expected:** All subagents refactored to separate code generation from builds

**Result:** [ ] PASS / [ ] FAIL
**Notes:**

---

## Success Criteria Summary

Phase 4c is **COMPLETE** when all of the following are TRUE:

### Deliverables

- [x] `scripts/build-and-install.sh` exists and is executable
- [x] `.claude/agents/troubleshooter.md` exists with complete protocol
- [x] `.claude/skills/build-automation/SKILL.md` exists with full workflow
- [x] All subagents refactored (foundation, shell, dsp, gui)
- [x] plugin-workflow integrated with build-automation
- [x] plugin-improve integrated with build-automation
- [x] CLAUDE.md updated with Phase 4 components
- [x] PLUGINS.md updated with build management section

### Automated Tests

- [ ] All 16 automated tests pass

### Manual Verification

- [ ] All build script tests pass (6 items)
- [ ] All troubleshooter verification items pass (5 items)
- [ ] All build-automation verification items pass (5 items)
- [ ] All integration verification items pass (4 items)
- [ ] All subagent refactoring items pass (4 items)

### Integration Tests

- [ ] Test 1 (Full Build) passes
- [ ] Test 2 (Error Handling) passes
- [ ] Test 3 (troubleshooter Invocation) passes
- [ ] Test 4 (Subagent Separation) passes

---

## Notes

- **Phase 4a** (build script) was completed separately and is verified by Tests 1-4
- **Phase 4b** (troubleshooter agent) was completed separately and is verified by Tests 5-6 and Test 3
- **Phase 4c** (build-automation skill + integration) is the focus of this verification
- Full workflow testing (plugin-workflow invoking build-automation during Stage 2) should be tested in a future plugin implementation
- Manual invocation of build-automation skill is deferred to user testing (Task 6 in original todo list)

---

**Verification Completed By:** [Your Name]
**Date:** YYYY-MM-DD
**Overall Status:** [ ] PASS / [ ] FAIL / [ ] PARTIAL
