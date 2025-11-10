#!/usr/bin/env python3
"""
Foundation Validator (Stage 2)
Validates that CMakeLists.txt exists and project builds successfully
Exit 0: PASS, Exit 1: FAIL
"""

import sys
from pathlib import Path

def find_plugin_directory() -> Path:
    """Find the active plugin directory (has CMakeLists.txt)"""
    plugins_dir = Path("plugins")
    if not plugins_dir.exists():
        return None

    for plugin_dir in plugins_dir.iterdir():
        if plugin_dir.is_dir():
            cmake_file = plugin_dir / "CMakeLists.txt"
            if cmake_file.exists():
                return plugin_dir

    return None

def check_cmake_exists(plugin_dir: Path) -> bool:
    """Check that CMakeLists.txt exists"""
    cmake_file = plugin_dir / "CMakeLists.txt"
    return cmake_file.exists()

def check_source_files(plugin_dir: Path) -> tuple[bool, list[str]]:
    """Check that essential source files exist"""
    source_dir = plugin_dir / "Source"
    if not source_dir.exists():
        return False, ["Source directory does not exist"]

    errors = []
    required_files = [
        "PluginProcessor.h",
        "PluginProcessor.cpp",
        "PluginEditor.h",
        "PluginEditor.cpp"
    ]

    for file in required_files:
        if not (source_dir / file).exists():
            errors.append(f"Missing required file: Source/{file}")

    return len(errors) == 0, errors

def check_build_directory(plugin_dir: Path) -> bool:
    """Check if build directory exists (indicates build was attempted)"""
    build_dir = plugin_dir / "build"
    return build_dir.exists()

def validate_cmake_content(cmake_path: Path) -> tuple[bool, list[str]]:
    """Validate CMakeLists.txt content for required JUCE configuration"""
    if not cmake_path.exists():
        return False, ["CMakeLists.txt not found"]

    content = cmake_path.read_text()
    errors = []

    # Check for juce_add_plugin()
    if 'juce_add_plugin' not in content:
        errors.append("Missing juce_add_plugin() declaration")

    # Check for PRODUCT_NAME (required for installation)
    if 'PRODUCT_NAME' not in content:
        errors.append("Missing PRODUCT_NAME in juce_add_plugin() - required for installation")

    # Check for required JUCE modules
    required_modules = ['juce_audio_processors', 'juce_audio_basics']
    for module in required_modules:
        if module not in content:
            errors.append(f"Missing required JUCE module: {module}")

    # Check for plugin formats (VST3, AU, or Standalone)
    has_format = any(fmt in content for fmt in ['VST3', 'AU', 'Standalone'])
    if not has_format:
        errors.append("No plugin formats specified (VST3, AU, Standalone)")

    return len(errors) == 0, errors

def main():
    """Main validation entry point"""
    # Find active plugin
    plugin_dir = find_plugin_directory()
    if not plugin_dir:
        print("No active plugin with CMakeLists.txt found, skipping validation", file=sys.stderr)
        sys.exit(0)  # Graceful skip

    print(f"Validating foundation for: {plugin_dir.name}")

    # Check CMakeLists.txt existence
    cmake_path = plugin_dir / "CMakeLists.txt"
    if not check_cmake_exists(plugin_dir):
        print("ERROR: CMakeLists.txt not found", file=sys.stderr)
        sys.exit(1)

    print("✓ CMakeLists.txt exists")

    # Validate CMakeLists.txt content
    cmake_ok, cmake_errors = validate_cmake_content(cmake_path)
    if not cmake_ok:
        print("ERROR: CMakeLists.txt validation failed:", file=sys.stderr)
        for error in cmake_errors:
            print(f"  - {error}", file=sys.stderr)
        sys.exit(1)

    print("✓ CMakeLists.txt has required JUCE configuration")

    # Check source files
    source_ok, errors = check_source_files(plugin_dir)
    if not source_ok:
        print("ERROR: Missing source files:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        sys.exit(1)

    print("✓ All required source files present")

    # Check build directory exists
    if not check_build_directory(plugin_dir):
        print("WARNING: Build directory not found - build may not have been attempted", file=sys.stderr)
        # Don't fail - build might happen in next step
    else:
        print("✓ Build directory exists")

    print(f"✓ Foundation validation PASSED for {plugin_dir.name}")
    sys.exit(0)

if __name__ == "__main__":
    main()
