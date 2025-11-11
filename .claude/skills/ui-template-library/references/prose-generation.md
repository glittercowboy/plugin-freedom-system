# Prose Generation Guidelines

**Purpose:** Transform extracted visual patterns into interpretable prose descriptions for aesthetic.md.

This document describes how to take raw extracted data (colors, fonts, spacing values) from pattern-extraction.md and generate clear, consistent prose that captures the aesthetic vibe and provides actionable design guidance.

---

## Core Principles

### 1. Descriptive + Concrete

**Good:** "Warm vintage palette with burnt orange accent (#ff6b35) against dark brown background (#2b2015). Cream text (#f5e6d3) provides high contrast."

**Bad:** "Colors: #ff6b35, #2b2015, #f5e6d3" (too sparse)

**Bad:** "Beautiful warm colors that evoke feelings of nostalgia and comfort in a retro aesthetic style" (too vague)

**Balance prose description with concrete values.**

### 2. Transferable Concepts

**Good:** "Generous spacing (32px) creating comfortable breathing room. Not cluttered - each element has space to be appreciated."

**Bad:** "32px gap between controls" (too rigid)

**Good:** "Medium-sized knobs (~70px diameter) comfortable for mouse interaction without dominating the interface."

**Bad:** "Knobs are 70px" (misses the why)

**Describe the design intent, not just the measurements.**

### 3. Actionable Guidance

**Good:** "Prominent parameters like Mix and Output should use vertical sliders on the right side, 150% larger than regular controls, to emphasize their importance."

**Bad:** "Important parameters are big" (not actionable)

**Provide clear application instructions.**

### 4. Consistent Structure

**Every aesthetic.md follows the exact same template structure.**
- Same sections, same order, every time
- Idempotent: same mockup → same section structure
- Human-editable while remaining machine-parseable

---

## Section-by-Section Generation

### Visual Identity Section

**Input:** Extracted patterns summary
**Output:** 2-3 sentence vibe description

**Strategy:**
1. Identify primary aesthetic category (vintage, modern, minimal, technical, etc.)
2. Describe core visual elements (colors, controls, surface treatment)
3. State the overall feeling/mood

**Example generation:**

```javascript
function generateVisualIdentity(patterns) {
    const vibe = inferVibe(patterns);
    const coreElements = identifyCoreElements(patterns);
    const mood = inferMood(patterns);

    return `${vibe} aesthetic featuring ${coreElements}. ${mood}.
            Creates a ${adjective} atmosphere suitable for ${pluginTypes}.`;
}

// Input patterns:
// - Colors: #2b2015, #ff6b35, #f5e6d3 (warm)
// - Controls: rotary knobs, tick marks
// - Texture: paper noise
// - Shadows: subtle

// Output:
"Warm vintage analog aesthetic featuring brass-toned rotary knobs with
prominent tick marks against a textured brown background. Burnt orange
accents provide vibrant highlights without overwhelming the retro vibe.
Creates a nostalgic, tactile atmosphere suitable for tape emulation and
vintage-style effects."
```

---

### Color System Section

**Input:** Extracted hex colors, RGB values, CSS variables
**Output:** Structured prose with color usage descriptions + concrete codes

#### Background Colors Subsection

```javascript
function generateBackgroundColorsProse(colors) {
    const bgMain = colors.darkest;
    const bgSurface = colors.secondDarkest;

    return `
**Background Colors:**
- Main background: ${describeColor(bgMain)} (${bgMain}) creating ${mood} ambiance
- Surface/panel background: ${describeColor(bgSurface)} (${bgSurface}) for layering
- Elevated surfaces: ${hasElevation ? 'description' : 'Not used in this aesthetic'}
    `.trim();
}

function describeColor(hex) {
    // Analyze hex code to generate description
    const hue = getHue(hex);
    const saturation = getSaturation(hex);
    const lightness = getLightness(hex);

    if (lightness < 30) return `Dark ${hue}`;
    if (lightness > 70) return `Light ${hue}`;
    return `${capitalize(hue)}`;
}

// Examples:
// #2b2015 → "Dark brown"
// #ff6b35 → "Burnt orange"
// #f5e6d3 → "Cream"
// #404040 → "Dark gray"
```

#### Accent Colors Subsection

```javascript
function generateAccentColorsProse(colors) {
    const accents = colors.filter(isVibrant);
    const primary = accents[0];
    const secondary = accents[1];

    return `
**Accent Colors:**
- Primary accent: ${describeColor(primary)} (${primary}) for active elements and highlights
- Secondary accent: ${describeColor(secondary)} (${secondary}) for control bodies
- Hover/active state: ${generateHoverColor(primary)} for interaction feedback
    `.trim();
}

function generateHoverColor(baseColor) {
    // Describe how hover color relates to base
    // Usually: "Brightened [color]" or "Saturated [color]"
    return `Brightened ${describeColor(baseColor)}`;
}
```

#### Color Philosophy Subsection

```javascript
function generateColorPhilosophy(colors, patterns) {
    const palette = classifyPalette(colors);  // warm/cool/neutral
    const contrast = calculateContrast(colors);  // high/medium/low
    const mood = inferMood(colors);

    return `
${palette} palette emphasizing ${designStyle}. ${contrast} contrast between
${darkest} and ${lightest} ensures ${readability}. ${accentDescription} pop
against ${baseDescription} without overwhelming. Overall mood: ${mood}.
    `.trim();
}

// Example outputs:
// "Warm palette emphasizing vintage analog hardware. High contrast between
//  dark background and cream text ensures readability. Orange accents pop
//  against brown base without overwhelming. Overall mood: nostalgic, warm."

// "Cool palette with modern blue/gray tones. Medium contrast maintains
//  comfortable readability without harshness. Subtle accent colors integrate
//  seamlessly. Overall mood: professional, calm, technical."
```

---

### Typography Section

**Input:** Extracted fonts, sizes, weights
**Output:** Font descriptions with usage context

#### Font Families Subsection

```javascript
function generateFontFamiliesProse(fonts) {
    const heading = fonts.heading || fonts.body;  // Fallback if same
    const body = fonts.body;
    const mono = fonts.mono || body;  // Fallback if no mono

    return `
**Font Families:**
- Headings: ${describeFontFamily(heading)}
- Body text: ${describeFontFamily(body)}
- Values/numbers: ${describeFontFamily(mono)}
    `.trim();
}

function describeFontFamily(fontFamily) {
    // Extract font name and provide fallback description
    const name = extractFontName(fontFamily);  // "Roboto" from "'Roboto', sans-serif"
    const generic = extractGeneric(fontFamily);  // "sans-serif"

    if (name && name !== generic) {
        return `${name} or similar ${generic}`;
    }

    // Generic only - describe style
    if (generic === 'sans-serif') return 'Clean sans-serif (Roboto, Inter, or similar)';
    if (generic === 'serif') return 'Traditional serif (Georgia, Times, or similar)';
    if (generic === 'monospace') return 'Technical monospace (Roboto Mono, Courier, or similar)';
}

// Examples:
// Input: "'Roboto', sans-serif"
// Output: "Roboto or similar sans-serif"

// Input: "monospace"
// Output: "Technical monospace (Roboto Mono, Courier, or similar)"
```

#### Font Sizing Subsection

```javascript
function generateFontSizingProse(sizes) {
    return `
**Font Sizing:**
- Plugin title: ${sizeDescription(sizes.title)} (${sizes.title})
- Section labels: ${sizeDescription(sizes.section)} (${sizes.section})
- Parameter labels: ${sizeDescription(sizes.label)} (${sizes.label})
- Parameter values: ${sizeDescription(sizes.value)} (${sizes.value})
    `.trim();
}

function sizeDescription(size) {
    const px = parseInt(size);

    if (px >= 24) return 'Large';
    if (px >= 18) return 'Medium-large';
    if (px >= 14) return 'Medium';
    if (px >= 12) return 'Standard';
    return 'Small';
}

// Combine descriptor with concrete value:
// "Large (24px)" - size with context
```

#### Font Philosophy Subsection

```javascript
function generateFontPhilosophy(fonts, patterns) {
    const approach = inferTypographicApproach(fonts);

    const descriptors = [];

    if (fonts.mono) descriptors.push('technical/readable');
    if (fonts.serif) descriptors.push('traditional/classic');
    if (hasBoldWeights(fonts)) descriptors.push('strong hierarchy');
    if (hasLetterSpacing(patterns)) descriptors.push('airy spacing');

    return `
**Philosophy:**
${approach} approach with ${descriptors.join(', ')}. ${additionalNotes}.
    `.trim();
}

// Examples:
// "Technical approach with readable sans-serif and monospace values. Strong
//  hierarchy through size variation. Optimized for at-a-glance reading."

// "Classic approach with serif headings and clean body text. Subtle hierarchy
//  maintains elegance without drama."
```

---

### Controls Section

**Input:** Detected control types, extracted styles
**Output:** Detailed control descriptions

#### Knob Style Subsection

```javascript
function generateKnobStyleProse(knobData) {
    return `
**Visual Design:**
- Shape: ${knobData.shape}
- Size: ${sizeDescription(knobData.diameter)} (${knobData.diameter}) for ${interactionQuality}
- Indicator style: ${knobData.indicatorType}
- Border treatment: ${borderDescription(knobData.border)}

**Surface Treatment:**
- Base appearance: ${surfaceDescription(knobData)}
- Depth: ${depthDescription(knobData.shadow)}
- Tick marks: ${tickDescription(knobData)}
- Center indicator: ${centerDotDescription(knobData)}

**Interaction Feel:**
${interactionDescription(knobData)}
    `.trim();
}

function sizeDescription(diameter) {
    const size = parseInt(diameter);
    if (size < 50) return 'Small';
    if (size < 70) return 'Medium';
    if (size < 90) return 'Large';
    return 'Extra large';
}

function surfaceDescription(knob) {
    if (knob.hasGradient) return 'Gradient fill creating dimensionality';
    if (knob.hasTexture) return 'Textured surface (brushed metal/wood grain)';
    return 'Flat color fill';
}

function depthDescription(shadow) {
    if (shadow === 'none') return 'Flat 2D (no shadow)';

    const blur = extractBlur(shadow);  // Extract "8px" from shadow string
    const blurValue = parseInt(blur);

    if (blurValue < 5) return 'Subtle elevation (minimal shadow)';
    if (blurValue < 15) return 'Medium depth (noticeable shadow)';
    return 'Prominent 3D (dramatic shadow)';
}

function tickDescription(knob) {
    if (!knob.hasTicks) return 'None (smooth perimeter)';
    if (knob.tickCount > 20) return 'Fine marks creating precise scale';
    return `Prominent marks at intervals around perimeter`;
}

// Complete example output:
/*
**Visual Design:**
- Shape: Circle
- Size: Medium (70px) for comfortable mouse interaction
- Indicator style: Single pointer line from center to edge
- Border treatment: Subtle 2px solid border in accent color

**Surface Treatment:**
- Base appearance: Flat color fill (brass/copper tone)
- Depth: Subtle elevation (0 2px 8px rgba(0,0,0,0.3))
- Tick marks: Prominent marks at intervals around perimeter
- Center indicator: Small dot at rotation center for reference

**Interaction Feel:**
Tactile, hardware-like interaction. Smooth continuous rotation with visual
pointer feedback. Tick marks provide rough position awareness without
restricting movement.
*/
```

---

### Spacing & Layout Philosophy Section

**Input:** Extracted gap/padding/margin values
**Output:** Spacing descriptions with density characterization

```javascript
function generateSpacingProse(spacingData) {
    const density = inferDensity(spacingData.controlGap);

    return `
**Overall Density:**
${densityDescription(density)} spacing creates ${densityMood(density)} interface.
${densityRationale(density)}.

**Control Spacing:**
- Between controls: ${density} (${spacingData.controlGap}) ${gapEffect(density)}
- Vertical rhythm: ${rhythmDescription(spacingData)}
- Grouping strategy: ${groupingStrategy(spacingData)}

**Padding & Margins:**
- Edge margins: ${sizeDescription(spacingData.edgeMargin)} (${spacingData.edgeMargin}) from window edges
- Section padding: ${spacingData.sectionPadding} within logical groups
- Label-to-control gap: ${sizeDescription(spacingData.labelGap)} (${spacingData.labelGap}) to maintain association

**Layout Flexibility:**
${flexibilityGuidance(density)}
    `.trim();
}

function inferDensity(gap) {
    const px = parseInt(gap);
    if (px < 16) return 'tight';
    if (px < 28) return 'comfortable';
    return 'generous';
}

function densityDescription(density) {
    if (density === 'tight') return 'Compact';
    if (density === 'comfortable') return 'Balanced';
    return 'Generous';
}

function densityMood(density) {
    if (density === 'tight') return 'efficient, information-dense';
    if (density === 'comfortable') return 'balanced, readable';
    return 'comfortable, uncluttered';
}

function densityRationale(density) {
    if (density === 'tight') return 'Maximizes screen real estate for complex plugins';
    if (density === 'comfortable') return 'Balances information density with visual comfort';
    return 'Each control has room to breathe, preventing accidental touches';
}

function gapEffect(density) {
    if (density === 'tight') return 'allowing many controls in small space';
    if (density === 'comfortable') return 'creating clear visual separation';
    return 'creating comfortable breathing room';
}

// Example complete output:
/*
**Overall Density:**
Generous spacing creates comfortable, uncluttered interface. Each control
has room to breathe, preventing accidental touches and allowing focus.

**Control Spacing:**
- Between controls: Generous (32px) creating comfortable breathing room
- Vertical rhythm: Consistent 32px maintains grid alignment
- Grouping strategy: Related controls share same row/column with no extra gap

**Padding & Margins:**
- Edge margins: Comfortable (24px) from window edges
- Section padding: 20px within logical groups
- Label-to-control gap: Tight (8px) to maintain association

**Layout Flexibility:**
Spacing scales proportionally with window size. Maintain generous feel even
with more parameters by using multi-row grid rather than cramming single row.
*/
```

---

### Surface Treatment Section

**Input:** Texture detection, shadow analysis, border extraction
**Output:** Surface treatment descriptions

```javascript
function generateSurfaceTreatmentProse(surfaceData) {
    return `
### Textures

**Background:**
- Texture type: ${textureTypeDescription(surfaceData.backgroundTexture)}
- Intensity: ${textureIntensityDescription(surfaceData.textureOpacity)}
- Implementation: ${textureImplementation(surfaceData.backgroundTexture)}

**Control Surfaces:**
- Control texture: ${controlTextureDescription(surfaceData)}
- Consistency: ${textureConsistency(surfaceData)}

### Depth & Dimensionality

**Shadow Strategy:**
- Shadow presence: ${shadowPresence(surfaceData.shadows)}
- Shadow color: ${shadowColor(surfaceData.shadows)}
- Shadow blur: ${shadowBlur(surfaceData.shadows)}
- Typical shadow values: ${exampleShadow(surfaceData.shadows)}

**Elevation System:**
- Layers: ${elevationDescription(surfaceData)}
- How elevation is shown: ${elevationMethod(surfaceData)}

**Borders:**
- Border presence: ${borderPresence(surfaceData.borders)}
- Border style: ${borderStyleDescription(surfaceData.borders)}
- Border color approach: ${borderColorApproach(surfaceData.borders)}
    `.trim();
}

function textureTypeDescription(texture) {
    if (!texture || texture === 'none') return 'None (flat background)';
    if (texture.includes('noise')) return 'Subtle noise texture';
    if (texture.includes('paper')) return 'Paper texture for vintage feel';
    if (texture.includes('grain')) return 'Fine grain pattern';
    return 'Custom texture pattern';
}

function shadowPresence(shadows) {
    if (!shadows || shadows === 'none') return 'None (flat design)';

    const blur = extractBlur(shadows);
    const blurValue = parseInt(blur);

    if (blurValue < 5) return 'Subtle';
    if (blurValue < 15) return 'Medium';
    return 'Dramatic';
}

// Example complete output:
/*
### Textures

**Background:**
- Texture type: Subtle noise texture
- Intensity: Barely visible (opacity: 0.03)
- Implementation: Noise pattern overlay via CSS pseudo-element

**Control Surfaces:**
- Control texture: Flat (no texture on controls for clarity)
- Consistency: Background textured, controls flat for contrast

### Depth & Dimensionality

**Shadow Strategy:**
- Shadow presence: Subtle
- Shadow color: Black with low opacity
- Shadow blur: Soft (8-10px)
- Typical shadow values: 0 2px 8px rgba(0,0,0,0.2)

**Elevation System:**
- Layers: Subtle elevation differences between background and controls
- How elevation is shown: Soft shadows only (no borders or highlights)

**Borders:**
- Border presence: Selective (only on controls)
- Border style: Subtle solid lines (2px width)
- Border color approach: Accent color for integration with palette
*/
```

---

### Application Guidelines Section

**Input:** Parameter count from source mockup, control types, layout analysis
**Output:** Actionable adaptation guidelines

```javascript
function generateApplicationGuidelines(mockupData, patterns) {
    return `
### When Applying to New Plugin

**Parameter Count Adaptation:**
- **1-3 parameters:** ${layoutGuidance(1, 3, patterns)}
- **4-6 parameters:** ${layoutGuidance(4, 6, patterns)}
- **7-9 parameters:** ${layoutGuidance(7, 9, patterns)}
- **10+ parameters:** ${layoutGuidance(10, 99, patterns)}

**Control Type Mapping:**
- Float parameters → ${floatControlRecommendation(patterns)}
- Boolean parameters → ${boolControlRecommendation(patterns)}
- Choice parameters → ${choiceControlRecommendation(patterns)}

**Prominent Parameter Handling:**
${prominentParameterGuidance(patterns)}

### Customization Points

**Easy to Adjust:**
${easyCustomizations(patterns)}

**Core Identity Elements:**
${coreIdentityElements(patterns)}

### Integration Notes

${integrationNotes(patterns)}
    `.trim();
}

function layoutGuidance(min, max, patterns) {
    const density = patterns.spacing.density;

    if (max <= 3) {
        return 'Single horizontal row. Controls centered with generous spacing.';
    }

    if (max <= 6) {
        if (density === 'generous') {
            return '2x3 grid (2 rows, 3 columns). Maintain generous gaps for uncluttered feel.';
        }
        return '2x3 or 3x2 grid depending on parameter importance. Main controls in top row.';
    }

    if (max <= 9) {
        return '3x3 grid. Group related parameters by row or column. Maintain consistent gaps.';
    }

    return 'Larger grid (4x3 or 3x4) or scrollable container. Consider sectioning if parameters group logically.';
}

function prominentParameterGuidance(patterns) {
    // Infer from mockup if it has prominent params
    return `
Prominent parameters (Mix, Output, Dry/Wet) should be visually distinguished:
- Place as vertical sliders on right side
- Use 150% size of regular controls
- Different control color (lighter shade of accent)
- Separate with extra margin or border
    `.trim();
}

// Example complete output:
/*
### When Applying to New Plugin

**Parameter Count Adaptation:**
- **1-3 parameters:** Single horizontal row. Controls centered with generous spacing.
- **4-6 parameters:** 2x3 grid (2 rows, 3 columns). Maintain generous gaps for uncluttered feel.
- **7-9 parameters:** 3x3 grid. Group related parameters by row or column.
- **10+ parameters:** 4x3 grid or scrollable container with maintained spacing philosophy.

**Control Type Mapping:**
- Float parameters → Rotary knobs (default) or vertical sliders for prominent params
- Boolean parameters → Toggle buttons with on/off states
- Choice parameters → Buttons arranged horizontally (for 2-3 choices) or dropdown (4+ choices)

**Prominent Parameter Handling:**
Prominent parameters (Mix, Output, Dry/Wet) should be visually distinguished:
- Place as vertical sliders on right side
- Use 150% size of regular controls
- Different control color (lighter shade of accent)

### Customization Points

**Easy to Adjust:**
- Exact color hues (while maintaining warm palette)
- Control sizes (within 20% of specified)
- Spacing values (while maintaining density feel)
- Border styles and widths

**Core Identity Elements:**
- Warm vintage color palette (don't shift to cool tones)
- Brass/copper control coloring
- Tick mark knob styling
- Generous spacing philosophy
- Subtle texture presence

### Integration Notes

Browser compatibility: CSS requires modern browser (Chrome/Edge 90+, Firefox 88+).
Performance: Texture overlay is lightweight. No heavy effects.
Build setup: Standard WebView integration, no special requirements.
*/
```

---

## Example Color Codes Section

**Always include concrete CSS variable definitions as reference:**

```javascript
function generateExampleColorCodes(colors) {
    return `
## Example Color Codes

[Provide concrete color values as reference examples]

\`\`\`css
/* Backgrounds */
--bg-main: ${colors.background};
--bg-surface: ${colors.surface};

/* Accents */
--accent-primary: ${colors.accentPrimary};
--accent-hover: ${colors.accentHover};

/* Text */
--text-primary: ${colors.textPrimary};
--text-secondary: ${colors.textSecondary};

/* Controls */
--control-base: ${colors.controlBase};
--control-active: ${colors.controlActive};

/* Additional as needed */
\`\`\`
    `.trim();
}

// Example output:
/*
## Example Color Codes

```css
/* Backgrounds */
--bg-main: #2b2015;
--bg-surface: #3a2820;

/* Accents */
--accent-primary: #ff6b35;
--accent-hover: #ff8555;

/* Text */
--text-primary: #f5e6d3;
--text-secondary: #a08060;

/* Controls */
--control-base: #d97742;
--control-active: #ff6b35;
```
*/
```

---

## Quality Control Checklist

**Before finalizing aesthetic.md:**

- [ ] **All template sections present** (no missing sections)
- [ ] **No placeholder text** (all [brackets] filled)
- [ ] **Prose is descriptive** (not just values)
- [ ] **Concrete values included** (colors, sizes where appropriate)
- [ ] **Consistent terminology** (same words for same concepts)
- [ ] **Actionable guidance** (can be implemented from descriptions)
- [ ] **Vibe captured** (reader can visualize the aesthetic)
- [ ] **Example color codes valid** (all hex codes are 6 digits)
- [ ] **Application guidelines complete** (covers all parameter counts)
- [ ] **Integration notes present** (any special considerations noted)

---

## Success Criteria

**Prose generation successful when:**
- ✅ Each section filled with interpretable descriptions
- ✅ Balance between descriptive prose and concrete values
- ✅ Vibe/feeling captured in words
- ✅ Application guidance is actionable
- ✅ Format follows template structure exactly
- ✅ Human-readable and machine-parseable
- ✅ Result can be reliably interpreted by aesthetic-interpretation.md strategies
