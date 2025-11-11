# Aesthetic Interpretation Logic

**Purpose:** Parse aesthetic.md prose descriptions and generate appropriate CSS/HTML for new plugins.

This document describes strategies for reading structured prose from aesthetic.md and transforming it into concrete design decisions (colors, sizes, layouts) that work with different parameter counts.

---

## Core Philosophy

**Aesthetics are conceptual, not pixel-perfect specs.**

When applying an aesthetic to a new plugin, the goal is to **capture the vibe and visual language** while adapting layout appropriately. This requires:

1. **Parsing prose sections** to extract design intent
2. **Interpreting descriptive language** to make concrete decisions
3. **Maintaining consistency** while allowing flexible adaptation
4. **Making context-appropriate choices** for the target plugin

---

## Parsing Strategy

### Section Extraction

**Use sed to extract specific sections from aesthetic.md:**

```bash
# Extract "Knob Style" section
sed -n '/^### Knob Style$/,/^### /p' aesthetic.md | head -n -1

# Extract "Color System" section
sed -n '/^## Color System$/,/^## /p' aesthetic.md | head -n -1

# Extract "Example Color Codes" section
sed -n '/^## Example Color Codes$/,/^---$/p' aesthetic.md
```

**Sections are delimited by markdown headers:**
- `## Section Name` = top-level section
- `### Subsection Name` = second-level section
- Extract until next header of same level

### Value Extraction

**Extract concrete values from prose:**

```bash
# Extract hex colors from "Example Color Codes" section
sed -n '/^## Example Color Codes$/,/^---$/p' aesthetic.md | \
    grep -oE '#[0-9a-fA-F]{6}'

# Extract size values from prose (look for patterns like "70px" or "medium (70px)")
grep -oE '\([0-9]+px\)' knob_section.txt | tr -d '()'
```

**Common patterns:**
- `(70px)` = explicit size in parentheses
- `#ff6b35` = hex color code
- `blur: 8px` = specific CSS value
- `"property: value"` = quoted CSS

---

## 1. Color System Interpretation

### Reading Color Prose

**Example prose:**
```
**Background Colors:**
- Main background: Dark brown (#2b2015) creating warm, vintage ambiance
- Surface/panel background: Slightly lighter brown (#3a2820) for layering
- Elevated surfaces: Not used in this aesthetic

**Accent Colors:**
- Primary accent: Burnt orange (#ff6b35) for active elements and highlights
- Secondary accent: Brass/copper tone (#d97742) for control bodies
- Hover/active state: Brightened orange (#ff8555) for interaction feedback
```

**Extraction strategy:**

```javascript
function extractColors(colorSection) {
    // Extract hex codes
    const hexColors = colorSection.match(/#[0-9a-fA-F]{6}/g);

    // Map to CSS variables based on context
    const colorMap = {
        '--bg-main': hexColors[0],  // First color mentioned under "Main background"
        '--bg-surface': hexColors[1],  // Second color under "Surface"
        '--accent-primary': hexColors[2],  // First under "Primary accent"
        '--accent-secondary': hexColors[3],  // Under "Secondary accent"
        '--accent-hover': hexColors[4],  // Under "Hover/active"
    };

    return colorMap;
}
```

**Interpretation rules:**
- First color in each subsection = primary value for that usage
- Multiple colors = alternatives or variations
- Descriptive terms guide usage: "active elements" → use for focused controls

### Color Philosophy Interpretation

**Example prose:**
```
Warm palette emphasizing vintage analog hardware. High contrast between
dark background and cream text ensures readability. Orange accents pop
against brown base without overwhelming. Overall mood: nostalgic, warm, approachable.
```

**Extraction:**
- "Warm palette" → Use warm hues for any generated colors
- "High contrast" → Maintain strong text/background contrast
- "Orange accents pop" → Keep accent colors vibrant, not muted
- "Nostalgic, warm" → Vibe descriptor for future customization

**Application:**
If aesthetic doesn't specify a color for a new need (e.g., error state), infer:
- Stay within warm palette (red-orange for errors, not cool red)
- Maintain contrast level (not too subtle)
- Match saturation (vibrant, not muted)

---

## 2. Typography Interpretation

### Reading Typography Prose

**Example prose:**
```
**Font Families:**
- Headings: Bold sans-serif, technical feel (Roboto Bold or similar)
- Body text: Same sans-serif family (Roboto Regular)
- Values/numbers: Monospace (Roboto Mono) for precision

**Font Sizing:**
- Plugin title: Large (24px) for prominence
- Section labels: Medium (16px) for hierarchy
- Parameter labels: Standard (14px) readable at distance
- Parameter values: Small (12px) monospace, technical
```

**Extraction strategy:**

```javascript
function extractTypography(typoSection) {
    // Extract font families from descriptions
    const headingFont = extractFontFamily(typoSection, 'Headings');
    const bodyFont = extractFontFamily(typoSection, 'Body text');
    const monoFont = extractFontFamily(typoSection, 'Values/numbers');

    // Extract sizes
    const sizes = {
        title: extractSize(typoSection, 'Plugin title'),  // → 24px
        section: extractSize(typoSection, 'Section labels'),  // → 16px
        label: extractSize(typoSection, 'Parameter labels'),  // → 14px
        value: extractSize(typoSection, 'Parameter values'),  // → 12px
    };

    return { fonts: { headingFont, bodyFont, monoFont }, sizes };
}

function extractFontFamily(text, key) {
    // Look for font names in parentheses or after descriptions
    // "Roboto Bold or similar" → 'Roboto', sans-serif
    // "Monospace (Roboto Mono)" → 'Roboto Mono', monospace

    const match = text.match(new RegExp(key + ':.*?\\((.*?)\\)', 'i'));
    if (match) return match[1] + ', sans-serif';

    // Fallback: extract generic family
    if (text.includes('sans-serif')) return 'sans-serif';
    if (text.includes('monospace')) return 'monospace';
    if (text.includes('serif')) return 'serif';
}

function extractSize(text, key) {
    // Look for size in parentheses after key
    const match = text.match(new RegExp(key + ':.*?\\(([0-9]+px)\\)', 'i'));
    return match ? match[1] : '14px';  // Default if not found
}
```

**Application:**
```css
h1 {
    font-family: 'Roboto', sans-serif;
    font-weight: 700;
    font-size: 24px;
}

.param-label {
    font-family: 'Roboto', sans-serif;
    font-size: 14px;
}

.param-value {
    font-family: 'Roboto Mono', monospace;
    font-size: 12px;
}
```

---

## 3. Control Style Interpretation

### Knob Style Interpretation

**Example prose:**
```
**Visual Design:**
- Shape: Circle
- Size: Medium (70px) for comfortable interaction
- Indicator style: Single pointer line from center to edge
- Border treatment: Subtle 2px solid border in accent color

**Surface Treatment:**
- Base appearance: Flat color fill (brass/copper tone)
- Depth: Subtle shadow (0 2px 8px rgba(0,0,0,0.3)) for slight elevation
- Tick marks: Prominent marks at 10° intervals around perimeter
- Center indicator: Small dot at rotation center
```

**Extraction strategy:**

```javascript
function extractKnobStyle(knobSection) {
    // Shape
    const isCircular = knobSection.includes('Circle');
    const borderRadius = isCircular ? '50%' : '8px';

    // Size
    const sizeMatch = knobSection.match(/\(([0-9]+px)\)/);
    const size = sizeMatch ? sizeMatch[1] : '70px';

    // Border
    const borderMatch = knobSection.match(/([0-9]+px)\s+solid/);
    const borderWidth = borderMatch ? borderMatch[1] : '2px';

    // Shadow
    const shadowMatch = knobSection.match(/\(([^)]+rgba[^)]+)\)/);
    const boxShadow = shadowMatch ? shadowMatch[1] : 'none';

    // Indicator type
    const hasPointer = knobSection.includes('pointer line');
    const hasTicks = knobSection.includes('tick marks');
    const hasCenterDot = knobSection.includes('center dot') ||
                         knobSection.includes('Center indicator');

    return {
        width: size,
        height: size,
        borderRadius,
        border: `${borderWidth} solid var(--accent-secondary)`,
        boxShadow,
        hasPointer,
        hasTicks,
        hasCenterDot,
    };
}
```

**CSS generation:**

```css
.rotary-knob {
    width: 70px;
    height: 70px;
    border-radius: 50%;
    border: 2px solid var(--accent-secondary);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
    background: var(--control-base);
    position: relative;
}

/* Pointer line indicator */
.rotary-knob::after {
    content: '';
    position: absolute;
    top: 10%;
    left: 50%;
    width: 2px;
    height: 40%;
    background: var(--accent-primary);
    transform-origin: bottom center;
}

/* Center dot */
.rotary-knob::before {
    content: '';
    position: absolute;
    top: 50%;
    left: 50%;
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: var(--accent-primary);
    transform: translate(-50%, -50%);
}
```

### Slider Style Interpretation

**Example prose:**
```
**Layout:**
- Orientation preference: Vertical for prominence/mix parameters, horizontal for others
- Dimensions: Medium thickness (30px wide, 120px tall for vertical)
- Track design: Filled from bottom-up with accent color

**Thumb Design:**
- Shape: Rectangle with rounded corners (8px radius)
- Size: Slightly wider than track (36px wide)
- Style: Flat with subtle shadow
```

**Extraction and CSS generation:**

```javascript
function extractSliderStyle(sliderSection) {
    const isVertical = sliderSection.includes('Vertical') ||
                       sliderSection.includes('vertical');

    const widthMatch = sliderSection.match(/([0-9]+px)\s+wide/);
    const heightMatch = sliderSection.match(/([0-9]+px)\s+tall/);

    return {
        orientation: isVertical ? 'vertical' : 'horizontal',
        width: widthMatch ? widthMatch[1] : '30px',
        height: heightMatch ? heightMatch[1] : '120px',
        thumbBorderRadius: extractValue(sliderSection, 'radius') || '8px',
    };
}
```

```css
.vertical-slider {
    width: 30px;
    height: 120px;
    background: var(--control-track);
    border-radius: 15px;
    position: relative;
}

.vertical-slider .fill {
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    background: var(--accent-primary);
    border-radius: 0 0 15px 15px;
    transition: height 0.2s ease;
}

.vertical-slider .thumb {
    width: 36px;
    height: 20px;
    background: var(--control-base);
    border-radius: 8px;
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);
    position: absolute;
    left: 50%;
    transform: translateX(-50%);
}
```

---

## 4. Spacing Interpretation

### Reading Spacing Prose

**Example prose:**
```
**Overall Density:**
Generous spacing creates comfortable, uncluttered interface. Each control
has room to breathe, preventing accidental touches and allowing focus.

**Control Spacing:**
- Between controls: Generous (32px) creating clear visual separation
- Vertical rhythm: Consistent 32px maintains grid alignment
- Grouping strategy: Related controls share same row/column with no extra gap

**Padding & Margins:**
- Edge margins: Comfortable (24px) from window edges
- Section padding: 20px within logical groups
- Label-to-control gap: Tight (8px) to maintain association
```

**Extraction:**

```javascript
function extractSpacing(spacingSection) {
    // Extract explicit values
    const controlGap = extractValue(spacingSection, 'Between controls') || '24px';
    const edgeMargin = extractValue(spacingSection, 'Edge margins') || '20px';
    const labelGap = extractValue(spacingSection, 'Label-to-control gap') || '8px';

    // Infer density from descriptors
    const density = spacingSection.includes('Generous') ? 'generous' :
                    spacingSection.includes('Tight') ? 'tight' : 'comfortable';

    return { controlGap, edgeMargin, labelGap, density };
}

function extractValue(text, key) {
    // Look for patterns like "(32px)" after key
    const regex = new RegExp(key + ':.*?\\(([0-9]+px)\\)', 'i');
    const match = text.match(regex);
    return match ? match[1] : null;
}
```

**CSS generation:**

```css
body {
    padding: 24px;  /* Edge margin */
}

.controls {
    display: grid;
    gap: 32px;  /* Control gap */
}

.control-wrapper {
    display: flex;
    flex-direction: column;
    gap: 8px;  /* Label-to-control gap */
}
```

### Density Adaptation

**If aesthetic says "generous" but plugin has 15 parameters:**

```javascript
function adaptSpacing(aesthetic, paramCount) {
    const baseGap = parseFloat(aesthetic.controlGap);

    // Scale down for high parameter counts
    if (paramCount > 12) {
        return {
            ...aesthetic,
            controlGap: `${Math.max(baseGap * 0.7, 16)}px`,  // Reduce but not below 16px
        };
    }

    return aesthetic;
}
```

**Maintain density feel while adapting to constraints:**
- "Generous" → Never go below 20px even with many parameters
- "Tight" → Can go as low as 8px
- "Comfortable" → 16-24px range

---

## 5. Surface Treatment Interpretation

### Texture Interpretation

**Example prose:**
```
**Background:**
- Texture type: Subtle paper texture
- Intensity: Barely visible (opacity: 0.03)
- Implementation: Noise pattern overlay via CSS pseudo-element
```

**CSS generation:**

```css
body::before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-image: url('data:image/svg+xml,...');  /* Noise pattern */
    opacity: 0.03;
    pointer-events: none;
}
```

**If no texture specified:**
- Default to flat background
- Don't invent textures not in aesthetic

### Shadow Interpretation

**Example prose:**
```
**Shadow Strategy:**
- Shadow presence: Subtle
- Shadow color: Black with low opacity
- Shadow blur: Soft (8-10px)
- Typical shadow values: 0 2px 8px rgba(0,0,0,0.2)
```

**Extraction:**

```javascript
function extractShadowStyle(surfaceSection) {
    // Look for explicit shadow value
    const shadowMatch = surfaceSection.match(/(\d+px\s+\d+px\s+\d+px\s+rgba\([^)]+\))/);
    if (shadowMatch) return shadowMatch[1];

    // Infer from descriptors
    if (surfaceSection.includes('Subtle')) return '0 2px 8px rgba(0,0,0,0.2)';
    if (surfaceSection.includes('Medium')) return '0 4px 12px rgba(0,0,0,0.3)';
    if (surfaceSection.includes('Dramatic')) return '0 8px 24px rgba(0,0,0,0.4)';
    if (surfaceSection.includes('None')) return 'none';

    return 'none';  // Default
}
```

---

## 6. Layout Decision Making

### Parameter Count Analysis

**Given target plugin parameters:**

```javascript
const parameters = [
    { id: 'GAIN', type: 'FLOAT', prominent: false },
    { id: 'TONE', type: 'FLOAT', prominent: false },
    { id: 'DRIVE', type: 'FLOAT', prominent: false },
    { id: 'MIX', type: 'FLOAT', prominent: true },
    { id: 'OUTPUT', type: 'FLOAT', prominent: true },
    { id: 'BYPASS', type: 'BOOL', prominent: false },
];
```

**Layout decision tree:**

```javascript
function chooseLayout(params, aesthetic) {
    const regularParams = params.filter(p => !p.prominent);
    const prominentParams = params.filter(p => p.prominent);

    const count = regularParams.length;

    // Consult aesthetic guidelines if present
    const guidelines = aesthetic.applicationGuidelines;
    if (guidelines && guidelines[`${count}-parameters`]) {
        return guidelines[`${count}-parameters`];
    }

    // Default layout strategy
    if (count <= 3) return 'horizontal-row';
    if (count <= 6) return 'grid-2x3';
    if (count <= 9) return 'grid-3x3';
    if (count <= 12) return 'grid-4x3';
    return 'scrollable-grid';
}
```

### Prominent Parameter Handling

**Consult aesthetic.md "Application Guidelines" section:**

```
**Prominent Parameter Handling:**
Prominent parameters (Mix, Output, Dry/Wet) should be visually distinguished:
- Place as vertical sliders on right side
- Use 150% size of regular controls
- Different control color (lighter shade of accent)
```

**Implementation:**

```javascript
function layoutProminentParams(prominentParams, aesthetic) {
    const handling = aesthetic.prominentHandling;

    if (handling.includes('vertical sliders on right')) {
        return {
            position: 'right-sidebar',
            controlType: 'vertical-slider',
            size: 'large',
        };
    }

    if (handling.includes('bottom row')) {
        return {
            position: 'bottom-row',
            controlType: 'knob',
            size: 'large',
        };
    }

    // Default: integrate into main grid with visual distinction
    return {
        position: 'inline',
        controlType: 'same-as-regular',
        size: 'larger',
        visualDistinction: 'border-highlight',
    };
}
```

---

## 7. Interaction Feel Interpretation

### Responsiveness

**Example prose:**
```
**Responsiveness:**
Controls react immediately to input with smooth 200ms transitions.
No artificial delays - interface feels direct and responsive.
```

**CSS generation:**

```css
.control {
    transition: all 0.2s ease-out;
}

.control:hover {
    /* Immediate hover state, but smoothly transition back */
    transform: scale(1.02);
}
```

### Feedback

**Example prose:**
```
**Feedback:**
Visual feedback on every interaction. Hover brightens, active slightly
scales down, drag shows real-time value change.
```

**Implementation:**

```css
.control:hover {
    filter: brightness(1.1);
}

.control:active {
    transform: scale(0.98);
}
```

---

## 8. Complete Generation Example

**Input:** aesthetic.md for "Vintage Hardware" + target plugin with 5 parameters

**Process:**

1. **Extract colors** → CSS variables
2. **Extract typography** → Font declarations
3. **Choose layout** → 5 params = grid-2x3 (2 rows, 3 columns)
4. **Extract knob style** → Generate .rotary-knob CSS
5. **Extract spacing** → gap: 32px, padding: 24px
6. **Extract shadows** → box-shadow for controls
7. **Generate HTML** → 5 control wrappers in grid

**Output HTML:**

```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Vintage Effect</title>
    <style>
        :root {
            --bg-main: #2b2015;
            --bg-surface: #3a2820;
            --accent-primary: #ff6b35;
            --accent-secondary: #d97742;
            --text-primary: #f5e6d3;
            --control-base: #d97742;
        }

        html, body {
            margin: 0;
            padding: 24px;
            height: 100%;
            background: var(--bg-main);
            color: var(--text-primary);
            font-family: 'Roboto', sans-serif;
            user-select: none;
        }

        h1 {
            font-size: 24px;
            font-weight: 700;
            margin: 0 0 32px 0;
        }

        .controls {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            grid-template-rows: repeat(2, auto);
            gap: 32px;
        }

        .control-wrapper {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
        }

        .rotary-knob {
            width: 70px;
            height: 70px;
            border-radius: 50%;
            border: 2px solid var(--accent-secondary);
            box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
            background: var(--control-base);
            position: relative;
        }

        label {
            font-size: 14px;
        }
    </style>
</head>
<body>
    <h1>Vintage Effect</h1>
    <div class="controls">
        <div class="control-wrapper">
            <div class="rotary-knob" data-param="GAIN"></div>
            <label>Gain</label>
        </div>
        <!-- ... repeat for 5 parameters -->
    </div>
</body>
</html>
```

---

## Success Criteria

**Interpretation successful when:**
- ✅ Colors extracted and applied via CSS variables
- ✅ Typography hierarchy matches aesthetic prose
- ✅ Control styling reflects described visual design
- ✅ Spacing density maintains aesthetic feel
- ✅ Layout appropriate for parameter count
- ✅ Surface treatments (shadows, textures) preserved
- ✅ Result recognizably embodies the aesthetic vibe
- ✅ WebView constraints enforced (no viewport units)
- ✅ Generated mockup is functional and production-ready
