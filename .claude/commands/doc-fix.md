---
name: doc-fix
description: Document a recently solved problem for the knowledge base
argument-hint: "[optional: brief context about the fix]"
---

# /doc-fix

Invoke the troubleshooting-docs skill to document a recently solved problem.

## Purpose

Captures problem solutions while context is fresh, creating structured documentation in a dual-indexed knowledge base (searchable by plugin name OR symptom category).

## Usage

```bash
/doc-fix                    # Document the most recent fix
/doc-fix [brief context]    # Provide additional context hint
```

## What It Captures

- **Problem symptom**: Exact error messages, observable behavior
- **Investigation steps tried**: What didn't work and why
- **Root cause analysis**: Technical explanation
- **Working solution**: Step-by-step fix with code examples
- **Prevention strategies**: How to avoid in future
- **Cross-references**: Links to related issues

## Preconditions

<preconditions enforcement="advisory">
  <check condition="problem_solved">
    Problem has been solved (not in-progress)
  </check>
  <check condition="solution_verified">
    Solution has been verified working
  </check>
  <check condition="non_trivial">
    Non-trivial problem (not simple typo or obvious error)
  </check>
</preconditions>

## What It Creates

**Dual-indexed documentation:**
- Real file: `troubleshooting/by-plugin/[Plugin]/[category]/[filename].md`
- Symlink: `troubleshooting/by-symptom/[category]/[filename].md`

**Categories auto-detected from problem:**
- build-failures/
- runtime-issues/
- validation-problems/
- webview-issues/
- dsp-issues/
- gui-issues/
- parameter-issues/
- api-usage/

## Success Output

```
✓ Solution documented

File created:
- Real: troubleshooting/by-plugin/ReverbPlugin/parameter-issues/[filename].md
- Symlink: troubleshooting/by-symptom/parameter-issues/[filename].md

This documentation will be searched by deep-research skill as Level 1 (Fast Path)
when similar issues occur.

What's next?
1. Continue workflow (recommended)
2. Link related issues
3. Update common patterns
4. View documentation
5. Other
```

## Why This Matters

This creates a compounding knowledge system:

1. First time you solve "Plugin X crashes on parameter change" → Research (30 min)
2. Document the solution → troubleshooting/runtime-issues/crashes-on-param.md (5 min)
3. Next time similar issue occurs → Quick lookup (2 min)
4. Knowledge compounds → System gets smarter

The feedback loop:
```
Build → Test → Find Issue → Research → Improve → Document → Validate → Deploy
    ↑                                                                      ↓
    └──────────────────────────────────────────────────────────────────────┘
```

## Auto-Invoke

<auto_invoke>
  <trigger_phrases>
    - "that worked"
    - "it's fixed"
    - "working now"
    - "problem solved"
  </trigger_phrases>

  <manual_override>
    Use /doc-fix [context] to document immediately without waiting for auto-detection.
  </manual_override>
</auto_invoke>

## Routes To

`troubleshooting-docs` skill

## Related Commands

- `/research [topic]` - Deep investigation (searches troubleshooting/ docs at Level 1)
- `/improve [Plugin]` - Enhancement workflow (may trigger documentation after fixes)
