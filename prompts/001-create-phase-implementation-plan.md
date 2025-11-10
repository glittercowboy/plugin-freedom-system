<objective>
Generate a comprehensive implementation plan for a specific phase from @ROADMAP.md that breaks down EXACTLY what needs to be implemented, with mandatory references to ALL relevant architecture and procedure documentation, proper sub-phase splitting when needed, and thorough verification criteria to ensure the vision is fully realized.

This plan will serve as the authoritative implementation guide ensuring nothing is forgotten and the phase is completed to specification with proper testing and verification at every step.
</objective>

<context>
You are creating a detailed implementation plan for phase $ARGUMENT (hereby called $PHASE_NUMBER) from the roadmap.
The plan must be exhaustive, leaving no room for interpretation or missed requirements.
This is for a plugin system (TÂCHES) that emphasizes freedom, simplicity, and user empowerment.
The implementation will be done incrementally with verification at each step.
</context>

<requirements>
1. Read @ROADMAP.md and locate phase $PHASE_NUMBER
2. Validate prerequisites before planning (see <prerequisite_validation>)
3. Thoroughly analyze what the phase entails, considering all implications
4. Determine if the phase should be split into sub-phases (e.g., 1a, 1b, 1c):
   - Split if the phase involves multiple distinct components (commands + skills + hooks)
   - Split if implementation would be clearer with logical groupings
   - Split if different parts have different dependencies or complexity levels
5. For each phase/sub-phase, create a sequential implementation plan that includes:
   - Every single task needed to complete that portion
   - The exact order tasks should be completed
   - Dependencies between tasks clearly stated
6. Reference EVERY relevant file from @architecture/ and @procedures/ directories
7. Create comprehensive verification criteria mixing automated and manual tests
8. Ensure the plan leaves nothing to chance - be explicit about every requirement
</requirements>

<prerequisite_validation>
Before planning phase $PHASE_NUMBER, verify prerequisite completion:

1. Read @ROADMAP.md and identify the "Prerequisites" section for phase $PHASE_NUMBER
2. If Phase 0: Skip validation (no prerequisites exist)
3. If Phase 1+: Check previous phase "Core Deliverables":
   - For each deliverable, verify file/directory existence using ls or similar
   - List what EXISTS vs what's EXPECTED
   - Check for key structural elements (directories, critical files)
4. Document findings for plan's "Prerequisites Status" section:
   - ✅ Complete: All deliverables exist
   - ⚠️ Partial: Some deliverables missing (list them specifically)
   - ❌ Incomplete: Most/all deliverables missing (list them specifically)
5. If incomplete/partial: Create highly visible warning section in plan
6. ALWAYS generate the plan regardless of prerequisite status - user decides when to implement

Note: This uses file existence checks only. Quality validation happens via phase verification checklists.
</prerequisite_validation>

<research>
Before creating the plan, you MUST:
1. Read @ROADMAP.md in its entirety to understand phase $PHASE_NUMBER in context
2. Read EVERY file in @architecture/ - do not skip any, each contains critical context:
   - Read each file completely, not just scanning
   - Note how each file relates to the phase being implemented
   - Identify which architectural decisions affect this phase
3. Read EVERY file in @procedures/ and its subdirectories:
   - @procedures/agents/
   - @procedures/commands/
   - @procedures/core/
   - @procedures/examples/
   - @procedures/scripts/
   - @procedures/skills/
   - @procedures/webview/
   - Each file contains implementation specifications that MUST be followed
4. Cross-reference the phase requirements with the architecture and procedures
5. Identify any gaps or ambiguities that need clarification
</research>

<implementation>
Structure the plan as follows:

# Phase $PHASE_NUMBER Implementation Plan

## Prerequisites Status

[If Phase 0: "No prerequisites for Phase 0."]

[If Phase 1+:]
**Previous Phase(s) Required:** Phase X[, Phase Y...]

**Deliverables Check:**
- ✅ `/path/to/deliverable` - EXISTS
- ❌ `/path/to/missing` - MISSING
- ⚠️ `/path/to/partial` - PARTIAL (explain what's missing)

**Status:** [Complete / Partial / Incomplete]

[If Partial or Incomplete, add visible warning:]

---
## ⚠️ PREREQUISITE WARNING ⚠️

**Prerequisites [PARTIAL/INCOMPLETE]. Phase X must be finished first.**

**Missing deliverables:**
- `specific/path/to/missing/item`
- `another/missing/deliverable`

**This plan assumes prerequisites will be completed. Do not begin implementation until Phase X verification passes.**

---

## Phase Overview

[Brief description of what this phase accomplishes]
[Why this phase is critical to the overall vision]

## Sub-Phase Breakdown (if applicable)

[Explain why this phase is being split]
[List each sub-phase with brief description]

## Required Documentation

### Architecture Files (MUST READ IN ENTIRETY)

- @architecture/[filename].md - [Why this file is needed for this phase]
  [List EVERY relevant architecture file]

### Procedure Files (MUST READ IN ENTIRETY)

- @procedures/[path]/[filename].md - [Why this file is needed for this phase]
  [List EVERY relevant procedure file]

## Implementation Sequence

### [Phase/Sub-Phase Number]: [Name]

#### Task 1: [Specific Task Name]

**Description**: [Detailed description of what needs to be done]
**Required Reading**:

- @architecture/[specific-file].md (sections: all)
- @procedures/[specific-file].md (sections: all)
  **Dependencies**: [Any tasks that must be completed first]
  **Implementation Steps**:

1. [Specific step with exact file to create/modify]
2. [Next step with explicit instructions]
   **Expected Output**: [Exact files/changes that should exist after completion]
   **Verification**:

- Automated: [Specific test command or script]
- Manual: STOP AND ASK LEX: "Please verify that [specific behavior] occurs when [specific action]"

**CRITICAL:** After specifying manual verification, the implementation MUST pause and wait for Lex's confirmation before proceeding to the next task. Manual verification is a blocking checkpoint, not optional documentation.

[Continue for every single task in perfect sequential order]

## Comprehensive Verification Criteria

### Automated Tests

```bash
# Test 1: [What it verifies]
[exact command to run]

# Test 2: [What it verifies]
[exact command to run]
```

### Manual Verification Checklist

- [ ] When I [do X], does [Y happen]?
- [ ] Can you confirm that [specific feature] works by [specific test]?
- [ ] Please test: [exact manual test procedure]

### File Existence Verification

```bash
# Verify all expected files exist
ls -la [path/to/expected/file1]
ls -la [path/to/expected/file2]
# ... for every file that should be created
```

### Integration Tests

[Tests that verify components work together]

### Success Criteria

Phase $PHASE_NUMBER is COMPLETE when:

1. [Specific measurable criterion]
2. [Another specific criterion]
3. All automated tests pass
4. All manual verification items are checked
5. All expected files exist and function correctly

## Potential Issues & Mitigations

[Anticipate what could go wrong and how to handle it]

## Notes for Next Phase

[Any setup or considerations for the following phase]
</implementation>

<constraints>
- Never skip reading any architecture or procedure file - each contains unique critical context
- Be exhaustive - if something might be needed, include it
- Make verification criteria extremely specific - ambiguity leads to bugs
- Order tasks logically for sequential implementation
- Include both "happy path" and error case testing
- Reference specific line numbers or sections when pointing to documentation
- Make manual test instructions so clear that they cannot be misinterpreted
- If a phase seems too large even after splitting, note this and recommend further division
- Manual verification steps are BLOCKING checkpoints - implementation must pause and wait for user response
- Use explicit language: "STOP AND ASK LEX:" to indicate required user interaction before proceeding
- Each task's manual verification must be completed and confirmed by Lex before the next task begins
- Never treat manual verification as optional documentation or combine multiple task verifications into a single checkpoint
</constraints>

<output>
Save the implementation plan to: `./plan/phase-$PHASE_NUMBER-plan.md`
If sub-phases are created, save each to: `./plan/phase-$PHASE_NUMBER$LETTER-plan.md`
Create the ./plan/ directory if it doesn't exist.
</output>

<verification>
Before declaring complete:
1. Verify every task in the phase has been addressed in the plan
2. Confirm every relevant architecture file is referenced
3. Confirm every relevant procedure file is referenced
4. Check that verification criteria exist for every single task
5. Ensure the sequential order makes logical sense
6. Verify that manual test instructions are unambiguous
7. Confirm success criteria are measurable and comprehensive
</verification>

<success_criteria>
The plan is successful when:

- It references 100% of relevant documentation
- Another developer could implement the phase using ONLY this plan
- Every possible failure point has a verification step
- The sequential order prevents any dependency issues
- Both automated and manual testing cover all functionality
- Success can be definitively proven through the verification criteria
  </success_criteria>
