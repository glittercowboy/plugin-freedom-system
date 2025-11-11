---
name: ui-template-library
description: Manage aesthetic templates - save visual systems from completed mockups, apply to new plugins with adaptive layouts
allowed-tools:
  - Read
  - Write
  - Bash
preconditions:
  - .claude/aesthetics/ directory exists
---

# ui-template-library Skill

**Purpose:** Capture and apply aesthetic "vibes" across plugins using structured prose descriptions rather than rigid specifications.

## Core Philosophy: Conceptual Aesthetics

**Aesthetic = Transferable design language captured as interpretable prose**

This system captures the **conceptual essence** of a design - the "vibe," color philosophy, control styling approach, spacing feel - in structured markdown that Claude can reliably interpret and apply to plugins with different parameter counts.

**Why prose instead of specs:**
- Aesthetics are conceptual, not rigid pixel-perfect templates
- Descriptions adapt naturally to different layouts and parameter counts
- Human-editable and machine-interpretable without browser APIs
- Works entirely with Read/Write/Bash tools (no DOM parsing required)

**Example:**
- **Save aesthetic** from 3-knob TapeAge: "Warm vintage aesthetic with brass rotary knobs, subtle paper texture, and generous spacing"
- **Apply aesthetic** to 8-parameter reverb: Same brass knobs, same paper texture, same spacing philosophy, but in a 3x3 grid layout appropriate for 8 parameters

The aesthetic system separates **visual design language** (colors, control styling, spacing philosophy) from **layout structure** (arrangement, grid, parameter count).

---

## Operations

1. **Save aesthetic** - Analyze mockup HTML and generate structured prose aesthetic.md
2. **Apply aesthetic** - Interpret aesthetic.md to generate new mockup with appropriate layout
3. **List aesthetics** - Show all saved aesthetics with prose summaries
4. **Delete aesthetic** - Remove aesthetic from library
5. **Update aesthetic** - Refine aesthetic.md from improved mockup

---

## Directory Structure

```
.claude/aesthetics/
├── manifest.json                    # Registry of all aesthetics
└── [aesthetic-id]/
    ├── aesthetic.md                 # Structured prose description (THE SOURCE OF TRUTH)
    ├── preview.html                 # Visual reference (original mockup)
    └── metadata.json                # Name, description, source plugin, tags
```

**Key change from previous system:**
- **aesthetic.yaml → aesthetic.md**: YAML specs replaced with structured prose descriptions
- Format is consistent and parseable, but content is interpretable concepts, not rigid values

---

## Operation 1: Save Aesthetic

**Input:** Path to finalized mockup HTML
**Output:** New aesthetic in `.claude/aesthetics/[aesthetic-id]/`

**High-level process:**
1. Read and analyze mockup HTML using string/pattern matching
2. Extract visual patterns (colors, fonts, control types, spacing)
3. Generate prose descriptions for each aesthetic section
4. Write structured aesthetic.md following template
5. Copy preview.html and generate metadata.json
6. Update manifest.json

### Step 1: Read Mockup HTML

```bash
MOCKUP_PATH="plugins/$PLUGIN_NAME/.ideas/mockups/v$N-ui.html"
test -f "$MOCKUP_PATH" || { echo "Mockup not found"; exit 1; }
```

**Use Read tool to load HTML content into memory for analysis.**

### Step 2: Extract Visual Patterns

**Use pattern matching (grep/sed) and string analysis to extract:**

**Colors:**
```bash
# Extract hex colors from CSS
grep -oE '#[0-9a-fA-F]{3,6}' "$MOCKUP_PATH" | sort -u

# Extract rgba colors
grep -oE 'rgba?\([^)]+\)' "$MOCKUP_PATH" | sort -u

# Extract CSS custom properties
grep -oE '--[a-zA-Z-]+:\s*[^;]+' "$MOCKUP_PATH"
```

**Fonts:**
```bash
# Extract font-family declarations
grep -oE "font-family:\s*[^;]+" "$MOCKUP_PATH" | \
    sed "s/font-family:\s*//" | \
    sort -u
```

**Control types:**
```bash
# Detect rotary knobs
grep -q "rotary\|knob" "$MOCKUP_PATH" && echo "Has rotary knobs"

# Detect sliders
grep -q "slider\|range" "$MOCKUP_PATH" && echo "Has sliders"

# Detect buttons
grep -q "button\|toggle" "$MOCKUP_PATH" && echo "Has buttons"
```

**Spacing values:**
```bash
# Extract gap, padding, margin values
grep -oE "(gap|padding|margin):\s*[0-9]+px" "$MOCKUP_PATH" | \
    awk -F': ' '{print $2}' | \
    sort -n
```

**See:** `references/pattern-extraction.md` for complete extraction strategies

### Step 3: Generate Prose Descriptions

**Transform extracted patterns into interpretable prose:**

**Example color extraction → prose:**
```
Extracted: #ff6b35, #2b2015, #f5e6d3
↓
Prose: "Warm vintage palette with burnt orange accent (#ff6b35) against
       dark brown background (#2b2015). Cream text (#f5e6d3) provides
       high contrast. Overall feel is retro/analog hardware."
```

**Example spacing extraction → prose:**
```
Extracted: gap: 32px, padding: 24px, margin: 16px
↓
Prose: "Generous spacing philosophy. Controls separated by 32px creating
       comfortable breathing room. Section padding of 24px maintains visual
       hierarchy. Not cluttered - each element has space to be appreciated."
```

**Example control detection → prose:**
```
Detected: .rotary-knob class, border-radius: 50%, tick marks
↓
Prose: "Rotary knobs with prominent tick marks around perimeter. Circular
       shape (border-radius: 50%) with ~70px diameter. Brass/orange coloring
       with center dot indicator. Style: vintage hardware, tactile feel."
```

**See:** `references/prose-generation.md` for detailed generation guidelines

### Step 4: Write Structured aesthetic.md

**Load template:**
```bash
TEMPLATE=".claude/skills/ui-template-library/assets/aesthetic-template.md"
```

**Fill in all sections using generated prose:**

1. **Visual Identity** - Overall vibe summary (2-3 sentences)
2. **Color System** - Primary palette, control colors, philosophy
3. **Typography** - Font families, sizing, styling, philosophy
4. **Controls** - Knob style, slider style, button style (detailed descriptions)
5. **Spacing & Layout Philosophy** - Density, gaps, adaptation strategy
6. **Surface Treatment** - Textures, depth, shadows, borders
7. **Details & Embellishments** - Special features, interactions
8. **Technical Patterns** - Border radius, transitions, layout techniques
9. **Interaction Feel** - Responsiveness, feedback, tactility
10. **Best Suited For** - Plugin types, design contexts
11. **Application Guidelines** - Parameter count adaptation, customization points
12. **Example Color Codes** - Concrete CSS values as reference

**Critical: Follow template structure exactly. Same sections, same order, every time.**

**Write aesthetic.md:**
```bash
AESTHETIC_ID=$(generate_aesthetic_id "$AESTHETIC_NAME")
AESTHETIC_DIR=".claude/aesthetics/$AESTHETIC_ID"
mkdir -p "$AESTHETIC_DIR"

# Fill template with generated prose
cat "$TEMPLATE" | \
    sed "s/\[Aesthetic Name\]/$AESTHETIC_NAME/" | \
    # ... more replacements with generated prose ...
    > "$AESTHETIC_DIR/aesthetic.md"
```

### Step 5: Copy Preview HTML

```bash
cp "$MOCKUP_PATH" "$AESTHETIC_DIR/preview.html"
```

This provides visual reference alongside prose description.

### Step 6: Generate Metadata

**Create metadata.json:**
```json
{
  "id": "vintage-hardware-001",
  "name": "Vintage Hardware",
  "description": "Warm, retro, skeuomorphic design with brass controls and textured background",
  "sourcePlugin": "TapeAge",
  "sourceVersion": "v2",
  "created": "2025-11-11T10:30:00Z",
  "usedInPlugins": ["TapeAge"],
  "tags": ["vintage", "warm", "skeuomorphic", "brass", "retro"]
}
```

**Aesthetic ID generation:**
- Slugify name: "Vintage Hardware" → "vintage-hardware"
- Append counter: "vintage-hardware-001"
- Increment if ID already exists

**Tag inference from prose:**
- Analyze aesthetic.md content for keywords
- Extract style descriptors: vintage, modern, minimal, aggressive, etc.
- Extract color descriptors: warm, cool, dark, bright, etc.
- Extract feel descriptors: tactile, smooth, technical, playful, etc.

### Step 7: Update Manifest

**Read existing manifest:**
```bash
MANIFEST=".claude/aesthetics/manifest.json"
```

**Append new aesthetic entry:**
```json
{
  "version": "1.0",
  "aesthetics": [
    {
      "id": "vintage-hardware-001",
      "name": "Vintage Hardware",
      "description": "Warm, retro, skeuomorphic design",
      "sourcePlugin": "TapeAge",
      "created": "2025-11-11T10:30:00Z",
      "tags": ["vintage", "warm", "skeuomorphic"]
    }
  ]
}
```

**Write updated manifest using Read → modify → Write pattern.**

### Step 8: Confirmation

```
✅ Aesthetic saved: "Vintage Hardware" (vintage-hardware-001)

Files created:
- .claude/aesthetics/vintage-hardware-001/aesthetic.md (prose description)
- .claude/aesthetics/vintage-hardware-001/preview.html (visual reference)
- .claude/aesthetics/vintage-hardware-001/metadata.json (metadata)

Preview: open .claude/aesthetics/vintage-hardware-001/preview.html

You can now apply this aesthetic to new plugins:
- From ui-mockup skill: Choose "Start from aesthetic template"
- Or manually invoke: ui-template-library with "apply" operation
```

---

## Operation 2: Apply Aesthetic

**Input:**
- Aesthetic ID
- Target plugin name
- Parameter spec (count, types, IDs)

**Output:** Generated mockup HTML with aesthetic applied + appropriate layout

**High-level process:**
1. Read aesthetic.md (structured prose)
2. Read target plugin parameter-spec.md or creative-brief.md
3. Interpret aesthetic prose to extract design rules
4. Choose layout based on parameter count
5. Generate HTML/CSS applying aesthetic rules to layout
6. Save generated mockup to target plugin

### Step 1: Load Aesthetic

```bash
AESTHETIC_DIR=".claude/aesthetics/$AESTHETIC_ID"
AESTHETIC_MD="$AESTHETIC_DIR/aesthetic.md"
METADATA_JSON="$AESTHETIC_DIR/metadata.json"

test -f "$AESTHETIC_MD" || { echo "Aesthetic not found"; exit 1; }
```

**Use Read tool to load aesthetic.md and metadata.json.**

### Step 2: Read Target Plugin Spec

```bash
PLUGIN_DIR="plugins/$PLUGIN_NAME"
PARAM_SPEC="$PLUGIN_DIR/.ideas/parameter-spec.md"
BRIEF="$PLUGIN_DIR/.ideas/creative-brief.md"

# Try parameter-spec.md first, fall back to creative-brief.md
if [ -f "$PARAM_SPEC" ]; then
    SOURCE="$PARAM_SPEC"
else
    SOURCE="$BRIEF"
fi
```

**Extract from spec:**
- Parameter count
- Parameter IDs and names
- Parameter types (FLOAT, BOOL, CHOICE)
- Default values and ranges
- Prominent parameters (Mix, Output, Dry/Wet)

**Example parsed parameters:**
```javascript
const parameters = [
    { id: 'GAIN', name: 'Gain', type: 'FLOAT', prominent: false },
    { id: 'TONE', name: 'Tone', type: 'FLOAT', prominent: false },
    { id: 'MIX', name: 'Mix', type: 'FLOAT', prominent: true },
    { id: 'BYPASS', name: 'Bypass', type: 'BOOL', prominent: false },
];
```

### Step 3: Interpret Aesthetic Prose

**Parse aesthetic.md sections to extract design decisions:**

**Color System section:**
- Parse "Background Colors" prose → extract color values and usage contexts
- Parse "Accent Colors" prose → extract accent hierarchy
- Parse "Philosophy" → understand warm/cool, contrast, mood

**Controls section:**
- Parse "Knob Style" prose → understand shape, size, indicator style
- Parse "Slider Style" prose → understand orientation, thumb design
- Parse "Button Style" prose → understand shape, states

**Spacing section:**
- Parse "Overall Density" prose → choose tight/comfortable/generous gaps
- Parse "Control Spacing" prose → extract pixel values or relative approach

**Typography section:**
- Parse "Font Families" prose → extract font stacks or fallbacks
- Parse "Font Sizing" prose → extract size hierarchy

**See:** `references/aesthetic-interpretation.md` for detailed interpretation strategies

### Step 4: Choose Appropriate Layout

**Decision tree based on parameter count:**

```javascript
function chooseLayout(paramCount) {
    if (paramCount <= 3) {
        return 'horizontal-row';  // Single row
    } else if (paramCount <= 6) {
        return 'grid-2x3';  // 2 rows, up to 3 columns
    } else if (paramCount <= 9) {
        return 'grid-3x3';  // 3x3 grid
    } else if (paramCount <= 12) {
        return 'grid-4x3';  // 4 rows, 3 columns
    } else {
        return 'scrollable-grid';  // Larger grid with scrolling
    }
}

const layout = chooseLayout(parameters.length);
```

**Separate prominent parameters:**
```javascript
const prominentParams = parameters.filter(isProminentParameter);
const regularParams = parameters.filter(p => !isProminentParameter(p));
```

**Layout modifications for prominent params:**
- If 1-2 prominent params exist: Place as side sliders or bottom row
- If 3+ prominent params: Integrate into main grid with visual distinction

**Consult aesthetic.md "Application Guidelines" section:**
- Check "Parameter Count Adaptation" subsection for layout preferences
- Check "Prominent Parameter Handling" for visual distinction strategy

### Step 5: Generate HTML with Aesthetic

**Strategy: Template-based generation with aesthetic insertions**

**Base structure:**
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{{PLUGIN_NAME}}</title>
    <style>
        /* Apply aesthetic color system */
        :root {
            {{INSERT_CSS_VARIABLES_FROM_AESTHETIC}}
        }

        html, body {
            margin: 0;
            padding: 0;
            height: 100%;  /* NOT 100vh - WebView constraint */
            background: var(--bg-main);
            color: var(--text-primary);
            font-family: {{FONT_FAMILY_FROM_AESTHETIC}};
            user-select: none;  /* Native feel */
        }

        /* Apply layout based on parameter count */
        .controls {
            {{GENERATE_LAYOUT_CSS}}
        }

        /* Apply control styling from aesthetic */
        {{GENERATE_KNOB_CSS_FROM_AESTHETIC}}
        {{GENERATE_SLIDER_CSS_FROM_AESTHETIC}}
        {{GENERATE_BUTTON_CSS_FROM_AESTHETIC}}
    </style>
</head>
<body>
    <h1>{{PLUGIN_NAME}}</h1>

    <div class="controls">
        {{FOR_EACH_PARAMETER_GENERATE_CONTROL_HTML}}
    </div>

    <script>
        {{GENERATE_PARAMETER_BINDING_JS}}
    </script>
</body>
</html>
```

**CSS variable generation from aesthetic prose:**
```javascript
// Parse "Example Color Codes" section from aesthetic.md
// Extract CSS variable definitions or generate from prose descriptions

const cssVars = `
    --bg-main: ${extractColor(aesthetic, 'main background')};
    --bg-surface: ${extractColor(aesthetic, 'surface background')};
    --accent-primary: ${extractColor(aesthetic, 'primary accent')};
    --accent-hover: ${extractColor(aesthetic, 'hover state')};
    --text-primary: ${extractColor(aesthetic, 'primary text')};
    --text-secondary: ${extractColor(aesthetic, 'secondary text')};
    --control-base: ${extractColor(aesthetic, 'control base')};
    --control-active: ${extractColor(aesthetic, 'control active')};
`;
```

**Knob CSS generation from aesthetic prose:**
```javascript
// Parse "Knob Style" section from aesthetic.md
// Extract: size, border style, shadow, indicator type

function generateKnobCSS(aestheticKnobSection) {
    // Extract size description: "medium (70px)" → 70px
    const size = extractSize(aestheticKnobSection) || '70px';

    // Extract shape: "Circle" → border-radius: 50%
    const borderRadius = aestheticKnobSection.includes('circle') ? '50%' : '8px';

    // Extract shadow from "Surface Treatment" section
    const shadow = extractShadowStyle(aesthetic) || 'none';

    return `
        .rotary-knob {
            width: ${size};
            height: ${size};
            border-radius: ${borderRadius};
            border: 2px solid var(--control-base);
            box-shadow: ${shadow};
            background: var(--control-base);
        }
    `;
}
```

**Layout CSS generation from parameter count + aesthetic spacing:**
```javascript
function generateLayoutCSS(layout, aesthetic) {
    // Extract spacing values from "Spacing & Layout Philosophy" section
    const gap = extractSpacingValue(aesthetic, 'between controls') || '24px';

    if (layout === 'horizontal-row') {
        return `
            display: flex;
            flex-direction: row;
            gap: ${gap};
            justify-content: center;
            align-items: center;
        `;
    } else if (layout === 'grid-2x3') {
        return `
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            grid-template-rows: repeat(2, auto);
            gap: ${gap};
        `;
    }
    // ... more layouts
}
```

**Control HTML generation per parameter:**
```javascript
function generateControlHTML(param, aesthetic) {
    // Determine control type from parameter type + aesthetic preferences
    const controlType = determineControlType(param, aesthetic);

    if (controlType === 'rotary-knob') {
        return `
            <div class="control-wrapper">
                <div class="rotary-knob" data-param="${param.id}">
                    <div class="knob-indicator"></div>
                </div>
                <label>${param.name}</label>
            </div>
        `;
    }
    // ... other control types
}
```

### Step 6: Save Generated Mockup

```bash
MOCKUP_DIR="plugins/$PLUGIN_NAME/.ideas/mockups"
mkdir -p "$MOCKUP_DIR"

# Determine next version number
VERSION=$(find_next_version "$MOCKUP_DIR")

# Save with aesthetic source comment
echo "<!-- Generated from aesthetic: $AESTHETIC_ID -->" > \
    "$MOCKUP_DIR/v${VERSION}-ui-test.html"
cat generated_mockup.html >> \
    "$MOCKUP_DIR/v${VERSION}-ui-test.html"
```

**Naming convention:**
- First application: `v1-ui-test.html` (user can iterate and finalize)
- Aesthetic comment at top documents source

### Step 7: Update Aesthetic Metadata

**Track usage:**
```bash
# Read metadata.json
# Add $PLUGIN_NAME to "usedInPlugins" array if not already present
# Write updated metadata.json
```

### Step 8: Present Decision Menu

```
✅ Mockup v1-test generated with "Vintage Hardware" aesthetic

Files created:
- plugins/ReverbPlugin/.ideas/mockups/v1-ui-test.html

Applied aesthetic:
- Layout: 3x3 grid (8 knobs) + 2 vertical sliders for prominent params
- Colors: Warm vintage palette (orange/brown/cream)
- Controls: Brass rotary knobs with tick marks
- Spacing: Generous (32px gaps)
- Surface: Subtle paper texture

What's next?
1. Preview in browser - Open v1-ui-test.html
2. Continue with ui-mockup skill - Generate full scaffolding
3. Iterate design - Adjust and regenerate
4. Choose different aesthetic - Try another template
```

---

## Operation 3: List Aesthetics

**Input:** None (or optional filter by tag)
**Output:** Table of all aesthetics with prose summaries

### Step 1: Read Manifest

```bash
MANIFEST=".claude/aesthetics/manifest.json"
test -f "$MANIFEST" || { echo "No aesthetics saved"; exit 0; }
```

**Use Read tool to load manifest.json.**

### Step 2: Display Table

**Format as markdown table:**

```markdown
# Aesthetic Library

| ID | Name | Vibe | Source Plugin | Created | Used In |
|----|------|------|---------------|---------|---------|
| vintage-hardware-001 | Vintage Hardware | Warm retro analog | TapeAge | 2025-11-10 | TapeAge, CompCo |
| modern-minimal-002 | Modern Minimal | Clean flat blue/gray | EQPro | 2025-11-11 | EQPro |
```

**For each aesthetic:**
- ID
- Name
- Vibe (extract from aesthetic.md "Visual Identity" or metadata description)
- Source plugin
- Created date
- Used in plugins (comma-separated)

### Step 3: Show Preview Paths

```
Preview files:
- vintage-hardware-001: .claude/aesthetics/vintage-hardware-001/preview.html
- modern-minimal-002: .claude/aesthetics/modern-minimal-002/preview.html

Open in browser:
  open .claude/aesthetics/vintage-hardware-001/preview.html
```

### Step 4: Decision Menu

```
What would you like to do?
1. View aesthetic details - Show full aesthetic.md prose
2. Apply aesthetic to plugin - Choose aesthetic and target
3. Delete aesthetic - Remove from library
4. Exit
```

---

## Operation 4: Delete Aesthetic

**Input:** Aesthetic ID
**Output:** Aesthetic removed from library

### Step 1: Confirm Deletion

**Show usage information:**
```
⚠️  Delete aesthetic "Vintage Hardware" (vintage-hardware-001)?

Currently used in 2 plugins:
- TapeAge
- CompCo

Deleting will NOT affect existing plugins (they have copies of the UI).
This only removes the template from the library.

Are you sure? (yes/no)
```

### Step 2: Remove Files

```bash
if confirmed; then
    rm -rf ".claude/aesthetics/$AESTHETIC_ID"
fi
```

Removes:
- aesthetic.md
- preview.html
- metadata.json
- Directory itself

### Step 3: Update Manifest

**Remove entry from manifest.json:**
```bash
# Read manifest.json
# Filter out deleted aesthetic from "aesthetics" array
# Write updated manifest.json
```

### Step 4: Confirmation

```
✅ Aesthetic "Vintage Hardware" deleted from library.

Plugins using this aesthetic are unaffected (they have independent copies).

Remaining aesthetics: 1
```

---

## Operation 5: Update Aesthetic

**Input:**
- Aesthetic ID
- Path to improved mockup HTML

**Output:** Updated aesthetic.md with refined prose

**Process:**
1. Run same analysis as Save workflow (Step 2-3)
2. Generate new prose descriptions
3. Merge with existing aesthetic.md (preserve user edits where possible)
4. Update metadata.json (increment version, update modified date)
5. Optionally replace preview.html

**Strategy for preserving user edits:**
- Parse existing aesthetic.md section by section
- Regenerate each section from new mockup
- Compare old vs new prose
- If user has manually edited a section (detected by non-template language), prompt:
  ```
  Section "Knob Style" has manual edits. How to handle?
  1. Keep existing (preserve manual edits)
  2. Replace with regenerated (lose edits)
  3. Show diff and let me merge
  ```

---

## Integration with ui-mockup Skill

**Invocation from ui-mockup Phase 0:**

```markdown
If aesthetics exist in library:
  ui-mockup presents menu:
    1. Start from aesthetic template → Invoke ui-template-library "apply"
    2. Start from scratch → Continue normal ui-mockup flow
    3. List all aesthetics → Invoke ui-template-library "list"
```

**After mockup finalized (Phase 5.5 decision menu):**

```markdown
Option 4: Save as aesthetic template
  → Invoke ui-template-library "save" operation

Option 5: Finalize AND save aesthetic
  → Invoke ui-template-library "save" then proceed to scaffolding
```

**See:** ui-mockup skill SKILL.md Phase 0 and Phase 5.5 for complete integration details

---

## Success Criteria

**Save operation successful when:**
- ✅ aesthetic.md generated following exact template structure
- ✅ All sections filled with interpretable prose (no placeholders)
- ✅ Example color codes provided as concrete reference
- ✅ preview.html copied to aesthetic directory
- ✅ metadata.json created with tags and source info
- ✅ manifest.json updated with new aesthetic entry
- ✅ Format is idempotent (same mockup → same structure every time)

**Apply operation successful when:**
- ✅ aesthetic.md parsed and interpreted correctly
- ✅ Appropriate layout chosen for parameter count
- ✅ Generated mockup reflects aesthetic visual language
- ✅ Control styling matches prose descriptions
- ✅ Colors, typography, spacing consistent with aesthetic
- ✅ WebView constraints enforced (no viewport units, etc.)
- ✅ Result is recognizably the same aesthetic despite different layout

**List operation successful when:**
- ✅ All aesthetics shown in readable table
- ✅ Vibe summaries extracted from prose
- ✅ Preview paths provided for visual reference
- ✅ Decision menu presented for next action

---

## Implementation Notes

### Why This Approach Works

**1. No browser APIs required:**
- Pattern matching with grep/sed extracts colors, fonts, values
- String analysis detects control types and styles
- Prose generation transforms patterns into descriptions
- Only requires Read/Write/Bash tools

**2. Aesthetics are conceptual:**
- "Brass knobs with tick marks" is more transferable than "knob diameter: 70px, border: 2px solid #c8a882"
- Descriptions capture intent and feel, not rigid specifications
- Claude can interpret prose and make appropriate decisions for different contexts

**3. Format is idempotent:**
- Template structure is always the same (same sections, same order)
- Parsing is predictable and reliable
- Sections are clearly delimited with markdown headers
- Human-editable while remaining machine-parseable

**4. Flexible adaptation:**
- Apply workflow interprets prose to make context-appropriate decisions
- Same aesthetic works for 3-parameter or 12-parameter plugins
- Layout adapts while visual language remains consistent

### Constraints Satisfied

- ✅ NO browser-based operations (getComputedStyle, DOM parsing)
- ✅ NO complex JavaScript execution requirements
- ✅ ONLY uses Read, Write, Bash tools
- ✅ Produces consistent, parseable aesthetic.md format
- ✅ Maintains compatibility with ui-mockup skill invocation patterns

### Parsing Strategy

**Extracting from aesthetic.md:**

1. **Section detection:**
   ```bash
   # Extract "Knob Style" section
   sed -n '/^### Knob Style$/,/^### /p' aesthetic.md | head -n -1
   ```

2. **Value extraction:**
   ```bash
   # Extract color codes from "Example Color Codes" section
   sed -n '/^## Example Color Codes$/,/^## /p' aesthetic.md | \
       grep -oE '#[0-9a-fA-F]{6}'
   ```

3. **Prose interpretation:**
   - Read section text into variable
   - Use Claude's reasoning to interpret descriptions
   - Extract key phrases: "generous spacing" → gap: 32px
   - Map descriptive terms: "warm palette" → orange/brown hues

### Quality Control

**Before saving aesthetic:**
- ✅ Verify all template sections are present
- ✅ Check for placeholder text not replaced
- ✅ Validate color codes are valid hex/rgb
- ✅ Ensure prose is descriptive (not just values)

**Before applying aesthetic:**
- ✅ Verify aesthetic.md exists and is readable
- ✅ Check required sections are present
- ✅ Validate parameter spec is available
- ✅ Ensure generated HTML follows WebView constraints

---

## Reference Documentation

- **Pattern extraction:** `references/pattern-extraction.md` - String/regex strategies for extracting visual patterns from HTML
- **Prose generation:** `references/prose-generation.md` - Guidelines for transforming patterns into interpretable prose
- **Aesthetic interpretation:** `references/aesthetic-interpretation.md` - Strategies for parsing prose and generating CSS/HTML
- **Layout generation:** `references/layout-generation.md` - Layout decision trees and control placement strategies

---

## Template Assets

- **Aesthetic template:** `assets/aesthetic-template.md` - Complete structured prose template (THE FORMAT SPEC)
- **Metadata template:** `assets/metadata-template.json` - Metadata JSON structure
- **Manifest init:** `assets/manifest-init.json` - Empty manifest structure for initialization
