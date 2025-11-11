---
name: destroy
description: Completely remove plugin - source code, binaries, registry entries, everything
args: "[PluginName]"
---

# /destroy - Nuclear Cleanup

**Purpose:** Completely annihilate a plugin and all traces of it from the system.

**⚠️ WARNING:** This is irreversible (except via backup). Use `/reset-to-ideation` if you want to keep the idea/mockups.

## What Gets Removed

1. **Source code:** `plugins/[PluginName]/` directory (ENTIRE folder)
2. **Installed binaries:** VST3, AU, Standalone from system folders
3. **Build artifacts:** `build/plugins/[PluginName]/`
4. **Registry entry:** Complete removal from PLUGINS.md
5. **Troubleshooting docs:** All docs mentioning this plugin (optional)

## What's Preserved (Backup)

Before deletion, creates timestamped backup:
```
backups/destroyed/[PluginName]_YYYYMMDD_HHMMSS.tar.gz
```

Contains:
- Complete source code
- PLUGINS.md entry
- Handoff state (if exists)
- Build artifacts (if any)

Backup can be extracted to restore plugin if needed.

## Safety Protocol

**Requires explicit confirmation:**

```
⚠️  WARNING: Destructive Operation

About to PERMANENTLY DELETE:
- Source: plugins/[PluginName]/ (124 files, 3.2 MB)
- Binaries: VST3 + AU (8.1 MB total)
- Build artifacts: build/plugins/[PluginName]/ (45 MB)
- PLUGINS.md entry

A backup will be created in backups/destroyed/

This CANNOT be undone (except by restoring backup).

Type the plugin name to confirm: _
```

User must type exact plugin name (case-sensitive).

If wrong: "Plugin name doesn't match. Aborting."

## Execution Steps

### 1. Validate Plugin Exists

```bash
# Check PLUGINS.md
if ! grep -q "^### $PLUGIN_NAME$" PLUGINS.md; then
  echo "❌ Plugin '$PLUGIN_NAME' not found in PLUGINS.md"
  exit 1
fi
```

### 2. Block If In Development

```bash
# Check status
STATUS=$(grep -A2 "^### $PLUGIN_NAME$" PLUGINS.md | grep "Status:" | grep -o "🚧")

if [ "$STATUS" = "🚧" ]; then
  echo "❌ Cannot destroy plugin in development (Status: 🚧)"
  echo "Complete or abandon workflow first with /continue"
  exit 1
fi
```

### 3. Gather Information

```bash
# Find all components
SOURCE_DIR="plugins/$PLUGIN_NAME"
BUILD_DIR="build/plugins/$PLUGIN_NAME"
PRODUCT_NAME=$(grep "PRODUCT_NAME" $SOURCE_DIR/CMakeLists.txt | sed 's/.*"\(.*\)".*/\1/')

VST3_PATH="$HOME/Library/Audio/Plug-Ins/VST3/${PRODUCT_NAME}.vst3"
AU_PATH="$HOME/Library/Audio/Plug-Ins/Components/${PRODUCT_NAME}.component"

# Calculate sizes
SOURCE_SIZE=$(du -sh "$SOURCE_DIR" 2>/dev/null | awk '{print $1}')
BUILD_SIZE=$(du -sh "$BUILD_DIR" 2>/dev/null | awk '{print $1}')
VST3_SIZE=$(du -sh "$VST3_PATH" 2>/dev/null | awk '{print $1}')
AU_SIZE=$(du -sh "$AU_PATH" 2>/dev/null | awk '{print $1}')
```

### 4. Present Confirmation Prompt

Show sizes, list what will be deleted, require exact name match.

### 5. Create Backup

```bash
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
BACKUP_DIR="backups/destroyed"
BACKUP_FILE="${BACKUP_DIR}/${PLUGIN_NAME}_${TIMESTAMP}.tar.gz"

mkdir -p "$BACKUP_DIR"

# Create archive
tar -czf "$BACKUP_FILE" \
  "$SOURCE_DIR" \
  --transform "s|^|${PLUGIN_NAME}/|"

# Add PLUGINS.md entry
ENTRY=$(awk "/^### $PLUGIN_NAME$/,/^###/" PLUGINS.md | head -n -1)
echo "$ENTRY" > "${BACKUP_DIR}/${PLUGIN_NAME}_entry.md"

# Verify backup
if [ -f "$BACKUP_FILE" ]; then
  BACKUP_SIZE=$(du -sh "$BACKUP_FILE" | awk '{print $1}')
  echo "✓ Backup created: $BACKUP_FILE ($BACKUP_SIZE)"
else
  echo "❌ Backup failed. Aborting."
  exit 1
fi
```

### 6. Remove Installed Binaries

```bash
# Uninstall if installed
if [ -d "$VST3_PATH" ] || [ -f "$AU_PATH" ]; then
  # Use plugin-lifecycle uninstall logic
  rm -rf "$VST3_PATH" 2>/dev/null
  rm -rf "$AU_PATH" 2>/dev/null

  # Clear DAW caches
  rm -rf ~/Library/Caches/Ableton/*/PluginCache* 2>/dev/null
  rm -rf ~/Library/Caches/AudioUnitCache* 2>/dev/null

  echo "✓ Binaries removed and caches cleared"
fi
```

### 7. Remove Build Artifacts

```bash
if [ -d "$BUILD_DIR" ]; then
  rm -rf "$BUILD_DIR"
  echo "✓ Build artifacts removed"
fi
```

### 8. Remove Source Code

```bash
if [ -d "$SOURCE_DIR" ]; then
  rm -rf "$SOURCE_DIR"
  echo "✓ Source code removed: $SOURCE_DIR"
fi
```

### 9. Remove PLUGINS.md Entry

```bash
# Delete entry (from ### PluginName to next ### or EOF)
awk "BEGIN {del=0} /^### $PLUGIN_NAME$/ {del=1; next} /^###/ {del=0} !del" PLUGINS.md > PLUGINS.md.tmp
mv PLUGINS.md.tmp PLUGINS.md

echo "✓ PLUGINS.md entry removed"
```

### 10. Optional: Clean Troubleshooting Docs

```bash
# Find docs mentioning this plugin
DOCS=$(grep -rl "plugin: $PLUGIN_NAME" troubleshooting/)

if [ -n "$DOCS" ]; then
  echo ""
  echo "Found troubleshooting docs mentioning $PLUGIN_NAME:"
  echo "$DOCS" | while read doc; do
    echo "  - $doc"
  done

  read -p "Delete these docs? (y/N): " DELETE_DOCS

  if [ "$DELETE_DOCS" = "y" ]; then
    echo "$DOCS" | xargs rm
    echo "✓ Troubleshooting docs removed"
  else
    echo "○ Troubleshooting docs preserved"
  fi
fi
```

### 11. Success Report

```
✓ [PluginName] DESTROYED

Removed:
- Source code: plugins/[PluginName]/ (3.2 MB)
- VST3: ~/Library/Audio/Plug-Ins/VST3/[ProductName].vst3 (4.1 MB)
- AU: ~/Library/Audio/Plug-Ins/Components/[ProductName].component (4.0 MB)
- Build artifacts: build/plugins/[PluginName]/ (45 MB)
- PLUGINS.md entry

Backup available at:
backups/destroyed/[PluginName]_20251110_234512.tar.gz (3.5 MB)

To restore:
1. Extract: tar -xzf backups/destroyed/[PluginName]_20251110_234512.tar.gz
2. Move to plugins/: mv [PluginName] plugins/
3. Restore PLUGINS.md entry from backups/destroyed/[PluginName]_entry.md
4. Rebuild: /continue [PluginName]
```

## Recovery Instructions

If you need to restore a destroyed plugin:

```bash
# List backups
ls -lh backups/destroyed/

# Extract backup
cd plugins/
tar -xzf ../backups/destroyed/[PluginName]_TIMESTAMP.tar.gz

# Restore PLUGINS.md entry (manually copy from *_entry.md)

# Resume development
/continue [PluginName]
```

## Error Handling

**Plugin not found:**
```
❌ Plugin 'Foo' not found in PLUGINS.md

Available plugins:
- GainKnob
- TapeAge
- ClapMachine
```

**In development (Status: 🚧):**
```
❌ Cannot destroy plugin in development (Status: 🚧 Stage 4)

Complete the workflow first:
- /continue [PluginName] - Resume and finish
- Or manually set status in PLUGINS.md if workflow abandoned
```

**Confirmation mismatch:**
```
Type the plugin name to confirm: FooBar
❌ Plugin name doesn't match. Aborting. (expected: Foo)
```

**Backup creation failed:**
```
❌ Backup creation failed

Cannot proceed without backup. Check disk space and permissions.
```

## Integration Points

**Invoked by:**
- `/destroy [PluginName]`
- User phrases: "completely remove", "delete everything", "annihilate"

**Routes to:**
- None (terminal operation)

**Similar commands:**
- `/uninstall` - Removes binaries only, keeps source code
- `/reset-to-ideation` - Removes implementation, keeps idea/mockups
- This command - Removes EVERYTHING

## Use Cases

**When to use `/destroy`:**
- ✅ Abandoned experiment, never going back
- ✅ Plugin concept was fundamentally flawed
- ✅ Made duplicate by mistake
- ✅ Cleaning up workspace, sure you won't need it

**When to use `/reset-to-ideation` instead:**
- ❌ Implementation went wrong, but idea is good
- ❌ Want to try different approach with same concept
- ❌ Mockup is solid, just need to redo code

**When to use `/uninstall` instead:**
- ❌ Just want to remove from DAW temporarily
- ❌ Need to rebuild from source
- ❌ Testing different versions

## Notes for Claude

**When executing this command:**

1. ALWAYS create backup first - NEVER delete without backup
2. Require exact name confirmation - typos could be catastrophic
3. Block if status is 🚧 - protect in-progress work
4. Show sizes before deletion - help user understand impact
5. Preserve troubleshooting docs by default (optional deletion)
6. Include recovery instructions in success message
7. Be thorough - check all possible locations

**Common pitfalls:**

- Forgetting to check for installed binaries
- Not clearing DAW caches (leaves ghost entries)
- Deleting without backup
- Not validating backup before proceeding
- Missing build artifacts in other locations
