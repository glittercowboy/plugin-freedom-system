# Decision Menu Patterns

All decision menus in ui-mockup skill follow consistent formatting and routing patterns.

## Menu Format Standard

All menus use this structure:
```
✓ [Completion statement]

[Optional: Files generated list]

What would you like to do?

1. [Primary action] - [Brief description]
2. [Secondary action] - [Brief description]
3. [Alternative action] - [Brief description]
4. Other

Choose (1-N): _
```

## Phase 5.5: Design Decision Menu

**Context**: Presented after Phase A completes (v[N]-ui.yaml + v[N]-ui-test.html generated and committed)

**Menu Presentation**:
```
✓ Mockup v[N] design created (2 files)

Files generated:
- v[N]-ui.yaml (design specification)
- v[N]-ui-test.html (browser-testable mockup)

What would you like to do?

1. Iterate - Refine design, adjust layout
2. Finalize - Validate alignment and complete mockup
3. Save as template - Add to aesthetic library for reuse
4. Other

Choose (1-4): _
```

**Option Routing**:

**Option 1: Iterate on design**
- Action: Stay in Phase A, collect feedback, increment version to v[N+1]
- Phase B status: BLOCKED (not ready for implementation)
- Implementation:
  - Collect user feedback on what to change
  - Increment version number: `NEXT_VERSION=$((LATEST_VERSION + 1))`
  - Return to Phase 2 (gap analysis) with new context
  - Generate v[N+1]-ui.yaml and v[N+1]-ui-test.html
  - Do NOT proceed to Phases 6-10

**Option 2: Finalize design**
- Action: Proceed to Phase 5.6 (automatic brief update)
- This is the ONLY option that proceeds to Phase B (files 3-7)
- Verification steps:
  1. Check WebView constraints validation (Phase 5.3 already executed)
  2. Update creative-brief.md from mockup (Phase 5.6 - automatic)
  3. Verify user explicitly confirmed finalization
  4. Mark design as finalized in YAML file
  5. Proceed to Phase 6-10 (generate 5 implementation files)
- If any step fails: show errors, offer retry or manual fix

**Option 3: Save as template**
- Action: Invoke ui-template-library skill
- Phase B status: BLOCKED until design finalized via option 2
- Implementation:
  ```
  Invoke Skill tool:
  - skill: "ui-template-library"
  - context: "Save aesthetic from plugins/[PluginName]/.ideas/mockups/v[N]-ui-test.html"
  ```
  After ui-template-library completes: return to Phase 5.5 decision menu

**Option 4: Other**
- Action: Collect user input, handle custom request, reassess
- Phase B status: BLOCKED until valid finalization option selected
- Common requests:
  - "Test in browser again" → Reopen v[N]-ui-test.html
  - "Validate WebView constraints" → Re-run Phase 5.3 checks
  - "Show me the YAML" → Display v[N]-ui.yaml contents
  - "Exit and save progress" → Update state, exit skill
- After handling: return to Phase 5.5 decision menu

## Completion Menu (After Phase 10)

**Context**: Presented after all 7 mockup files generated (Phases 1-10 complete)

**Menu Presentation**:
```
✓ Mockup v[N] complete (7 files generated)

What's next?

1. Start implementation (invoke plugin-workflow)
2. Create another UI version (explore alternative design)
3. Test in browser (open v[N]-ui-test.html)
4. Other

Choose (1-4): _
```

**Option Routing**:

**Option 1: Start implementation**
- Invoke plugin-workflow skill:
  ```
  Invoke Skill tool:
  - skill: "plugin-workflow"
  - prompt: "Continue workflow for [PluginName] from Stage 1"
  ```

**Option 2: Create another UI version**
- Return to Phase 1 with new version number (v[N+1])
- Explore alternative design approach
- Preserves previous version for comparison

**Option 3: Test in browser**
- Reopen v[N]-ui-test.html in browser
- Allow final verification before implementation

**Option 4: Other**
- Handle custom requests
- Return to appropriate phase or exit

## Menu Consistency Rules

1. All menus end with "Choose (1-N): _"
2. Option labels are concise (1-5 words)
3. "Other" is always the last option
4. Completion statement uses ✓ checkmark
5. After handling any option: return to menu or proceed to next phase (never auto-proceed without user choice)
