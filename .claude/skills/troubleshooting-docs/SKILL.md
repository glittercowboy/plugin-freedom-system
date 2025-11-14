---
name: troubleshooting-docs
description: Captures solved problems as categorized documentation with YAML frontmatter for fast lookup. Use when user confirms solution with phrases like 'that worked', 'it's fixed', 'working now', or via /doc-fix command.
allowed-tools:
  - Read # Parse conversation context
  - Write # Create resolution docs
  - Bash # Create directories
  - Grep # Search existing docs
preconditions:
  - Problem has been solved (not in-progress)
  - Solution has been verified working
---

# troubleshooting-docs Skill

**Purpose:** Automatically document solved problems to build searchable institutional knowledge with category-based organization (enum-validated problem types).

## Overview

This skill captures problem solutions immediately after confirmation, creating structured documentation that serves as a searchable knowledge base for future sessions.

**Organization:** Single-file architecture - each problem documented as one markdown file in its symptom category directory (e.g., `troubleshooting/build-failures/cmake-version-mismatch.md`).

---

<critical_sequence name="documentation-capture" enforce_order="strict">

## 7-Step Process

<step number="1" required="true">
### Step 1: Detect Confirmation

**Auto-invoke after phrases:**

- "that worked"
- "it's fixed"
- "working now"
- "problem solved"
- "that did it"

**OR manual:** `/doc-fix` command

**Document when:**

- Multiple investigation attempts needed
- Non-obvious solution that future sessions would benefit from
- Tricky debugging or root cause analysis

**Skip when:**

- Single-attempt fixes (typos, syntax errors, obvious mistakes)
</step>

<step number="2" required="true" depends_on="1">
### Step 2: Gather Context

Extract from conversation: plugin name, symptom (exact error messages), stage (0-6 or post-implementation), solution (code/config changes), investigation attempts, root cause, JUCE version, OS version, file/line references.

**BLOCKING REQUIREMENT:** If critical context is missing (plugin name, exact error, stage, or resolution steps), ask user and WAIT for response before proceeding to Step 3.
</step>

<step number="3" required="false" depends_on="2">
### Step 3: Check Existing Docs (Skip if unique first-time issue)

Search target category directory first (based on problem_type), then expand to all categories if no matches:

```bash
grep -r "exact error phrase" troubleshooting/[category]/
# If no match, expand: grep -r "exact error phrase" troubleshooting/
```

**IF similar issue found:**

THEN present decision options:

```
Found similar issue: troubleshooting/[path]

What's next?
1. Create new doc with cross-reference (recommended)
2. Update existing doc (only if same root cause)
3. Other

Choose (1-3): _
```

WAIT for user response, then execute chosen action.

**ELSE** (no similar issue found):

Proceed directly to Step 4 (no user interaction needed).
</step>

<step number="4" required="true" depends_on="2">
### Step 4: Generate Filename

Format: `[sanitized-symptom]-[plugin]-[YYYYMMDD].md`

**Examples:**
- `missing-juce-dsp-module-DelayPlugin-20251110.md`
- `parameter-not-saving-state-ReverbPlugin-20251110.md`
- `webview-crash-on-resize-TapeAgePlugin-20251110.md`
</step>

<step number="5" required="true" depends_on="4" blocking="true">
### Step 5: Validate YAML Schema

**CRITICAL:** All docs require validated YAML frontmatter with enum validation.

<validation_gate name="yaml-schema" blocking="true">

**Validate against schema:**
Classify the problem against enum values defined in `references/yaml-schema.md`. Ensure all required fields present and match allowed values exactly.

**BLOCK if validation fails:**
Load `schema.yaml` to show specific allowed enum values. Present errors and WAIT for corrected values.

**GATE ENFORCEMENT:** Do NOT proceed to Step 6 until YAML frontmatter passes all validation rules.

</validation_gate>
</step>

<step number="6" required="true" depends_on="5">
### Step 6: Create Documentation

Category mapping from problem_type: See [references/yaml-schema.md](references/yaml-schema.md) for problem_type to directory mapping.

Read `assets/resolution-template.md` and populate with Step 2 context in a single operation:

```bash
PROBLEM_TYPE="[from validated YAML]"
CATEGORY="[mapped from problem_type]"
FILENAME="[generated-filename].md"
DOC_PATH="troubleshooting/${CATEGORY}/${FILENAME}"

mkdir -p "troubleshooting/${CATEGORY}"
# Write documentation with Step 2 context + validated YAML frontmatter
```
</step>

<step number="7" required="conditional" depends_on="6">
### Step 7: Cross-Reference & Pattern Detection

### Step 7a: Cross-Reference (REQUIRED if similar issues found in Step 3)

**If similar issues were found in Step 3**, add bidirectional cross-references:

**Update existing doc:**

```bash
# Add Related Issues link to similar doc
echo "- See also: [$FILENAME]($REAL_FILE)" >> [similar-doc.md]
```

**Update new doc:**
Cross-reference already included from Step 6.

**Update patterns if 3+ similar issues:**

```bash
# Add to troubleshooting/patterns/common-solutions.md
cat >> troubleshooting/patterns/common-solutions.md << 'EOF'

## [Pattern Name]

**Common symptom:** [Description]
**Root cause:** [Technical explanation]
**Solution pattern:** [General approach]

**Examples:**
- [Link to doc 1]
- [Link to doc 2]
- [Link to doc 3]
EOF
```

**If no similar issues found in Step 3**, skip to Step 7b.

### Step 7b: Critical Pattern Detection (OPTIONAL - System Suggestion)

If automatic indicators suggest this might be a critical pattern:
- Severity: `critical` in YAML
- Affects multiple plugins OR foundational stage (Stage 1 or 3)
- Non-obvious solution

Then add suggestion in decision menu (Step 8):
```
💡 This might be worth adding to Required Reading (Option 2)
```

**User decides via decision menu** - NEVER auto-promote to Required Reading.

**Template for Required Reading addition:**

When user selects Option 2, use template from `assets/critical-pattern-template.md`. Number sequentially based on existing patterns in `troubleshooting/patterns/juce8-critical-patterns.md`.
</step>

</critical_sequence>

---

<decision_gate name="post-documentation" wait_for_user="true">

## Decision Menu After Capture

After successful documentation, present options and WAIT for user response:

```
✓ Solution documented

File created:
- troubleshooting/[category]/[filename].md

What's next?
1. Continue workflow (recommended)
2. Add to Required Reading - Promote to critical patterns (juce8-critical-patterns.md)
3. Link related issues - Connect to similar problems
4. Update common patterns - Add to pattern library
5. View documentation - See what was captured
6. Other
```

**Handle responses:**

**Option 1: Continue workflow**

- Return to calling skill/workflow
- Documentation is complete

**Option 2: Add to Required Reading** ⭐ PRIMARY PATH FOR CRITICAL PATTERNS

User selects this when:
- System made this mistake multiple times across different plugins
- Solution is non-obvious but must be followed every time
- Foundational requirement (CMake, JUCE API, threading, etc.)

Action:
1. Extract pattern from the documentation
2. Format as ❌ WRONG vs ✅ CORRECT with code examples
3. Add to `troubleshooting/patterns/juce8-critical-patterns.md`
4. Add cross-reference back to this doc
5. Confirm: "✓ Added to Required Reading. All subagents will see this pattern before code generation."

**Note**: For direct addition without full documentation workflow, users can use `/add-critical-pattern` command.

**Option 3: Link related issues**

- Prompt: "Which doc to link? (provide filename or describe)"
- Search troubleshooting/ for the doc
- Add cross-reference to both docs
- Confirm: "✓ Cross-reference added"

**Option 4: Update common patterns**

- Check if 3+ similar issues exist
- If yes: Add pattern to troubleshooting/patterns/common-solutions.md
- If no: "Need 3+ similar issues to establish pattern (currently N)"

**Option 5: View documentation**

- Display the created documentation
- Present decision menu again

**Option 6: Other**

- Ask what they'd like to do

</decision_gate>

---

<integration_protocol>

## Integration Points

**Invoked by:**
- deep-research skill (after solution found)
- plugin-improve skill (after fix validated)
- /doc-fix command (manual user invocation)

**Invokes:**
- None (terminal skill - does not delegate to other skills)

**Handoff expectations:**
All context needed for documentation should be present in conversation history before invocation.

</integration_protocol>

---

<success_criteria>

## Success Criteria

Documentation is successful when ALL of the following are true:

- ✅ YAML frontmatter validated (all required fields, correct formats)
- ✅ File created in troubleshooting/[category]/[filename].md
- ✅ Enum values match schema.yaml exactly
- ✅ Code examples included in solution section
- ✅ Cross-references added if related issues found
- ✅ User presented with decision menu and action confirmed

</success_criteria>

---

## Example Walkthrough

See [references/example-walkthrough.md](references/example-walkthrough.md) for decision points and edge cases.
