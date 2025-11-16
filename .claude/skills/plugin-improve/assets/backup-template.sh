#!/bin/bash
# Backup plugin source to timestamped directory (excludes build artifacts)

PLUGIN_NAME="$1"
CURRENT_VERSION="${2#v}"  # Strip leading "v" if present

mkdir -p "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"

# Use rsync to exclude build artifacts and other generated files
rsync -a \
  --exclude='build/' \
  --exclude='build.log' \
  --exclude='.DS_Store' \
  --exclude='*.user' \
  --exclude='.cache/' \
  "plugins/${PLUGIN_NAME}/" "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"

echo "✓ Backup created: backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
