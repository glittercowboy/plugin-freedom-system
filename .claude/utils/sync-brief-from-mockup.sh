#!/usr/bin/env bash
set -euo pipefail

# sync-brief-from-mockup.sh
# Auto-updates creative-brief.md from finalized mockup
# Preserves conceptual sections, updates UI-specific sections

PLUGIN_NAME="${1:-}"
MOCKUP_VERSION="${2:-}"

if [[ -z "$PLUGIN_NAME" ]]; then
  echo "Error: Plugin name required"
  echo "Usage: sync-brief-from-mockup.sh <plugin-name> [mockup-version]"
  exit 1
fi

PLUGIN_DIR="/Users/lexchristopherson/Developer/plugin-freedom-system/plugins/${PLUGIN_NAME}"
IDEAS_DIR="${PLUGIN_DIR}/.ideas"
MOCKUPS_DIR="${IDEAS_DIR}/mockups"
BRIEF_FILE="${IDEAS_DIR}/creative-brief.md"
PARAM_SPEC_FILE="${IDEAS_DIR}/parameter-spec.md"

# Check if creative brief exists
if [[ ! -f "$BRIEF_FILE" ]]; then
  echo "Standalone mode: No creative brief to update"
  exit 0
fi

# Find latest mockup version if not specified
if [[ -z "$MOCKUP_VERSION" ]]; then
  LATEST_YAML=$(find "$MOCKUPS_DIR" -name "v*-ui.yaml" 2>/dev/null | sort -V | tail -n 1)
  if [[ -z "$LATEST_YAML" ]]; then
    echo "Error: No mockup YAML files found in $MOCKUPS_DIR"
    exit 1
  fi
  MOCKUP_VERSION=$(basename "$LATEST_YAML" | sed 's/v\([0-9]*\)-.*/\1/')
fi

MOCKUP_YAML="${MOCKUPS_DIR}/v${MOCKUP_VERSION}-ui.yaml"

# Verify files exist
if [[ ! -f "$MOCKUP_YAML" ]]; then
  echo "Error: Mockup file not found: $MOCKUP_YAML"
  exit 1
fi

if [[ ! -f "$PARAM_SPEC_FILE" ]]; then
  echo "Warning: parameter-spec.md not found, will skip Parameters section update"
  SKIP_PARAMS=true
else
  SKIP_PARAMS=false
fi

echo "Syncing creative brief from mockup v${MOCKUP_VERSION}..."

# Create temporary files for parsing
TEMP_DIR=$(mktemp -d)
trap 'rm -rf "$TEMP_DIR"' EXIT

PRESERVED_FILE="${TEMP_DIR}/preserved.md"
NEW_BRIEF_FILE="${TEMP_DIR}/new-brief.md"

# Parse current brief and preserve conceptual sections
python3 <<'PYTHON_SCRIPT' - "$BRIEF_FILE" "$PRESERVED_FILE"
import sys
import re

brief_file = sys.argv[1]
output_file = sys.argv[2]

# Sections to preserve (conceptual content)
preserve_sections = ["Overview", "Vision", "Use Cases", "Inspirations", "Technical Notes"]

# Read brief
with open(brief_file, 'r') as f:
    content = f.read()

# Extract title
title_match = re.match(r'^#\s+(.+?)(?:\s*-\s*Creative Brief)?\s*$', content, re.MULTILINE)
title = title_match.group(1) if title_match else "Unknown Plugin"

# Parse sections
sections = {}
current_section = None
current_content = []

for line in content.split('\n'):
    if line.startswith('## '):
        # Save previous section
        if current_section:
            sections[current_section] = '\n'.join(current_content)
        # Start new section
        current_section = line[3:].strip()
        current_content = []
    elif current_section:
        current_content.append(line)

# Save last section
if current_section:
    sections[current_section] = '\n'.join(current_content)

# Write preserved sections to output
preserved_data = {
    'title': title,
    'sections': {k: v for k, v in sections.items() if k in preserve_sections}
}

import json
with open(output_file, 'w') as f:
    json.dump(preserved_data, f, indent=2)

print(f"Preserved {len(preserved_data['sections'])} sections: {', '.join(preserved_data['sections'].keys())}", file=sys.stderr)
PYTHON_SCRIPT

# Extract mockup metadata
echo "Extracting mockup metadata..."
AESTHETIC=$(python3 -c "import yaml; data = yaml.safe_load(open('$MOCKUP_YAML')); print(data.get('design_system', {}).get('aesthetic', 'Not specified'))" 2>/dev/null || echo "Not specified")
WIDTH=$(python3 -c "import yaml; data = yaml.safe_load(open('$MOCKUP_YAML')); print(data.get('layout', {}).get('dimensions', {}).get('width', 800))" 2>/dev/null || echo "800")
HEIGHT=$(python3 -c "import yaml; data = yaml.safe_load(open('$MOCKUP_YAML')); print(data.get('layout', {}).get('dimensions', {}).get('height', 600))" 2>/dev/null || echo "600")
LAYOUT_TYPE=$(python3 -c "import yaml; data = yaml.safe_load(open('$MOCKUP_YAML')); print(data.get('layout', {}).get('type', 'grid'))" 2>/dev/null || echo "grid")

# Extract color scheme
COLORS=$(python3 -c "
import yaml
data = yaml.safe_load(open('$MOCKUP_YAML'))
colors = data.get('design_system', {}).get('colors', {})
if colors:
    print(', '.join([f'{k}: {v}' for k, v in colors.items()]))
else:
    print('Not specified')
" 2>/dev/null || echo "Not specified")

# Extract features
FEATURES=$(python3 -c "
import yaml
data = yaml.safe_load(open('$MOCKUP_YAML'))
features = data.get('features', {})
if features:
    for key, val in features.items():
        if val:
            print(f'- {key.replace(\"_\", \" \").title()}')
" 2>/dev/null || echo "")

# Count controls
CONTROL_COUNT=$(python3 -c "
import yaml
data = yaml.safe_load(open('$MOCKUP_YAML'))
controls = data.get('layout', {}).get('controls', [])
print(len(controls))
" 2>/dev/null || echo "0")

# Generate new brief
python3 <<'PYTHON_SCRIPT' - "$PRESERVED_FILE" "$PARAM_SPEC_FILE" "$NEW_BRIEF_FILE" "$SKIP_PARAMS" "$AESTHETIC" "$WIDTH" "$HEIGHT" "$LAYOUT_TYPE" "$COLORS" "$FEATURES" "$CONTROL_COUNT"
import sys
import json

preserved_file = sys.argv[1]
param_spec_file = sys.argv[2]
output_file = sys.argv[3]
skip_params = sys.argv[4] == 'true'
aesthetic = sys.argv[5]
width = sys.argv[6]
height = sys.argv[7]
layout_type = sys.argv[8]
colors = sys.argv[9]
features = sys.argv[10]
control_count = sys.argv[11]

# Load preserved data
with open(preserved_file, 'r') as f:
    preserved = json.load(f)

title = preserved['title']
sections = preserved['sections']

# Start building new brief
output = []
output.append(f"# {title} - Creative Brief\n")

# Overview section (preserve if exists)
if 'Overview' in sections:
    output.append("## Overview\n")
    output.append(sections['Overview'])
    output.append("\n")

# Vision section (preserve)
if 'Vision' in sections:
    output.append("## Vision\n")
    output.append(sections['Vision'])
    output.append("\n")

# Parameters section (UPDATE from parameter-spec.md)
if not skip_params:
    output.append("## Parameters\n")
    with open(param_spec_file, 'r') as f:
        param_content = f.read()

    # Extract just the parameters section content (skip title if present)
    lines = param_content.split('\n')
    in_params = False
    param_lines = []

    for line in lines:
        if line.startswith('## Parameters'):
            in_params = True
            continue
        elif line.startswith('# ') or line.startswith('## '):
            if in_params:
                break
        elif in_params or (not line.startswith('#') and line.strip()):
            param_lines.append(line)

    # If we found parameter content, use it
    if param_lines:
        output.append('\n'.join(param_lines))
    else:
        # Otherwise use entire param spec
        output.append(param_content)

    output.append("\n")

# UI Concept section (UPDATE from mockup)
output.append("## UI Concept\n")
output.append(f"\n**Layout:** {layout_type.replace('_', ' ').title()} layout with {control_count} controls\n")
output.append(f"\n**Visual Style:** {aesthetic}\n")
output.append(f"\n**Color Scheme:** {colors}\n")
output.append(f"\n**Dimensions:** {width}×{height}px\n")

if features.strip():
    output.append("\n**Key Elements:**\n")
    output.append(features)
    output.append("\n")

# Use Cases section (preserve)
if 'Use Cases' in sections:
    output.append("## Use Cases\n")
    output.append(sections['Use Cases'])
    output.append("\n")

# Inspirations section (preserve)
if 'Inspirations' in sections:
    output.append("## Inspirations\n")
    output.append(sections['Inspirations'])
    output.append("\n")

# Technical Notes section (preserve)
if 'Technical Notes' in sections:
    output.append("## Technical Notes\n")
    output.append(sections['Technical Notes'])
    output.append("\n")

# Next Steps section (auto-generate)
output.append("## Next Steps\n")
output.append("\n- [ ] Complete mockup finalization\n")
output.append("- [ ] Run /implement to begin Stage 2 (Foundation)\n")
output.append("- [ ] Proceed through workflow stages\n")
output.append("- [ ] Test and validate plugin\n")

# Write output
with open(output_file, 'w') as f:
    f.write(''.join(output))

print(f"Generated new brief: {output_file}", file=sys.stderr)
PYTHON_SCRIPT

# Replace original brief
cp "$NEW_BRIEF_FILE" "$BRIEF_FILE"

echo "✓ Creative brief updated from mockup v${MOCKUP_VERSION}"
echo ""
echo "Updated sections:"
if [[ "$SKIP_PARAMS" == "false" ]]; then
  PARAM_COUNT=$(grep -c '^###' "$PARAM_SPEC_FILE" 2>/dev/null || echo "0")
  echo "- Parameters (${PARAM_COUNT} parameters from parameter-spec.md)"
fi
echo "- UI Concept (layout, visual style, dimensions from mockup)"
echo ""
echo "Preserved sections:"
python3 -c "
import json
with open('$PRESERVED_FILE') as f:
    data = json.load(f)
    for section in data['sections'].keys():
        print(f'- {section} (original content intact)')
"

exit 0
