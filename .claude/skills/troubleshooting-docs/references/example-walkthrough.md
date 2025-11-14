# Example Walkthrough: troubleshooting-docs Skill

## Scenario

Parameter state management issue fixed in ReverbPlugin. User confirms: "That worked! The parameter is saving correctly now."

## Key Decision Points

### Decision 1: Is this worth documenting?

**Context**: DECAY parameter wasn't persisting after DAW reload. Took 3 investigation attempts (checked APVTS creation, verified parameter ID, finally discovered state sync issue).

**Decision**: YES - document it
- **Why**: Non-obvious solution (state sync between UI and APVTS)
- **Trigger**: Multiple investigation attempts needed
- **Skip if**: Single-attempt fix like typo or syntax error

### Decision 2: Update existing doc or create new?

**Context**: Step 3 search found similar issue: `parameter-not-saving-reverb-OtherPlugin-20251105.md`

**Decision**: CREATE NEW with cross-reference
- **Why**: Different root cause (that one was APVTS initialization, this is state sync)
- **Update existing if**: Exact same root cause, just different plugin
- **Edge case**: If root cause unclear, ask user which to choose

### Decision 3: Promote to Required Reading?

**Context**: System detects:
- Severity: `moderate` (not critical)
- Scope: Single plugin (ReverbPlugin)
- Complexity: Non-obvious but not foundational

**Decision**: DON'T auto-suggest Required Reading
- **Suggest if**: Critical severity + affects multiple plugins + foundational stage (1 or 3)
- **Example that WOULD suggest**: Thread violation affecting all Stage 2 DSP implementations
- **User can still choose**: Option 2 in decision menu always available

## Edge Cases

### Missing context in conversation

If plugin name, exact error, or solution steps not in conversation:
1. Ask user for missing details with specific questions
2. BLOCK (don't proceed to Step 3)
3. WAIT for response

**Example**:
```
I need details to document this:
1. Plugin name?
2. Exact error message?
3. What fixed it?
```

### YAML validation failure

If first attempt produces:
```yaml
problem_type: compilation_error  # ❌ Not valid enum
severity: high                    # ❌ Should be "critical"/"moderate"/"minor"
```

**What happens**:
1. Load `schema.yaml` to show allowed values
2. Show specific errors with correct enum options
3. BLOCK until corrected
4. Retry validation

### Similar issue with ambiguous relationship

Search finds 2 matches:
- `parameter-not-saving-reverb-PluginA.md` (APVTS init issue)
- `parameter-state-loss-PluginB.md` (state sync issue)

**Decision**:
- Show both matches to user
- Let user decide: new doc, update existing, or cross-reference both
- WAIT for choice before proceeding

## See Also

- [SKILL.md](../SKILL.md) - Full workflow steps
- [yaml-schema.md](yaml-schema.md) - Schema reference
