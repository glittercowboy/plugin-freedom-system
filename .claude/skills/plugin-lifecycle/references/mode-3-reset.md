---
name: reset-to-ideation
description: Roll back plugin to ideation stage - keep idea/mockups, remove all implementation
args: "[PluginName]"
---

# /reset-to-ideation - Surgical Rollback

**Purpose:** Reset plugin to just-after-dream-stage, removing all implementation but keeping the idea, mockups, and parameter specs intact.

**Use case:** "The implementation went wrong, but the concept and UI design are solid. Start fresh."

## What Gets Preserved

✅ **Keep (The Ideation):**
- `.ideas/creative-brief.md` - Your original vision
- `.ideas/mockups/` - All UI exploration (v1, v2, v3, etc.)
- `.ideas/mockups/*.yaml` - Aesthetic specifications
- `.ideas/parameter-spec.md` - Generated from finalized mockup

✅ **Keep (Backup):**
- Timestamped backup of removed code: `backups/rollbacks/[PluginName]_TIMESTAMP.tar.gz`

## What Gets Removed

❌ **Remove (The Implementation):**
- `Source/` directory - All C++ code
- `CMakeLists.txt` - Build configuration
- `.ideas/architecture.md` - DSP specification (Stage 0 output)
- `.ideas/plan.md` - Implementation strategy (Stage 1 output)
- `.ideas/.continue-here.md` - Handoff state
- Build artifacts in `build/plugins/[PluginName]/`
- Installed binaries (VST3, AU) if present

## What Gets Updated

📝 **Update:**
- **PLUGINS.md:** Status → `💡 Ideated`
- **Lifecycle Timeline:** Add rollback entry, preserve history

## State After Reset

You'll be back at exactly this point:

```
plugins/[PluginName]/
  .ideas/
    creative-brief.md          ← Your vision (preserved)
    mockups/
      v1-ui.html              ← UI iterations (preserved)
      v1-ui.yaml
      v2-ui.html
      v2-ui.yaml
    parameter-spec.md          ← Parameters (preserved)

  [No Source/, no CMakeLists.txt, nothing else]
```

**Next step:** `/implement [PluginName]` starts from Stage 0 (research).

## Safety Protocol

**Requires confirmation:**

```
⚠️  Rolling back to ideation stage

Will REMOVE:
- Source code: plugins/[PluginName]/Source/ (47 files)
- Build config: plugins/[PluginName]/CMakeLists.txt
- Implementation docs: architecture.md, plan.md
- Binaries: VST3 + AU (if installed)
- Build artifacts

Will PRESERVE:
- Idea: creative-brief.md
- Mockups: 2 versions (v1-ui, v2-ui)
- Parameters: parameter-spec.md

A backup will be created in backups/rollbacks/

Status will change: [Current] → 💡 Ideated

Continue? (y/N): _
```

## Execution Steps

### 1. Validate Plugin Exists

```bash
# Check PLUGINS.md
if ! grep -q "^### $PLUGIN_NAME$" PLUGINS.md; then
  echo "❌ Plugin '$PLUGIN_NAME' not found in PLUGINS.md"
  exit 1
fi

# Check if already ideated
STATUS=$(grep -A2 "^### $PLUGIN_NAME$" PLUGINS.md | grep "Status:" | grep -o "💡")

if [ "$STATUS" = "💡" ]; then
  echo "ℹ️  Plugin is already at ideation stage (💡 Ideated)"
  echo "Nothing to roll back."
  exit 0
fi
```

### 2. Check Prerequisites (Creative Brief Must Exist)

```bash
BRIEF_FILE="plugins/$PLUGIN_NAME/.ideas/creative-brief.md"

if [ ! -f "$BRIEF_FILE" ]; then
  echo "❌ Cannot roll back: No creative brief found"
  echo ""
  echo "This plugin has no ideation artifacts to preserve."
  echo "Use /destroy instead if you want to remove completely."
  exit 1
fi
```

### 3. Gather Information

```bash
SOURCE_DIR="plugins/$PLUGIN_NAME/Source"
BUILD_DIR="build/plugins/$PLUGIN_NAME"
CMAKE_FILE="plugins/$PLUGIN_NAME/CMakeLists.txt"
ARCH_FILE="plugins/$PLUGIN_NAME/.ideas/architecture.md"
PLAN_FILE="plugins/$PLUGIN_NAME/.ideas/plan.md"
HANDOFF_FILE="plugins/$PLUGIN_NAME/.ideas/.continue-here.md"

# Count files to remove
SOURCE_COUNT=$(find "$SOURCE_DIR" -type f 2>/dev/null | wc -l | xargs)

# Check for installed binaries
if [ -f "$CMAKE_FILE" ]; then
  PRODUCT_NAME=$(grep "PRODUCT_NAME" "$CMAKE_FILE" | sed 's/.*"\(.*\)".*/\1/')
  VST3_PATH="$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3"
  AU_PATH="$HOME/Library/Audio/Plug-Ins/Components/${PRODUCT_NAME}.component"
fi

# Count preserved files
MOCKUP_COUNT=$(ls -1 plugins/$PLUGIN_NAME/.ideas/mockups/*.html 2>/dev/null | wc -l | xargs)
```

### 4. Present Confirmation Prompt

Show what will be removed vs preserved, require confirmation.

### 5. Create Backup

```bash
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
BACKUP_DIR="backups/rollbacks"
BACKUP_FILE="${BACKUP_DIR}/${PLUGIN_NAME}_${TIMESTAMP}.tar.gz"

mkdir -p "$BACKUP_DIR"

# Backup everything that will be removed
BACKUP_ITEMS=""
[ -d "$SOURCE_DIR" ] && BACKUP_ITEMS="$BACKUP_ITEMS $SOURCE_DIR"
[ -f "$CMAKE_FILE" ] && BACKUP_ITEMS="$BACKUP_ITEMS $CMAKE_FILE"
[ -f "$ARCH_FILE" ] && BACKUP_ITEMS="$BACKUP_ITEMS $ARCH_FILE"
[ -f "$PLAN_FILE" ] && BACKUP_ITEMS="$BACKUP_ITEMS $PLAN_FILE"
[ -f "$HANDOFF_FILE" ] && BACKUP_ITEMS="$BACKUP_ITEMS $HANDOFF_FILE"

if [ -n "$BACKUP_ITEMS" ]; then
  tar -czf "$BACKUP_FILE" $BACKUP_ITEMS --transform "s|^plugins/$PLUGIN_NAME/|${PLUGIN_NAME}/|"

  if [ -f "$BACKUP_FILE" ]; then
    BACKUP_SIZE=$(du -sh "$BACKUP_FILE" | awk '{print $1}')
    echo "✓ Backup created: $BACKUP_FILE ($BACKUP_SIZE)"
  else
    echo "❌ Backup failed. Aborting."
    exit 1
  fi
fi
```

### 6. Uninstall Binaries (If Installed)

```bash
if [ -d "$VST3_PATH" ] || [ -f "$AU_PATH" ]; then
  echo "Uninstalling binaries..."

  rm -rf "$VST3_PATH" 2>/dev/null
  rm -rf "$AU_PATH" 2>/dev/null

  # Clear DAW caches
  rm -rf ~/Library/Caches/Ableton/*/PluginCache* 2>/dev/null
  rm -rf ~/Library/Caches/AudioUnitCache* 2>/dev/null

  echo "✓ Binaries uninstalled"
fi
```

### 7. Remove Build Artifacts

```bash
if [ -d "$BUILD_DIR" ]; then
  rm -rf "$BUILD_DIR"
  echo "✓ Build artifacts removed"
fi
```

### 8. Remove Implementation Files

```bash
# Remove source code
if [ -d "$SOURCE_DIR" ]; then
  rm -rf "$SOURCE_DIR"
  echo "✓ Source code removed: $SOURCE_DIR"
fi

# Remove CMakeLists.txt
if [ -f "$CMAKE_FILE" ]; then
  rm "$CMAKE_FILE"
  echo "✓ Build config removed: CMakeLists.txt"
fi

# Remove implementation docs
[ -f "$ARCH_FILE" ] && rm "$ARCH_FILE" && echo "✓ Removed: architecture.md"
[ -f "$PLAN_FILE" ] && rm "$PLAN_FILE" && echo "✓ Removed: plan.md"
[ -f "$HANDOFF_FILE" ] && rm "$HANDOFF_FILE" && echo "✓ Removed: .continue-here.md"
```

### 9. Update PLUGINS.md

```bash
# Update status to 💡 Ideated
awk -v plugin="$PLUGIN_NAME" '
  /^### / { if ($0 ~ plugin) in_plugin=1; else in_plugin=0 }
  in_plugin && /^\*\*Status:\*\*/ {
    print "**Status:** 💡 **Ideated**"
    next
  }
  { print }
' PLUGINS.md > PLUGINS.md.tmp
mv PLUGINS.md.tmp PLUGINS.md

# Add rollback entry to lifecycle timeline
ROLLBACK_ENTRY="- **$(date +%Y-%m-%d):** Reset to ideation stage - implementation removed, mockups preserved"

awk -v plugin="$PLUGIN_NAME" -v entry="$ROLLBACK_ENTRY" '
  /^### / { if ($0 ~ plugin) in_plugin=1; else in_plugin=0 }
  in_plugin && /^\*\*Lifecycle Timeline:\*\*/ {
    print
    getline
    print
    print entry
    next
  }
  { print }
' PLUGINS.md > PLUGINS.md.tmp
mv PLUGINS.md.tmp PLUGINS.md

echo "✓ PLUGINS.md updated: Status → 💡 Ideated"
```

### 10. Success Report

```
✓ [PluginName] reset to ideation stage

Removed:
- Source code: 47 files
- Build configuration: CMakeLists.txt
- Implementation docs: architecture.md, plan.md
- Binaries: VST3 + AU (uninstalled)
- Build artifacts

Preserved:
- Creative brief: creative-brief.md ✓
- UI mockups: 2 versions ✓
- Parameters: parameter-spec.md ✓

Backup available at:
backups/rollbacks/[PluginName]_20251110_235612.tar.gz

Status changed: [Previous] → 💡 Ideated

Next steps:
1. Review mockups and parameter spec
2. Run /implement [PluginName] to start fresh from Stage 0
3. Or modify creative brief and re-run /dream
```

## Recovery Instructions

If you need to restore the removed implementation:

```bash
# List backups
ls -lh backups/rollbacks/

# Extract backup
cd plugins/[PluginName]/
tar -xzf ../../backups/rollbacks/[PluginName]_TIMESTAMP.tar.gz --strip-components=1

# Update PLUGINS.md status manually back to previous state

# Resume development
/continue [PluginName]
```

## Error Handling

**Already at ideation stage:**
```
ℹ️  Plugin is already at ideation stage (💡 Ideated)

Nothing to roll back. Current state:
- Creative brief: ✓ Exists
- Mockups: 2 versions
- Implementation: None (as expected)

To implement: /implement [PluginName]
```

**No creative brief (nothing to preserve):**
```
❌ Cannot roll back: No creative brief found

This plugin has no ideation artifacts to preserve.
Use /destroy instead if you want to remove completely.
```

**Plugin not found:**
```
❌ Plugin 'Foo' not found in PLUGINS.md

Available plugins:
- GainKnob
- TapeAge
- ClapMachine
```

## Comparison with Other Commands

| Command | Source Code | Binaries | Idea/Mockups | PLUGINS.md Entry |
|---------|-------------|----------|--------------|------------------|
| `/uninstall` | ✅ Keep | ❌ Remove | ✅ Keep | ✅ Keep (update status) |
| `/reset-to-ideation` | ❌ Remove | ❌ Remove | ✅ Keep | ✅ Keep (reset to 💡) |
| `/destroy` | ❌ Remove | ❌ Remove | ❌ Remove | ❌ Remove |

## Integration Points

**Invoked by:**
- `/reset-to-ideation [PluginName]`
- User phrases: "start over", "go back to ideation", "reset implementation"

**Routes to:**
- None (terminal operation)

**Workflow after reset:**
```
💡 Ideated → /implement → Stage 0 → ... → Stage 6 → ✅ Working
```

## Use Cases

### Scenario 1: DSP Architecture Was Wrong

```
You: "The architecture.md we created won't work for this effect."
You: "/reset-to-ideation TapeAge"

Claude: [Confirms, creates backup, removes implementation]
Claude: ✓ TapeAge reset to ideation stage
        Preserved mockups and parameters.
        Status → 💡 Ideated

You: "/implement TapeAge"
Claude: [Starts Stage 0 with fresh research, different architecture]
```

### Scenario 2: Want Different UI Approach

```
You: "I like the idea but want to try a different mockup design."
You: "/reset-to-ideation GuitarPedal"

[After reset]

You: "/mockup GuitarPedal"
[Creates new v3 mockup with different layout]

You: "/implement GuitarPedal"
[Uses new mockup for implementation]
```

### Scenario 3: Complexity Underestimated

```
You: "This is way more complex than Complexity 3. Need to restart with proper phasing."
You: "/reset-to-ideation Reverb"

[After reset, creative brief preserved]

You: "/implement Reverb"
Claude: [Stage 0 research]
Claude: [Stage 1 planning - recalculates complexity as 6.5, creates phases]
[Proceeds with proper phased implementation]
```

## Notes for Claude

**When executing this command:**

1. ALWAYS preserve creative brief and mockups - these are the essence
2. Create backup before removing anything
3. Check for installed binaries and uninstall cleanly
4. Update PLUGINS.md status to 💡 Ideated
5. Add rollback entry to lifecycle timeline
6. Clear handoff state (.continue-here.md)
7. Suggest `/implement` as next step

**What NOT to remove:**

- ❌ Creative brief (this is the idea!)
- ❌ Mockups (UI design work is preserved)
- ❌ Parameter spec (generated from finalized mockup)
- ❌ PLUGINS.md entry (just update status)

**Common pitfalls:**

- Removing mockups (they're part of ideation!)
- Not backing up implementation
- Forgetting to clear build artifacts
- Not updating PLUGINS.md status
- Missing installed binaries
