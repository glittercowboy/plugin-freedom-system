#!/usr/bin/env python3
"""
Parameter Validator (Stage 3)
Validates that all parameters from parameter-spec.md exist in PluginProcessor.cpp
Exit 0: PASS, Exit 1: FAIL
"""

import re
import sys
from pathlib import Path
from typing import Dict, Set, List, Tuple

def find_plugin_directory() -> Path:
    """Find the active plugin directory (has parameter-spec.md)"""
    plugins_dir = Path("plugins")
    if not plugins_dir.exists():
        return None

    for plugin_dir in plugins_dir.iterdir():
        if plugin_dir.is_dir():
            spec_file = plugin_dir / ".ideas" / "parameter-spec.md"
            if spec_file.exists():
                return plugin_dir

    return None

def parse_parameter_spec(spec_path: Path) -> Dict[str, Dict]:
    """Parse parameter-spec.md to extract parameter IDs, types, and ranges"""
    if not spec_path.exists():
        print(f"ERROR: parameter-spec.md not found at {spec_path}", file=sys.stderr)
        return {}

    content = spec_path.read_text()
    parameters = {}

    # Match parameter definitions in markdown table format
    # Format: | paramID | Type | Range | Default | ...
    param_pattern = r'\|\s*(\w+)\s*\|\s*(Float|Bool|Choice)\s*\|'

    for match in re.finditer(param_pattern, content):
        param_id = match.group(1)
        param_type = match.group(2)
        parameters[param_id] = {"type": param_type}

    return parameters

def parse_plugin_processor(processor_path: Path) -> Dict[str, Dict]:
    """Parse PluginProcessor.cpp to extract APVTS parameter declarations"""
    if not processor_path.exists():
        print(f"ERROR: PluginProcessor.cpp not found at {processor_path}", file=sys.stderr)
        return {}

    content = processor_path.read_text()
    parameters = {}

    # Match JUCE 8 parameter declarations with ParameterID
    # Pattern: AudioParameter(Float|Bool|Choice)(...ParameterID { "id", 1 }...)
    param_pattern = r'AudioParameter(Float|Bool|Choice)\s*\(\s*ParameterID\s*\{\s*"(\w+)"'

    for match in re.finditer(param_pattern, content):
        param_type = match.group(1)
        param_id = match.group(2)
        parameters[param_id] = {"type": param_type}

    return parameters

def validate_parameters(spec_params: Dict[str, Dict], code_params: Dict[str, Dict]) -> Tuple[bool, List[str]]:
    """Validate that spec parameters match code parameters"""
    errors = []

    spec_ids = set(spec_params.keys())
    code_ids = set(code_params.keys())

    # Check for missing parameters (in spec but not in code)
    missing = spec_ids - code_ids
    if missing:
        errors.append(f"Missing parameters in code: {', '.join(sorted(missing))}")

    # Check for extra parameters (in code but not in spec) - WARNING only
    extra = code_ids - spec_ids
    if extra:
        print(f"WARNING: Extra parameters in code (not in spec): {', '.join(sorted(extra))}", file=sys.stderr)

    # Check type mapping for common parameters
    for param_id in spec_ids & code_ids:
        spec_type = spec_params[param_id]["type"]
        code_type = code_params[param_id]["type"]

        # Validate type mapping
        if spec_type != code_type:
            errors.append(f"Type mismatch for '{param_id}': spec has {spec_type}, code has {code_type}")

    return len(errors) == 0, errors

def main():
    """Main validation entry point"""
    # Find active plugin
    plugin_dir = find_plugin_directory()
    if not plugin_dir:
        print("No active plugin with parameter-spec.md found, skipping validation", file=sys.stderr)
        sys.exit(0)  # Graceful skip

    print(f"Validating parameters for: {plugin_dir.name}")

    # Parse spec and code
    spec_path = plugin_dir / ".ideas" / "parameter-spec.md"
    processor_path = plugin_dir / "Source" / "PluginProcessor.cpp"

    spec_params = parse_parameter_spec(spec_path)
    code_params = parse_plugin_processor(processor_path)

    if not spec_params:
        print("WARNING: No parameters found in parameter-spec.md", file=sys.stderr)
        sys.exit(0)  # Skip if no parameters defined yet

    if not code_params:
        print("ERROR: No APVTS parameters found in PluginProcessor.cpp", file=sys.stderr)
        sys.exit(1)

    # Validate
    success, errors = validate_parameters(spec_params, code_params)

    if success:
        print(f"✓ All {len(spec_params)} parameters from spec present in code")
        sys.exit(0)
    else:
        print("✗ Parameter validation FAILED:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        sys.exit(1)

if __name__ == "__main__":
    main()
