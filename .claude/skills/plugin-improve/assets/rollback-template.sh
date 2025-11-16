#!/bin/bash
# Rollback plugin to previous version from backup

PLUGIN_NAME="$1"
ROLLBACK_VERSION="${2#v}"  # Strip leading "v" if present

BACKUP_PATH="backups/${PLUGIN_NAME}/v${ROLLBACK_VERSION}/"

if [ ! -d "$BACKUP_PATH" ]; then
  echo "❌ Backup not found: $BACKUP_PATH"
  exit 1
fi

echo "Rolling back ${PLUGIN_NAME} to v${ROLLBACK_VERSION}..."

# Remove current source
rm -rf "plugins/${PLUGIN_NAME}/"

# Restore from backup
cp -r "$BACKUP_PATH" "plugins/${PLUGIN_NAME}/"

echo "✓ Rollback complete: ${PLUGIN_NAME} restored to v${ROLLBACK_VERSION}"
