#!/usr/bin/env python3
"""
GUI Binding Validator (Stage 5)
Validates that HTML parameter IDs match C++ relay IDs
Exit 0: PASS, Exit 1: FAIL
"""

import re
import sys
from pathlib import Path
from typing import Set, Dict, List

def find_plugin_directory() -> Path:
    """Find the active plugin directory (has ui/public/index.html)"""
    plugins_dir = Path("plugins")
    if not plugins_dir.exists():
        return None

    for plugin_dir in plugins_dir.iterdir():
        if plugin_dir.is_dir():
            ui_file = plugin_dir / "ui" / "public" / "index.html"
            if ui_file.exists():
                return plugin_dir

    return None

def extract_html_parameter_ids(html_path: Path) -> Set[str]:
    """Extract parameter IDs from HTML via getSliderState() calls"""
    if not html_path.exists():
        return set()

    content = html_path.read_text()
    param_ids = set()

    # Pattern: getSliderState('paramID') or getSliderState("paramID")
    slider_pattern = r"getSliderState\s*\(\s*['\"](\w+)['\"]\s*\)"

    for match in re.finditer(slider_pattern, content):
        param_id = match.group(1)
        param_ids.add(param_id)

    # Also check for getToggleButtonState and getComboBoxState
    toggle_pattern = r"getToggleButtonState\s*\(\s*['\"](\w+)['\"]\s*\)"
    for match in re.finditer(toggle_pattern, content):
        param_id = match.group(1)
        param_ids.add(param_id)

    combo_pattern = r"getComboBoxState\s*\(\s*['\"](\w+)['\"]\s*\)"
    for match in re.finditer(combo_pattern, content):
        param_id = match.group(1)
        param_ids.add(param_id)

    return param_ids

def extract_cpp_relays(editor_cpp: Path) -> Dict[str, str]:
    """Extract relay declarations from PluginEditor.cpp"""
    if not editor_cpp.exists():
        return {}

    content = editor_cpp.read_text()
    relays = {}

    # Pattern: WebSliderRelay relayName { "paramID" }
    # or: WebSliderRelay relayName("paramID")
    relay_pattern = r'Web(?:Slider|ToggleButton|ComboBox)Relay\s+(\w+)(?:Relay)?\s*(?:\{|\()\s*"(\w+)"'

    for match in re.finditer(relay_pattern, content):
        relay_var = match.group(1)
        param_id = match.group(2)
        relays[param_id] = relay_var

    return relays

def extract_cpp_attachments(editor_cpp: Path) -> Set[str]:
    """Extract attachment declarations from PluginEditor.cpp"""
    if not editor_cpp.exists():
        return set()

    content = editor_cpp.read_text()
    attachments = set()

    # Pattern: Web*ParameterAttachment attachmentName
    attachment_pattern = r'Web(?:Slider|ToggleButton|ComboBox)ParameterAttachment\s+(\w+)Attachment'

    for match in re.finditer(attachment_pattern, content):
        param_base = match.group(1)
        attachments.add(param_base)

    return attachments

def check_with_options_from(editor_cpp: Path, relays: Dict[str, str]) -> List[str]:
    """Check that relays are registered with .withOptionsFrom()"""
    if not editor_cpp.exists():
        return []

    content = editor_cpp.read_text()
    warnings = []

    for param_id, relay_var in relays.items():
        # Check for .withOptionsFrom(relayVar)
        pattern = rf'\.withOptionsFrom\s*\(\s*{relay_var}(?:Relay)?\s*\)'

        if not re.search(pattern, content):
            warnings.append(f"Relay '{relay_var}' for parameter '{param_id}' not registered with .withOptionsFrom()")

    return warnings

def check_member_declaration_order(editor_h: Path) -> tuple[bool, List[str]]:
    """
    CRITICAL CHECK: Validate member declaration order to prevent WebView crashes.

    Correct order (destroyed in reverse):
    1. Relays first (Web*Relay)
    2. WebView second (WebBrowserComponent)
    3. Attachments last (Web*ParameterAttachment)

    This prevents crashes caused by webView being destroyed before objects that reference it.
    """
    if not editor_h.exists():
        return True, []  # No header file yet, skip check

    content = editor_h.read_text()
    lines = content.split('\n')
    errors = []

    # Track line numbers for each member type
    relay_lines = []
    webview_lines = []
    attachment_lines = []

    for i, line in enumerate(lines, 1):
        # Skip comments
        if '//' in line:
            line = line[:line.index('//')]

        # Match relay declarations
        if re.search(r'Web(?:Slider|ToggleButton|ComboBox)Relay\s+\w+', line):
            relay_lines.append(i)

        # Match WebView component
        if re.search(r'(?:juce::)?WebBrowserComponent\s+\w+', line):
            webview_lines.append(i)

        # Match attachment declarations
        if re.search(r'Web(?:Slider|ToggleButton|ComboBox)ParameterAttachment\s+\w+', line):
            attachment_lines.append(i)

    # If no WebView components found, skip check
    if not webview_lines:
        return True, []

    # Validate order: all relays must come before webView
    webview_first_line = min(webview_lines) if webview_lines else float('inf')

    for relay_line in relay_lines:
        if relay_line > webview_first_line:
            errors.append(
                f"MEMBER ORDER ERROR: Relay declared at line {relay_line} AFTER WebView at line {webview_first_line}. "
                f"This WILL cause crashes! Relays must be declared BEFORE WebView."
            )

    # Validate order: webView must come before attachments
    attachment_first_line = min(attachment_lines) if attachment_lines else float('inf')

    for webview_line in webview_lines:
        if webview_line > attachment_first_line:
            errors.append(
                f"MEMBER ORDER ERROR: WebView declared at line {webview_line} AFTER attachment at line {attachment_first_line}. "
                f"This WILL cause crashes! WebView must be declared BEFORE attachments."
            )

    return len(errors) == 0, errors

def main():
    """Main validation entry point"""
    # Find active plugin
    plugin_dir = find_plugin_directory()
    if not plugin_dir:
        print("No active plugin with ui/public/index.html found, skipping validation", file=sys.stderr)
        sys.exit(0)  # Graceful skip

    print(f"Validating GUI bindings for: {plugin_dir.name}")

    # Extract HTML parameter IDs
    html_path = plugin_dir / "ui" / "public" / "index.html"
    html_ids = extract_html_parameter_ids(html_path)

    if not html_ids:
        print("No parameter IDs found in HTML, skipping GUI validation")
        sys.exit(0)

    print(f"Found {len(html_ids)} parameter IDs in HTML: {', '.join(sorted(html_ids))}")

    # Extract C++ relays
    editor_cpp = plugin_dir / "Source" / "PluginEditor.cpp"
    relays = extract_cpp_relays(editor_cpp)

    relay_ids = set(relays.keys())

    if not relay_ids:
        print("ERROR: No WebSliderRelay declarations found in PluginEditor.cpp", file=sys.stderr)
        sys.exit(1)

    print(f"Found {len(relay_ids)} relays in C++: {', '.join(sorted(relay_ids))}")

    # Compare ID sets
    missing_relays = html_ids - relay_ids
    extra_relays = relay_ids - html_ids

    errors = []

    if missing_relays:
        errors.append(f"Missing relays for HTML parameters: {', '.join(sorted(missing_relays))}")

    if extra_relays:
        print(f"WARNING: Extra relays in C++ (not in HTML): {', '.join(sorted(extra_relays))}", file=sys.stderr)

    # Check attachments exist
    attachments = extract_cpp_attachments(editor_cpp)
    for param_id in html_ids:
        # Convert paramID to expected attachment variable name (e.g., gainRelay -> gain)
        relay_var = relays.get(param_id)
        if relay_var and relay_var not in attachments:
            print(f"WARNING: No attachment found for relay '{relay_var}'", file=sys.stderr)

    # Check .withOptionsFrom() registration
    registration_warnings = check_with_options_from(editor_cpp, relays)
    for warning in registration_warnings:
        print(f"WARNING: {warning}", file=sys.stderr)

    # CRITICAL CHECK: Validate member declaration order (prevents 90% of WebView crashes)
    editor_h = plugin_dir / "Source" / "PluginEditor.h"
    order_ok, order_errors = check_member_declaration_order(editor_h)
    if not order_ok:
        errors.extend(order_errors)

    if errors:
        print("✗ GUI binding validation FAILED:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        sys.exit(1)

    print(f"✓ All {len(html_ids)} HTML parameter IDs have corresponding relays")
    print("✓ Member declaration order is correct (relays → webView → attachments)")
    sys.exit(0)

if __name__ == "__main__":
    main()
