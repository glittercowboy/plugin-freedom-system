#!/usr/bin/env python3
"""
DSP Component Validator (Stage 4)
Validates that all DSP components from architecture.md exist in PluginProcessor
Exit 0: PASS, Exit 1: FAIL
"""

import re
import sys
from pathlib import Path
from typing import Set, List, Tuple

def find_plugin_directory() -> Path:
    """Find the active plugin directory (has architecture.md)"""
    plugins_dir = Path("plugins")
    if not plugins_dir.exists():
        return None

    for plugin_dir in plugins_dir.iterdir():
        if plugin_dir.is_dir():
            arch_file = plugin_dir / ".ideas" / "architecture.md"
            if arch_file.exists():
                return plugin_dir

    return None

def parse_architecture_components(arch_path: Path) -> Set[str]:
    """Parse architecture.md to extract DSP component names"""
    if not arch_path.exists():
        print(f"ERROR: architecture.md not found at {arch_path}", file=sys.stderr)
        return set()

    content = arch_path.read_text()
    components = set()

    # Match JUCE DSP component references
    # Pattern: juce::dsp::ComponentName
    component_pattern = r'juce::dsp::(\w+)'

    for match in re.finditer(component_pattern, content):
        component_name = match.group(1)
        components.add(component_name)

    return components

def check_includes(processor_cpp: Path) -> bool:
    """Check that juce_dsp header is included"""
    if not processor_cpp.exists():
        return False

    content = processor_cpp.read_text()
    return '#include <juce_dsp/juce_dsp.h>' in content

def find_component_declarations(processor_h: Path, components: Set[str]) -> Tuple[Set[str], List[str]]:
    """Find DSP component member variable declarations in PluginProcessor.h"""
    if not processor_h.exists():
        return set(), []

    content = processor_h.read_text()
    found_components = set()
    warnings = []

    for component in components:
        # Pattern: juce::dsp::ComponentName<float> memberName;
        # or: juce::dsp::ComponentName memberName;
        pattern = rf'juce::dsp::{component}(?:<[^>]+>)?\s+\w+\s*;'

        if re.search(pattern, content):
            found_components.add(component)
        else:
            # Also check for using statements
            using_pattern = rf'using\s+\w+\s*=\s*juce::dsp::{component}'
            if re.search(using_pattern, content):
                found_components.add(component)

    return found_components, warnings

def check_prepare_calls(processor_cpp: Path, components: Set[str]) -> List[str]:
    """Check if components are prepared in prepareToPlay()"""
    if not processor_cpp.exists():
        return []

    content = processor_cpp.read_text()
    warnings = []

    # Find prepareToPlay() method
    prepare_pattern = r'void\s+\w*AudioProcessor::prepareToPlay\s*\([^)]*\)\s*\{([^}]*(?:\{[^}]*\}[^}]*)*)\}'
    prepare_match = re.search(prepare_pattern, content, re.DOTALL)

    if not prepare_match:
        warnings.append("prepareToPlay() method not found")
        return warnings

    prepare_body = prepare_match.group(1)

    # Check for .prepare( calls
    if '.prepare(' not in prepare_body:
        warnings.append("No .prepare() calls found in prepareToPlay() - components may not be initialized")

    return warnings

def main():
    """Main validation entry point"""
    # Find active plugin
    plugin_dir = find_plugin_directory()
    if not plugin_dir:
        print("No active plugin with architecture.md found, skipping validation", file=sys.stderr)
        sys.exit(0)  # Graceful skip

    print(f"Validating DSP components for: {plugin_dir.name}")

    # Parse architecture
    arch_path = plugin_dir / ".ideas" / "architecture.md"
    components = parse_architecture_components(arch_path)

    if not components:
        print("No juce::dsp components found in architecture.md, skipping DSP validation")
        sys.exit(0)  # Skip if no DSP components specified

    print(f"Found {len(components)} DSP components in architecture: {', '.join(sorted(components))}")

    # Check code files
    processor_h = plugin_dir / "Source" / "PluginProcessor.h"
    processor_cpp = plugin_dir / "Source" / "PluginProcessor.cpp"

    # Check includes
    if not check_includes(processor_cpp):
        print("ERROR: #include <juce_dsp/juce_dsp.h> not found in PluginProcessor.cpp", file=sys.stderr)
        sys.exit(1)

    # Check component declarations
    found_components, decl_warnings = find_component_declarations(processor_h, components)

    missing = components - found_components
    if missing:
        print(f"ERROR: Missing DSP components in PluginProcessor.h: {', '.join(sorted(missing))}", file=sys.stderr)
        sys.exit(1)

    # Check prepare calls (warnings only)
    prepare_warnings = check_prepare_calls(processor_cpp, components)
    for warning in prepare_warnings:
        print(f"WARNING: {warning}", file=sys.stderr)

    print(f"✓ All {len(components)} DSP components from architecture declared in code")
    sys.exit(0)

if __name__ == "__main__":
    main()
