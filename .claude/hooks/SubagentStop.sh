#!/bin/bash
# SubagentStop hook - Deterministic validation after each subagent completes
# Layer 1 validation: Fast pattern matching (1-3s), blocks workflow on failure

INPUT=$(cat)
SUBAGENT=$(echo "$INPUT" | jq -r '.subagent_name // empty' 2>/dev/null)

# Gracefully skip if we can't extract subagent name
if [ -z "$SUBAGENT" ]; then
  echo "Hook not relevant: no subagent_name in input, skipping gracefully"
  exit 0
fi

# Check relevance FIRST - only validate our implementation subagents
if [[ ! "$SUBAGENT" =~ ^(foundation-agent|shell-agent|dsp-agent|gui-agent)$ ]]; then
  echo "Hook not relevant for subagent: $SUBAGENT, skipping gracefully"
  exit 0
fi

# Extract plugin name from context if available
PLUGIN_NAME=$(echo "$INPUT" | jq -r '.plugin_name // empty' 2>/dev/null)

# Execute hook logic based on subagent
case "$SUBAGENT" in
  foundation-agent)
    echo "Validating foundation-agent output (Stage 2)..."
    python3 .claude/hooks/validators/validate-foundation.py
    RESULT=$?
    if [ $RESULT -ne 0 ]; then
      echo "Foundation validation FAILED: CMakeLists.txt missing or build failed" >&2
      exit 2  # Block workflow
    fi
    echo "Foundation validation PASSED"
    ;;

  shell-agent)
    echo "Validating shell-agent output (Stage 3)..."
    python3 .claude/hooks/validators/validate-parameters.py
    RESULT=$?
    if [ $RESULT -ne 0 ]; then
      echo "Parameter validation FAILED: Parameters from spec missing in code" >&2
      exit 2  # Block workflow
    fi
    echo "Parameter validation PASSED"
    ;;

  dsp-agent)
    echo "Validating dsp-agent output (Stage 4)..."
    python3 .claude/hooks/validators/validate-dsp-components.py
    RESULT=$?
    if [ $RESULT -ne 0 ]; then
      echo "DSP component validation FAILED: Components from architecture missing" >&2
      exit 2  # Block workflow
    fi
    echo "DSP component validation PASSED"
    ;;

  gui-agent)
    echo "Validating gui-agent output (Stage 5)..."
    python3 .claude/hooks/validators/validate-gui-bindings.py
    RESULT=$?
    if [ $RESULT -ne 0 ]; then
      echo "GUI binding validation FAILED: HTML IDs don't match relay IDs" >&2
      exit 2  # Block workflow
    fi
    echo "GUI binding validation PASSED"
    ;;
esac

exit 0
