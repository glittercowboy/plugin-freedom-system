# Pattern Extraction Logic

**Purpose:** Extract visual patterns from HTML mockups using string/regex matching (no browser APIs).

This document describes strategies for analyzing HTML files with Read/Bash tools to identify colors, fonts, control types, spacing, and other visual patterns that inform aesthetic prose generation.

---

## Core Strategy

**No DOM parsing or browser APIs available.** Instead:

1. **Read HTML file into memory** (Read tool)
2. **Extract patterns using grep/sed/awk** (Bash tool)
3. **Analyze extracted strings** (Claude reasoning)
4. **Transform patterns into prose descriptions** (see prose-generation.md)

---

## 1. Color Extraction

### Hex Colors

```bash
# Extract all hex color codes
grep -oE '#[0-9a-fA-F]{3,6}' mockup.html | sort -u

# Example output:
# #2b2015
# #ff6b35
# #f5e6d3
# #d97742
```

**Analysis:**
- Count occurrences to determine primary vs accent colors
- Group by hue family (warm: orange/red/yellow, cool: blue/green, neutral: gray/brown)
- Identify darkest (background candidate) and lightest (text candidate)

### RGB/RGBA Colors

```bash
# Extract rgba colors
grep -oE 'rgba?\([^)]+\)' mockup.html | sort -u

# Example output:
# rgba(0, 0, 0, 0.3)
# rgb(255, 107, 53)
```

**Analysis:**
- Alpha values indicate shadow/overlay usage
- Convert to hex for consistency if needed

### CSS Custom Properties

```bash
# Extract CSS variable definitions
grep -oE '--[a-zA-Z-]+:\s*[^;]+' mockup.html

# Example output:
# --bg-color: #2b2015
# --accent-color: #ff6b35
# --text-color: #f5e6d3
```

**Analysis:**
- CSS variables show intentional color system
- Variable names indicate usage (bg, accent, text, control, etc.)
- Use as primary source if present

### Color Categorization Strategy

```javascript
// Pseudo-code for categorizing extracted colors
function categorizeColors(colors) {
    // Sort by luminance
    const sorted = colors.sort((a, b) => luminance(a) - luminance(b));

    return {
        background: sorted[0],  // Darkest (or lightest for light themes)
        text_primary: sorted[sorted.length - 1],  // Highest contrast to background
        accents: sorted.slice(1, -1).filter(isVibrant),  // Saturated colors
        neutrals: sorted.slice(1, -1).filter(isNeutral),  // Desaturated colors
    };
}
```

### Color Usage Context

**Identify where colors are used:**

```bash
# Find colors used in body/background
grep -B2 -A2 'body\s*{' mockup.html | grep -oE '#[0-9a-fA-F]{6}'

# Find colors in control styles
grep -B5 -A5 '\.knob\|\.slider\|\.button' mockup.html | grep -oE '#[0-9a-fA-F]{6}'
```

---

## 2. Typography Extraction

### Font Families

```bash
# Extract font-family declarations
grep -oE "font-family:\s*[^;]+" mockup.html | \
    sed "s/font-family:\s*//" | \
    sort -u

# Example output:
# 'Courier New', monospace
# 'Roboto', sans-serif
# 'Bebas Neue', sans-serif
```

**Analysis:**
- Multiple fonts indicate hierarchy (headings vs body vs mono)
- Fallback stacks show intent (serif = traditional, sans-serif = modern, monospace = technical)
- Generic families as fallbacks maintain style across systems

### Font Sizes

```bash
# Extract font-size values
grep -oE "font-size:\s*[0-9]+px" mockup.html | \
    awk -F': ' '{print $2}' | \
    sort -n -u

# Example output:
# 12px
# 14px
# 16px
# 24px
```

**Analysis:**
- Identify size hierarchy: smallest (labels/values), medium (body), largest (headings)
- Size jumps indicate emphasis levels
- Typical scales: 12-14-16-24 (moderate), 10-12-14-20 (tight), 14-16-18-32 (generous)

### Font Weights

```bash
# Extract font-weight values
grep -oE "font-weight:\s*[0-9]+" mockup.html | \
    awk -F': ' '{print $2}' | \
    sort -n -u

# Example output:
# 400
# 700
```

**Analysis:**
- 400 = normal, 700 = bold
- Multiple weights indicate typographic hierarchy
- Single weight (400) suggests minimal styling

### Letter Spacing

```bash
# Extract letter-spacing values
grep -oE "letter-spacing:\s*[^;]+" mockup.html | \
    sed "s/letter-spacing:\s*//" | \
    sort -u
```

**Analysis:**
- Positive values (0.05em, 1px) = airy, technical feel
- Negative values (-0.02em) = compact, tight feel
- Zero/omitted = standard reading

---

## 3. Control Type Detection

### Rotary Knobs

```bash
# Detect knob classes and styling
grep -qi "knob\|rotary" mockup.html && echo "Has rotary knobs"

# Extract knob-specific styles
grep -A10 "\.knob\|\.rotary" mockup.html

# Look for circular indicators
grep -qi "border-radius:\s*50%" mockup.html && echo "Circular controls present"

# Check for tick marks
grep -qi "tick\|mark\|notch" mockup.html && echo "Tick marks present"
```

**Key patterns:**
- `border-radius: 50%` = circular knob
- Classes named "knob", "rotary", "dial"
- SVG circles or arcs = skeuomorphic knobs
- "pointer", "indicator", "needle" = knob pointer

### Sliders

```bash
# Detect slider classes
grep -qi "slider\|range\|fader" mockup.html && echo "Has sliders"

# Detect orientation
grep -qi "vertical.*slider\|slider.*vertical" mockup.html && echo "Vertical sliders"
grep -qi "horizontal.*slider\|slider.*horizontal" mockup.html && echo "Horizontal sliders"

# Extract slider dimensions
grep -B5 -A5 "\.slider" mockup.html | grep -E "width:\s*[0-9]+|height:\s*[0-9]+"
```

**Key patterns:**
- `height > width` = vertical slider
- `width > height` = horizontal slider
- "track", "thumb", "rail" = slider components
- "fill", "progress" = filled sliders

### Buttons/Toggles

```bash
# Detect button elements and classes
grep -qi "button\|toggle\|switch" mockup.html && echo "Has buttons"

# Check for toggle indicators
grep -qi "checked\|active\|on.*state" mockup.html && echo "Stateful buttons"
```

**Key patterns:**
- `<button>` elements
- Classes "toggle", "switch", "checkbox"
- Active/checked states defined = stateful controls

---

## 4. Spacing Analysis

### Gap Values

```bash
# Extract gap, padding, margin values
grep -oE "(gap|padding|margin):\s*[0-9]+px" mockup.html | \
    awk -F': ' '{print $2}' | \
    sort -n

# Example output:
# 8px
# 16px
# 24px
# 32px
```

**Analysis:**
- Identify most common value = base spacing unit
- Multiples of base unit = consistent spacing system
- Typical units: 8px (tight), 16px (comfortable), 24-32px (generous)

### Spacing Philosophy Inference

```javascript
// Pseudo-code for inferring spacing density
function inferSpacingDensity(gaps) {
    const median = calculateMedian(gaps);

    if (median < 16) return 'tight';
    if (median < 24) return 'comfortable';
    return 'generous';
}
```

### Flexbox/Grid Detection

```bash
# Detect layout systems
grep -qi "display:\s*flex" mockup.html && echo "Uses flexbox"
grep -qi "display:\s*grid" mockup.html && echo "Uses grid"

# Extract grid template values
grep -oE "grid-template-columns:\s*[^;]+" mockup.html
grep -oE "grid-template-rows:\s*[^;]+" mockup.html
```

**Analysis:**
- Flex = row/column arrangements
- Grid = structured layouts
- Template values indicate grid size (repeat(3, 1fr) = 3 columns)

---

## 5. Visual Effects Detection

### Shadows

```bash
# Extract box-shadow values
grep -oE "box-shadow:\s*[^;]+" mockup.html | \
    sed "s/box-shadow:\s*//" | \
    sort -u

# Example output:
# 0 2px 8px rgba(0, 0, 0, 0.2)
# 0 4px 12px rgba(0, 0, 0, 0.3)
```

**Analysis:**
- Shadow presence = depth/elevation
- Blur radius: <5px = subtle, 5-15px = medium, >15px = dramatic
- Alpha values: <0.2 = subtle, 0.2-0.4 = medium, >0.4 = prominent
- No shadows = flat design

### Border Radius

```bash
# Extract border-radius values
grep -oE "border-radius:\s*[0-9]+px" mockup.html | \
    awk -F': ' '{print $2}' | \
    sort -n -u

# Example output:
# 0px
# 4px
# 8px
# 50%
```

**Analysis:**
- 0px = sharp corners (technical, modern)
- 4-8px = subtle rounding (friendly, modern)
- 12-20px = prominent rounding (playful, soft)
- 50% = circular elements (knobs, indicators)

### Background Textures

```bash
# Detect background images (textures)
grep -oE "background(-image)?:\s*url\([^)]+\)" mockup.html

# Detect pattern/texture keywords
grep -qi "noise\|grain\|paper\|wood\|metal\|fabric" mockup.html && echo "Texture detected"
```

**Key patterns:**
- `url('noise.png')` = noise texture
- `url('paper.jpg')` = paper texture
- SVG patterns = programmatic textures
- Keywords in comments or class names

### Borders

```bash
# Extract border definitions
grep -oE "border:\s*[^;]+" mockup.html | \
    sed "s/border:\s*//" | \
    sort -u

# Example output:
# 1px solid #404040
# 2px solid #d97742
```

**Analysis:**
- Width: 1px = subtle, 2-3px = prominent, >3px = bold
- Style: solid = clean, dashed = technical, none = borderless
- Color: matching controls = integrated, contrasting = defined

---

## 6. Interaction Patterns

### Hover States

```bash
# Detect hover state definitions
grep -qi ":hover" mockup.html && echo "Hover states defined"

# Extract hover effects
grep -B2 -A2 ":hover" mockup.html
```

**Analysis:**
- Brightness changes: `filter: brightness(1.1)` = subtle brightening
- Color changes: different background/border on hover
- Scale changes: `transform: scale(1.05)` = grow on hover
- No hover styles = static interface

### Active/Focus States

```bash
# Detect active and focus states
grep -qi ":active\|:focus" mockup.html && echo "Interaction states defined"
```

**Analysis:**
- Active states = visual feedback on click
- Focus states = keyboard navigation support

### Transitions

```bash
# Extract transition values
grep -oE "transition:\s*[^;]+" mockup.html | \
    sed "s/transition:\s*//"

# Example output:
# all 0.2s ease-in-out
# background-color 300ms ease
```

**Analysis:**
- Duration: <200ms = fast, 200-300ms = normal, >300ms = slow
- Easing: linear = mechanical, ease = natural, ease-in-out = smooth
- No transitions = instant changes (technical, precise)

---

## 7. Layout Structure Analysis

### Control Count and Arrangement

```bash
# Count controls (estimate from class occurrences)
grep -o "class=['\"]control\|class=['\"]knob\|class=['\"]slider" mockup.html | wc -l
```

**Analysis:**
- 1-3 controls = simple layout (horizontal or vertical)
- 4-6 controls = small grid (2x3)
- 7-9 controls = medium grid (3x3)
- 10+ controls = large grid or scrollable

### Section Detection

```bash
# Detect logical sections
grep -qi "section\|group\|panel" mockup.html && echo "Sectioned layout"

# Count sections
grep -o "<section\|<div class=['\"]section\|<div class=['\"]group" mockup.html | wc -l
```

**Analysis:**
- Sections = grouped related controls
- Section padding/margins = visual hierarchy

---

## 8. Vibe Inference

**Combine extracted patterns to infer overall aesthetic vibe:**

### Vintage/Retro Indicators

- Warm colors (orange, brown, yellow)
- Serif fonts or monospace
- Texture presence (paper, noise)
- Skeuomorphic controls (tick marks, brass colors)
- Generous spacing
- Prominent shadows

### Modern/Minimal Indicators

- Cool colors (blue, gray) or monochromatic
- Sans-serif fonts
- No textures (flat)
- Simple controls (clean circles, rectangles)
- Subtle shadows or none
- Tight to comfortable spacing

### Technical/Professional Indicators

- High contrast colors
- Monospace fonts
- Precise values visible
- Grid-based layout
- Borders on controls
- Minimal decoration

### Playful/Creative Indicators

- Vibrant colors (multiple accents)
- Rounded corners (prominent)
- Animations/transitions
- Unconventional control shapes
- Asymmetric layout
- Bold typography

---

## 9. Extraction Workflow Example

**Complete extraction process:**

```bash
#!/bin/bash
MOCKUP="plugins/TapeAge/.ideas/mockups/v2-ui.html"

echo "=== COLOR EXTRACTION ==="
COLORS=$(grep -oE '#[0-9a-fA-F]{6}' "$MOCKUP" | sort -u)
echo "$COLORS"

echo -e "\n=== FONT EXTRACTION ==="
FONTS=$(grep -oE "font-family:\s*[^;]+" "$MOCKUP" | sed "s/font-family:\s*//" | sort -u)
echo "$FONTS"

echo -e "\n=== SPACING EXTRACTION ==="
SPACING=$(grep -oE "(gap|padding):\s*[0-9]+px" "$MOCKUP" | awk -F': ' '{print $2}' | sort -n -u)
echo "$SPACING"

echo -e "\n=== CONTROL DETECTION ==="
grep -qi "knob\|rotary" "$MOCKUP" && echo "- Rotary knobs: YES"
grep -qi "slider" "$MOCKUP" && echo "- Sliders: YES"
grep -qi "button\|toggle" "$MOCKUP" && echo "- Buttons: YES"

echo -e "\n=== SHADOW DETECTION ==="
SHADOWS=$(grep -oE "box-shadow:\s*[^;]+" "$MOCKUP" | sed "s/box-shadow:\s*//")
if [ -n "$SHADOWS" ]; then
    echo "Shadows present:"
    echo "$SHADOWS"
else
    echo "No shadows (flat design)"
fi

echo -e "\n=== BORDER RADIUS ==="
RADIUS=$(grep -oE "border-radius:\s*[0-9]+px" "$MOCKUP" | awk -F': ' '{print $2}' | sort -n -u)
echo "$RADIUS"
```

**Output informs prose generation:**
- Colors → "Warm vintage palette with burnt orange accent..."
- Fonts → "Monospace typography for technical feel..."
- Spacing → "Generous spacing (32px gaps) creates breathing room..."
- Controls → "Rotary knobs with tick marks, circular shape..."
- Shadows → "Subtle shadows (blur: 8px) provide depth..."
- Radius → "Moderate rounding (8px) softens industrial aesthetic..."

---

## 10. Limitations and Workarounds

### What We CAN'T Extract

**No browser APIs means we can't:**
- Compute final rendered styles (getComputedStyle)
- Parse complex CSS cascades accurately
- Execute JavaScript to see dynamic values
- Measure actual rendered dimensions

### Workarounds

**Instead we:**
- Parse explicit CSS declarations (good enough for most cases)
- Use heuristics for common patterns
- Focus on intentional design (explicitly styled elements)
- Generate prose that captures intent, not exact pixels

### Edge Cases

**Multiple stylesheets:**
- If mockup imports external CSS, we miss those styles
- Workaround: Encourage inline styles or single-file mockups

**CSS variables with calc():**
- Can't evaluate `calc(var(--base) * 2)`
- Workaround: Extract variable definitions, describe relationship in prose

**Complex selectors:**
- Hard to trace nested selectors (`.parent > .child .grandchild`)
- Workaround: Focus on class-based styles, ignore complex inheritance

---

## Success Criteria

**Extraction successful when:**
- ✅ All unique colors identified and categorized
- ✅ Font families extracted with usage context
- ✅ Control types detected (knobs, sliders, buttons)
- ✅ Spacing values extracted and density inferred
- ✅ Visual effects identified (shadows, borders, textures)
- ✅ Enough patterns extracted to generate comprehensive aesthetic prose
- ✅ Vibe inference possible from extracted patterns

**Output feeds into prose-generation.md for transformation into aesthetic.md descriptions.**
