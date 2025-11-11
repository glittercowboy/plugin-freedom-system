<objective>
Implement a new skill called aesthetic-dreaming that allows users to create aesthetic templates without first creating a plugin. The skill should capture visual design concepts through adaptive questioning, generate a structured aesthetic.md file, and optionally create test preview mockups demonstrating the aesthetic applied to different plugin types.

This solves the friction of needing to create a full plugin just to design and save an aesthetic template. Users can build a library of visual design systems before creating any actual plugins.
</objective>

<context>
The Plugin Freedom System uses skills (markdown instruction documents) that Claude executes via tools. The aesthetic template system was just rewritten to use structured prose descriptions in aesthetic.md format.

Current system:
- Skills are in `.claude/skills/[skill-name]/` with SKILL.md, references/, and assets/
- Aesthetics stored in `.claude/aesthetics/[aesthetic-id]/` with aesthetic.md, preview.html, metadata.json
- ui-template-library skill handles save/apply operations for aesthetics
- plugin-ideation skill uses adaptive questioning pattern: free-form → gap analysis → 4 questions → decision gate → loop

Entry point: `/dream` command (in `.claude/commands/dream.md`) which presents menu of creative options

Read these for context:
@.claude/skills/plugin-ideation/SKILL.md - Study the adaptive questioning pattern (Phases 1-3.5)
@.claude/skills/ui-template-library/SKILL.md - Understand aesthetic.md format and apply operation
@.claude/skills/ui-template-library/assets/aesthetic-template.md - The aesthetic.md template structure
@.claude/commands/dream.md - Current dream command structure
</context>

<requirements>
1. **Create aesthetic-dreaming skill** with:
   - SKILL.md following same patterns as plugin-ideation
   - Adaptive questioning focused on visual design (not DSP/parameters)
   - Generate aesthetic.md from captured concept
   - Optional test preview generation

2. **Implement adaptive questioning flow:**
   - Phase 1: Free-form collection ("What aesthetic are you envisioning?")
   - Phase 2: Gap analysis (extract what's covered, identify missing visual concepts)
   - Phase 3: Generate 4 questions via AskUserQuestion based on gaps
   - Phase 3.5: Decision gate (finalize / 4 more questions / add context)
   - Loop until user finalizes

3. **Question prioritization tiers:**
   - Tier 1 (Critical): Overall vibe, color philosophy, control style
   - Tier 2 (Visual): Textures, shadows, typography, spacing
   - Tier 3 (Context): Best suited for, technical patterns, inspirations

4. **Test preview selection after finalization:**
   - Use AskUserQuestion with multiSelect: true
   - 4 standard options: Simple compressor, Complex reverb, Drum machine, Simple synth
   - "Other" option for custom test previews
   - User can select 0-4 options (skip previews entirely if desired)

5. **Generate aesthetic.md and test previews:**
   - Create structured aesthetic.md following template (all 12 sections)
   - For each selected test preview, invoke ui-template-library "apply" operation
   - Store in `.claude/aesthetics/[aesthetic-id]/test-previews/`

6. **Update /dream command:**
   - Add "Create aesthetic template" as option 4
   - Route to aesthetic-dreaming skill
</requirements>

<implementation_approach>
Thoroughly analyze plugin-ideation's adaptive pattern and adapt it for visual design:

1. **Adaptive questioning:** Don't use rigid pre-scripted questions. Extract from free-form response, identify gaps, generate contextual questions. Never ask about information already provided.

2. **Question generation examples:**
   - If user says "retro 80s neon", don't ask "What's the vibe?" (already answered)
   - DO ask: "Primary color palette?" "Control style?" "Texture approach?"
   - Build questions from what's missing, not a fixed script

3. **Test plugin specifications** (store in assets/test-plugin-specs.json):
   - Simple compressor: 4 params (threshold, ratio, attack, release)
   - Complex reverb: 8 params (size, damping, predelay, mix, tone, width, decay, diffusion)
   - Drum machine: 16-pad grid + 4 controls (tempo, swing, volume, pattern)
   - Simple synth: 6 oscillator controls

4. **Integration with ui-template-library:**
   - Generate aesthetic.md first
   - Then invoke ui-template-library "apply" for each test preview
   - Pass temporary parameter specs for test plugins

5. **Decision menus throughout:**
   - After question batches: finalize / more questions / add context
   - After finalization: select test previews
   - After generation: open previews / refine / save to library / discard
</implementation_approach>

<output>
Create these files:

`.claude/skills/aesthetic-dreaming/SKILL.md` - Main skill implementation (~600-800 lines)
- Frontmatter with allowed-tools: Read, Write, Bash, AskUserQuestion
- Adaptive questioning workflow matching plugin-ideation pattern
- Test preview selection and generation
- Integration with ui-template-library

`.claude/skills/aesthetic-dreaming/assets/test-plugin-specs.json` - Test plugin parameter specifications
- 4 standard test plugin definitions
- Each with parameters, types, ranges, layout hints

`.claude/skills/aesthetic-dreaming/references/aesthetic-questions.md` - Question bank and gap analysis strategies
- Visual design question categories
- Gap identification patterns
- Question generation examples

Update this file:
`.claude/commands/dream.md` - Add aesthetic option
- Option 4: "Create aesthetic template"
- Route to aesthetic-dreaming skill
</output>

<constraints>
- MUST follow plugin-ideation's adaptive pattern (not rigid script)
- MUST use AskUserQuestion tool with max 4 options + Other
- MUST generate complete aesthetic.md with all 12 sections
- MUST allow skipping test previews (user choice)
- MUST store test previews in aesthetic directory, not plugins/
- MUST maintain integration with ui-template-library skill
</constraints>

<verification>
After implementation, verify:
1. Skill follows adaptive questioning pattern from plugin-ideation
2. Questions focus on visual design concepts
3. aesthetic.md generation follows structured template
4. Test preview selection uses multiSelect properly
5. Integration with ui-template-library "apply" operation works
6. /dream command routes correctly to new skill
7. Test previews generate only for selected options
8. Files stored in correct locations (.claude/aesthetics/, not plugins/)
</verification>

<success_criteria>
- Users can create aesthetic templates without creating plugins
- Adaptive questioning captures visual design intent effectively
- Generated aesthetic.md follows exact template structure
- Test previews demonstrate aesthetic across different plugin types
- Integration with existing aesthetic system works seamlessly
- /dream aesthetic provides smooth creative flow
</success_criteria>