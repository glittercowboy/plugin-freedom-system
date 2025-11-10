---
name: dream
description: Explore plugin ideas without implementing
---

# /dream

When user runs `/dream [concept?]`, invoke the plugin-ideation skill.

## Behavior

**Without argument:**
Present interactive menu:
```
What would you like to explore?

1. New plugin idea
2. Improve existing plugin
3. Create UI mockup
4. Research problem
```

Route based on selection:
- Option 1 → plugin-ideation skill (new plugin mode)
- Option 2 → plugin-ideation skill (improvement mode)
- Option 3 → ui-mockup skill
- Option 4 → deep-research skill

**With plugin name:**
```bash
/dream [PluginName]
```

Check if plugin exists in PLUGINS.md:
- If exists: Present plugin-specific menu (improvement, mockup, research)
- If new: Route to plugin-ideation skill for creative brief

## Preconditions

None - brainstorming is always available.

## Output

All /dream operations create documentation in `.ideas/` subdirectories:
- Creative briefs: `plugins/[Name]/.ideas/creative-brief.md`
- Improvement proposals: `plugins/[Name]/.ideas/improvements/[feature].md`
- UI mockups: `plugins/[Name]/.ideas/mockups/v[N]-*`
- Research findings: Documentation with solutions

Nothing is implemented - this is purely exploratory.
