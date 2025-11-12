---
name: sync-design
description: Validate mockup ↔ creative brief consistency
argument-hint: "[PluginName]"
---

# /sync-design

## Routing

When user runs `/sync-design [PluginName?]`:

**Phase 1: Parameter Resolution**
- If no plugin name provided: List eligible plugins (have creative-brief.md + mockup finalized) and wait for user selection
- If plugin name provided: Use provided plugin name

**Phase 2: Precondition Validation**
- Verify required files exist (see Preconditions section below)
- If any files missing: Display error with guidance and stop (do not invoke skill)
- If all files present: Proceed to Phase 3

**Phase 3: Skill Invocation**
Invoke the design-sync skill via Skill tool with:
- Plugin name
- Paths to validated files

## Purpose

Compare finalized UI mockup against original creative brief to catch design drift before implementation.

**Detects:**
- Parameter count mismatches (brief says 8, mockup has 12)
- Missing features from brief (brief mentions presets, mockup lacks them)
- Visual style divergence (brief says vintage, mockup is modern)
- Scope creep or reduction

**Prevents:**
- Implementing designs that don't match user's vision
- Wasted work from misaligned expectations
- Surprises during Stage 5 (GUI implementation)

## Usage

```bash
/sync-design [PluginName]
```

## Examples

```bash
/sync-design DelayPlugin
/sync-design ReverbPlugin
```

## What It Does

### Quantitative Checks (Fast)
- Compares parameter counts (brief vs parameter-spec.md)
- Detects missing features (grep brief for keywords, check mockup)
- Measures scope changes (+/- parameters)

### Semantic Validation (LLM)
- Analyzes visual style alignment (brief aesthetic vs mockup design)
- Assesses feature completeness (brief promises vs mockup delivery)
- Evaluates scope changes (justified evolution vs drift)

### Drift Handling

The design-sync skill categorizes drift into 4 levels:
✅ No Drift, ✅ Acceptable Evolution, ⚠️ Attention Required, ❌ Critical

See design-sync skill docs for drift category definitions and handling logic.

## Output

The design-sync skill presents findings and context-appropriate decision menu.

**Example output:**
```
✓ Design-brief alignment verified

- Parameter count: 12 (matches brief)
- All features present: preset browser, bypass, meter
- Visual style aligned: Modern minimal with analog warmth

What's next?
1. Continue implementation (recommended)
2. Review details
3. Other
```

**Menu options adapt based on drift category:**
- No drift: Continue implementation
- Acceptable evolution: Update brief and continue / Review / Revert
- Drift detected: Update mockup / Update brief / Override / Review
- Critical drift: Update mockup / Update brief (no override)

See design-sync skill for complete drift handling logic.

## Preconditions

<preconditions enforcement="blocking">
  <required_files>
    <file path="plugins/$PLUGIN_NAME/.ideas/creative-brief.md"
          purpose="Original vision"
          created_by="ideation">
      creative-brief.md
    </file>
    <file path="plugins/$PLUGIN_NAME/.ideas/parameter-spec.md"
          purpose="Parameter specification from finalized mockup"
          created_by="ui-mockup Phase 4">
      parameter-spec.md
    </file>
    <file path="plugins/$PLUGIN_NAME/.ideas/mockups/vN-ui.yaml"
          purpose="Finalized UI mockup (N = version number)"
          created_by="ui-mockup"
          pattern="v*-ui.yaml">
      mockup file (any version)
    </file>
  </required_files>

  <validation_sequence>
    <step order="1">Extract plugin name from $ARGUMENTS or prompt user if missing</step>
    <step order="2">
      Check existence of all required files
      ```bash
      PLUGIN_NAME="$1"
      test -f "plugins/${PLUGIN_NAME}/.ideas/creative-brief.md" || MISSING="$MISSING creative-brief.md"
      test -f "plugins/${PLUGIN_NAME}/.ideas/parameter-spec.md" || MISSING="$MISSING parameter-spec.md"
      find "plugins/${PLUGIN_NAME}/.ideas/mockups/" -name "v*-ui.yaml" -type f | grep -q . || MISSING="$MISSING mockup"
      ```
    </step>
    <step order="3">
      IF any files missing:
        BLOCK with message:
        ```
        ✗ BLOCKED: Missing required files for design-sync

        [PluginName] is missing files needed for design validation:
        [List missing files here]

        HOW TO UNBLOCK:
        - Missing creative-brief.md: Run /dream [PluginName] to create
        - Missing mockup: Run /dream [PluginName] and finalize UI mockup
        - Missing parameter-spec.md: Finalize mockup (auto-generated in Phase 4)

        Once all files exist, /sync-design will proceed.
        ```
        DO NOT invoke design-sync skill
    </step>
    <step order="4">
      IF all files present:
        Invoke design-sync skill via Skill tool
    </step>
  </validation_sequence>
</preconditions>

## When To Use

**Run when:**
- After mockup finalization (auto-suggested by ui-mockup skill)
- Before Stage 1 Planning as optional pre-check
- Anytime to verify alignment
- When unsure if design matches vision

**Don't use when:**
- Mockup still iterating (v1, v2 - not finalized)
- No creative brief exists
- Just checking visual appearance (open in browser instead)

## Integration Points

### Auto-suggested by ui-mockup
After Phase 4 finalization:
```
✓ Mockup finalized: v3

What's next?
1. Check alignment - Run design-sync (recommended) ←
2. Generate implementation files
3. Iterate design
4. Other
```

### Optional Stage 1 pre-check
Before planning:
```
Mockup finalized: Yes (v3)
design-sync validation: Not yet run

Recommendation: Validate alignment before planning

What's next?
1. Run design-sync (recommended) ←
2. Skip validation - Proceed with planning
3. Other
```

## Technical Details

The design-sync skill uses dual-layer validation (quantitative + semantic) and logs all override decisions to `.validator-overrides.yaml` for audit trail.

## Routes To

`design-sync` skill

## Related Commands

- `/implement [Plugin]` - Full 7-stage workflow (auto-runs design-sync at Stage 1 if mockup exists)
- `/improve [Plugin]` - Enhancement workflow (may re-validate design)

## Why This Matters

**Without design-sync:**
- Brief says 8 parameters → Implement 12 → User surprised
- Brief says "vintage warmth" → Implement modern minimal → Rework required
- Brief mentions presets → Forget to implement → Missing feature discovered late

**With design-sync:**
- Drift detected before implementation starts
- User makes informed decision (update mockup / update brief / proceed)
- No surprises, no wasted work

**The feedback loop:**
1. Brief captures vision
2. Mockup iterates design
3. design-sync catches drift
4. User resolves alignment
5. Implementation matches vision
