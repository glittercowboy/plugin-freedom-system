---
name: test
description: Validate plugins through automated tests
---

# /test

When user runs `/test [PluginName?] [mode?]`, invoke the plugin-testing skill or build-automation skill.

## Preconditions

**Check PLUGINS.md status:**
- Plugin MUST exist
- Status MUST NOT be 💡 Ideated (need implementation to test)

**If status is 💡:**
Reject with message:
```
[PluginName] is not implemented yet (Status: 💡 Ideated).
Use /implement [PluginName] to build it first.
```

## Three Test Methods

### 1. Automated Stability Tests
**Command:** `/test [PluginName] automated`

**Requirements:** `plugins/[Plugin]/Tests/` directory must exist

**Routes to:** plugin-testing skill

**Duration:** ~2 minutes

**Tests:**
- Parameter stability (all combinations, edge cases)
- State save/restore (preset corruption)
- Processing stability (buffer sizes, sample rates)
- Thread safety (concurrent access)
- Edge case handling (silence, extremes, denormals)

### 2. Build and Validate
**Command:** `/test [PluginName] build`

**Requirements:** None (always available)

**Routes to:** build-automation skill

**Duration:** ~5-10 minutes

**Steps:**
1. Build Release mode (VST3 + AU)
2. Run pluginval with strict settings (level 10)
3. Install to system folders
4. Clear DAW caches

### 3. Manual DAW Testing
**Command:** `/test [PluginName] manual`

**Requirements:** None

**Routes to:** Display checklist directly (no skill)

**Checklist includes:**
- Load & initialize
- Audio processing
- Parameter testing
- State management
- Performance
- Compatibility
- Stress testing

## Behavior

**Without plugin name:**
List available plugins with test status:
```
Which plugin would you like to test?

1. [PluginName1] v[X.Y.Z] - Has automated tests ✓
2. [PluginName2] v[X.Y.Z] - Build validation only
3. [PluginName3] v[X.Y.Z] - Has automated tests ✓
```

**With plugin, no mode:**
Present test method options:
```
How would you like to test [PluginName]?

1. Automated stability tests (2 min)
   Run test suite - catches crashes, explosions, broken params

2. Build and validate (5-10 min)
   Compile Release build + run pluginval

3. Manual DAW testing (guidance)
   Show testing checklist for real-world validation
```

Options adapt based on what's available (automated only shown if Tests/ exists).

**With plugin and mode:**
Execute test directly.

## Auto-Invoked During Workflow

plugin-workflow auto-invokes testing:
- After Stage 4 (DSP) completion
- After Stage 5 (GUI) completion

If tests fail, workflow stops until fixed.

## Error Handling

**Automated tests fail:**
```
❌ [N] tests failed:
  - [testName]: [Description]
  - [testName]: [Description]

Options:
1. Investigate failures (triggers deep-research)
2. Show me the test code
3. Show full test output
4. I'll fix it manually
```

**Build fails:**
```
Build error at [stage]:
[Error output with context]

Options:
1. Investigate (triggers deep-research)
2. Show me the code
3. Show full output
4. I'll fix it manually
```

**Pluginval fails:**
```
Pluginval failed [N] tests:
- [Test name]: [Description]
- [Test name]: [Description]

Options:
1. Investigate failures
2. Show full pluginval report
3. Continue anyway (skip validation)
```

## Output

After successful testing:
```
✓ [Test method] passed

Next steps:
- Test manually in DAW (if not done)
- /improve [PluginName] if you find issues
```
