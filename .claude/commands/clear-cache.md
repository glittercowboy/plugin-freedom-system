---
description: Clear validation cache for a plugin or entire system (project)
---

Clear cached validation results to force re-validation.

**Usage:**
- `/clear-cache [PluginName]` - Clear cache for specific plugin
- `/clear-cache --all` - Clear entire cache
- `/clear-cache --expired` - Remove only expired entries

**When to use:**
- After changing validation logic
- When suspecting stale cache results
- After system updates or configuration changes
- To force fresh validation on unchanged content

**Implementation:**

```bash
# Source cache utilities
source .claude/utils/validation-cache.sh

# Get the argument
ARG="${1:-}"

if [ "$ARG" = "--all" ]; then
    # Clear entire cache
    rm -f .claude/cache/validation-results.json
    echo "✓ Entire validation cache cleared"
    echo ""
    echo "Next validations will run fresh analysis on all content."

elif [ "$ARG" = "--expired" ]; then
    # Clean expired entries only
    clean_cache
    echo "✓ Expired cache entries removed"
    echo ""
    echo "Valid cache entries preserved. Run with --all to clear everything."

elif [ -n "$ARG" ]; then
    # Clear cache for specific plugin
    PLUGIN_NAME="$ARG"

    # Verify plugin exists
    if [ ! -d "plugins/$PLUGIN_NAME" ]; then
        echo "❌ Plugin not found: $PLUGIN_NAME"
        echo ""
        echo "Available plugins:"
        ls -1 plugins/ | grep -v "^\." | head -5
        exit 1
    fi

    # Clear all cache entries for this plugin (matches any validator)
    clear_cache ".*:${PLUGIN_NAME}"

    echo "✓ Cache cleared for $PLUGIN_NAME"
    echo ""
    echo "Next validations will run fresh analysis:"
    echo "- contract-checksums (integrity verification)"
    echo "- build-verification (if applicable)"

else
    # No arguments - show usage
    echo "Clear validation cache to force re-validation"
    echo ""
    echo "Usage:"
    echo "  /clear-cache [PluginName]  - Clear cache for specific plugin"
    echo "  /clear-cache --all         - Clear entire cache"
    echo "  /clear-cache --expired     - Remove only expired entries"
    echo ""
    echo "Examples:"
    echo "  /clear-cache MyPlugin      - Re-validate MyPlugin from scratch"
    echo "  /clear-cache --expired     - Clean up old cache entries"
    echo "  /clear-cache --all         - Force all future validations to run fresh"
fi
```

**Cache entry types:**
- `contract-checksums:[PluginName]` - Contract integrity verification (24h expiry)
- `build-verification:[PluginName]` - Build success status (6h expiry)

**Safety:**
- Clearing cache only forces re-validation, it doesn't modify any plugin files
- Cache is automatically rebuilt on next validation
- Expired entries are automatically cleaned at session start
