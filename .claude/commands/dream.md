---
name: dream
description: Explore plugin ideas without implementing
argument-hint: "[concept or PluginName?]"
---

# /dream

## Behavior

<argument_handling>
  <parameter name="concept_or_name" optional="true">
    If provided: Check PLUGINS.md for existing plugin match
    If not provided: Present interactive discovery menu
  </parameter>
</argument_handling>

## Routing

**If no argument provided:**
1. Present discovery menu with 5 options:
   ```
   What would you like to explore?

   1. New plugin idea
   2. Improve existing plugin
   3. Create UI mockup
   4. Create aesthetic template
   5. Research problem
   ```
2. Capture user selection (1-5)
3. Invoke corresponding skill via Skill tool:
   - Option 1 → plugin-ideation skill (new mode)
   - Option 2 → plugin-ideation skill (improvement mode)
   - Option 3 → ui-mockup skill
   - Option 4 → aesthetic-dreaming skill
   - Option 5 → deep-research skill

**If plugin name provided:**
1. Check PLUGINS.md for plugin existence
2. If exists: Present plugin-specific menu (improvement, mockup, research) and invoke selected skill with plugin context
3. If not exists: Invoke plugin-ideation skill with concept as seed

## Preconditions

<preconditions enforcement="conditional">
  <check condition="plugin_name_provided">
    <target>PLUGINS.md</target>
    <validation>Verify plugin existence to determine routing</validation>
    <on_found>Route to plugin-specific improvement menu</on_found>
    <on_not_found>Route to plugin-ideation skill (new plugin mode)</on_not_found>
  </check>

  <check condition="no_argument">
    <validation>No preconditions - present discovery menu</validation>
  </check>
</preconditions>

## State Files

<state_files>
  <reads>
    <file>PLUGINS.md</file>
    <purpose>Check plugin existence when name argument provided</purpose>
  </reads>

  <writes>
    <none>This command only invokes skills that manage their own state</none>
  </writes>
</state_files>

## Output

<output_contract>
  All /dream operations create documentation only - no implementation.

  Output locations managed by invoked skills:
  - plugin-ideation → `plugins/[Name]/.ideas/creative-brief.md`
  - plugin-ideation → `plugins/[Name]/.ideas/improvements/[feature].md`
  - ui-mockup → `plugins/[Name]/.ideas/mockups/v[N]-*`
  - aesthetic-dreaming → `.claude/aesthetics/[aesthetic-id]/aesthetic.md`
  - deep-research → `troubleshooting/[category]/[problem].md`
</output_contract>
