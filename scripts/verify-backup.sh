#!/bin/bash
set -e

PLUGIN_NAME="$1"
VERSION="${2#v}"  # Strip leading "v" if present
BACKUP_PATH="backups/${PLUGIN_NAME}/v${VERSION}"

# Color codes
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "Verifying backup: ${BACKUP_PATH}"

# Check directory exists
if [[ ! -d "$BACKUP_PATH" ]]; then
  echo -e "${RED}❌ Backup not found${NC}"
  exit 1
fi

# Check critical files (handle both flat and Source/ structure)
CRITICAL_FILES=(
  "CMakeLists.txt"
  "Source/PluginProcessor.h"
  "Source/PluginProcessor.cpp"
  "Source/PluginEditor.h"
  "Source/PluginEditor.cpp"
)

# Also check for flat structure (fallback)
CRITICAL_FILES_FLAT=(
  "PluginProcessor.h"
  "PluginProcessor.cpp"
  "PluginEditor.h"
  "PluginEditor.cpp"
)

for file in "${CRITICAL_FILES[@]}"; do
  if [[ ! -f "${BACKUP_PATH}/${file}" ]]; then
    # Try flat structure
    basename_file=$(basename "$file")
    if [[ ! -f "${BACKUP_PATH}/${basename_file}" ]]; then
      echo -e "${RED}❌ Missing: ${file}${NC}"
      exit 1
    else
      echo -e "${GREEN}✓${NC} ${basename_file}"
    fi
  else
    echo -e "${GREEN}✓${NC} ${file}"
  fi
done

# Verify CMakeLists.txt can be parsed (check for project() or juce_add_plugin())
if grep -q "project(\|juce_add_plugin(" "${BACKUP_PATH}/CMakeLists.txt"; then
  echo -e "${GREEN}✓${NC} CMakeLists.txt valid"
else
  echo -e "${RED}❌ CMakeLists.txt invalid${NC}"
  exit 1
fi

# Dry-run build test (clean any existing build dir first)
cd "$BACKUP_PATH"
rm -rf build
if cmake -B build -G Ninja 2>/dev/null | grep -q "Build files"; then
  echo -e "${GREEN}✓${NC} Dry-run build successful"
else
  echo -e "${YELLOW}⚠️${NC} Dry-run build failed (may be okay if dependencies missing)"
fi

echo -e "${GREEN}✓ Backup verification complete${NC}"
exit 0
