---
name: system-setup
description: Validates and configures all dependencies required for the Plugin Freedom System. This is a STANDALONE skill that runs BEFORE plugin workflows begin. It checks for Python, build tools, CMake, JUCE, and pluginval, optionally installing missing dependencies with user approval. Configuration is saved to .claude/system-config.json for use by other skills.
allowed-tools:
  - Bash # For dependency checks and installation
  - Read # For checking existing config
  - Write # For creating system-config.json
  - Edit # For updating config
preconditions:
  - None - this is the entry point for new users
---

# system-setup Skill

**Purpose:** Validate and configure all dependencies required for JUCE plugin development in the Plugin Freedom System.

## Overview

This skill ensures new users can get started without friction by:
- Detecting the current platform (macOS, Linux, Windows)
- Checking for required dependencies (Python, build tools, CMake, JUCE, pluginval)
- Offering automated installation where possible
- Guiding manual installation when automation isn't available
- Validating that all tools are functional
- Saving validated configuration for build scripts

**Target platform:** macOS (extensible to Windows/Linux later)

**User experience:** Interactive, with clear choices between automated and guided setup

<test_mode_protocol>
IF --test=SCENARIO argument provided:
  MUST pass --test=$SCENARIO to ALL system-check.sh invocations
  MUST display test mode banner at start:
    [TEST MODE: $SCENARIO]
    Using mock data - no actual system changes will be made
  MUST NOT make actual system changes
  MUST use mock data from test scenarios
</test_mode_protocol>

<precondition_gate>
None required - this is the entry point skill
Can be invoked anytime, creates system-config.json
No dependencies on PLUGINS.md or .continue-here.md
</precondition_gate>

<delegation_rules>
  <rule>This skill is STANDALONE - it does NOT delegate to other skills or subagents</rule>
  <rule>All validation logic is handled by system-check.sh bash script</rule>
  <rule>This skill is invoked BEFORE plugin workflows begin</rule>
  <rule>DO NOT invoke plugin-workflow, plugin-planning, or any other plugin skills from here</rule>
</delegation_rules>

<cross_references>
  <file>references/platform-requirements.md</file>
  <file>references/juce-setup-guide.md</file>
  <script>assets/system-check.sh</script>
</cross_references>

---

## Required Dependencies

### Python 3.8+
- **Purpose:** Build scripts and automation
- **Check:** `python3 --version`
- **Minimum version:** 3.8
- **Auto-install (macOS):** Homebrew (`brew install python3`) or download from python.org

### Build Tools
- **macOS:** Xcode Command Line Tools
  - **Check:** `xcode-select -p`
  - **Auto-install:** `xcode-select --install`
- **Linux:** GCC/Clang, build-essential
- **Windows:** Visual Studio 2019+

### CMake 3.15+
- **Purpose:** Build system for JUCE projects
- **Check:** `cmake --version`
- **Minimum version:** 3.15
- **Auto-install (macOS):** Homebrew (`brew install cmake`)

### JUCE 8.0.0+
- **Purpose:** Audio plugin framework
- **Check:** Search standard locations, validate version in `modules/juce_core/juce_core.h`
- **Standard locations:**
  - `~/JUCE`
  - `/Applications/JUCE`
  - `/usr/local/JUCE`
- **Auto-install:** Download from juce.com, extract to `~/JUCE`

### pluginval
- **Purpose:** Plugin validation tool
- **Check:** `pluginval --version` or check standard locations
- **Standard locations:**
  - `/usr/local/bin/pluginval`
  - `~/bin/pluginval`
- **Auto-install:** Download from GitHub releases, install to `/usr/local/bin`

---

## Skill Entry Point

When invoked via `/setup` command:

**Check for test mode first:**
- If user provided `--test=SCENARIO` argument, set TEST_MODE variable
- Pass test mode to all system-check.sh invocations via `--test=$SCENARIO`
- Show test mode banner if active:
  ```
  [TEST MODE: $SCENARIO]
  Using mock data - no actual system changes will be made
  ```

1. **Welcome message:**
   ```
   System Setup - Plugin Freedom System

   This will validate and configure all dependencies needed for JUCE plugin development.

   How would you like to proceed?
   1. Automated setup (install missing dependencies automatically)
   2. Guided setup (step-by-step instructions for manual installation)
   3. Check only (detect what's installed, no changes)
   4. Exit

   Choose (1-4): _
   ```

2. **Store user choice and proceed to platform detection**

<mode_definitions>
After user selects mode, ALL subsequent dependency validations MUST respect this mode:

MODE: "automated"
  - Attempt installations with user confirmation
  - Fall back to guided instructions if automation fails
  - Present installation menus for missing dependencies
  - Wait for user choice at each menu

MODE: "guided"
  - NEVER attempt automated installation
  - Display manual instructions from references/
  - Wait for user to complete manual steps
  - Verify installation after user confirms completion

MODE: "check-only"
  - NEVER offer installation (automated or guided)
  - Report each dependency status (found/missing/wrong-version)
  - Continue through all dependencies regardless of failures
  - Generate report at end showing what needs attention
</mode_definitions>

---

<critical_sequence name="dependency-validation" enforcement="strict">
**These dependencies MUST be validated in order. Each dependency may block on user input.**

<execution_rules>
- Execute dependencies sequentially (1 → 2 → 3 → 4 → 5)
- Each dependency must complete or be explicitly skipped before proceeding
- Mode choice (automated/guided/check-only) persists across all dependencies unless changed
- Failed automated installs fall back to guided mode
- User can abort at any decision gate
- If TEST_MODE is set, append --test=$TEST_MODE to all system-check.sh invocations
</execution_rules>

## Platform Detection

**Step 1: Detect platform**

```bash
# Run system check script (append --test=$SCENARIO if in test mode)
bash .claude/skills/system-setup/assets/system-check.sh --detect-platform ${TEST_MODE:+--test=$TEST_MODE}
```

**Note:** If TEST_MODE is set, append `--test=$TEST_MODE` to ALL system-check.sh invocations throughout this skill.

The script returns JSON:
```json
{
  "platform": "darwin",
  "platform_version": "14.0",
  "arch": "arm64"
}
```

**Step 2: Confirm with user**

```
Detected platform: macOS 14.0 (arm64)

Is this correct?
1. Yes, continue
2. No, let me specify

Choose (1-2): _
```

---

## Dependency Validation Workflow

For each dependency (in order):

1. **Check if already installed and functional**
2. **If found:**
   - Display version and path
   - Validate it meets minimum requirements
   - Save to config
   - Continue to next dependency
3. **If not found:**
   - **Automated mode:** Offer to install automatically
   - **Guided mode:** Show manual installation instructions
   - **Check-only mode:** Report as missing, continue

<dependency name="python" order="1" required="true" depends_on="">
  <detection>
    <command>bash .claude/skills/system-setup/assets/system-check.sh --check-python</command>
    <version_check minimum="3.8">
      <parse_pattern>Python (\d+\.\d+\.\d+)</parse_pattern>
      <returns_json>
        {
          "found": true,
          "version": "3.11.5",
          "path": "/usr/local/bin/python3",
          "meets_minimum": true
        }
      </returns_json>
    </version_check>
  </detection>

  <validation>
    <if condition="found AND version >= 3.8">
      <action>
        Display: "✓ Python [version] found at [path]"
        Save to system-config.json: python_path, python_version
        Continue to dependency 2 (build-tools)
      </action>
    </if>

    <if condition="found AND version < 3.8">
      <decision_gate type="upgrade" wait_required="true">
        Display: "✗ Python [version] found, but version 3.8+ is required"

        Menu:
        1. Update via Homebrew (automated)
        2. Manual update instructions
        3. Continue anyway (build may fail)

        Choose (1-3): _

        <option id="1">
          <critical_sequence>
            <step>Check if Homebrew installed</step>
            <step>Run: brew upgrade python3</step>
            <step>Verify: python3 --version</step>
            <step>IF success: Save to config, continue</step>
            <step>ELSE: Fall back to guided mode</step>
          </critical_sequence>
        </option>

        <option id="2">
          Display manual instructions from references/platform-requirements.md
          Wait for user confirmation
          Re-run detection
        </option>

        <option id="3">
          Display warning: "Build may fail with Python < 3.8"
          Save current version to config with warning flag
          Continue to next dependency
        </option>
      </decision_gate>
    </if>

    <if condition="not_found">
      <decision_gate type="mode_based" wait_required="true">
        <automated_mode>
          Display: "✗ Python 3.8+ not found"

          Menu:
          1. Yes, install automatically
          2. No, show me manual instructions
          3. Skip Python (not recommended)

          Choose (1-3): _

          <option id="1">
            <critical_sequence enforcement="strict">
              <step id="check_homebrew">Check if Homebrew installed: which brew</step>
              <step id="install_homebrew" depends_on="check_homebrew">
                IF NOT installed: Run Homebrew installer with user confirmation
                ELSE: Continue
              </step>
              <step id="install_python" depends_on="install_homebrew">
                Run: brew install python3
              </step>
              <step id="verify" depends_on="install_python">
                Run: python3 --version
                IF success: Save to config, continue to dependency 2
                ELSE: Display error, fall back to option 2 (manual instructions)
              </step>
            </critical_sequence>
          </option>

          <option id="2">
            Display content from references/platform-requirements.md § Python
            WAIT for user to complete installation
            Prompt: "Press Enter when Python is installed..."
            Re-run detection and validation
          </option>

          <option id="3">
            Display warning: "Python required for build scripts"
            Mark as skipped in config
            Continue to dependency 2
          </option>
        </automated_mode>

        <guided_mode>
          Display content from references/platform-requirements.md § Python
          WAIT for user to complete installation
          Prompt: "Press Enter when Python is installed..."
          Re-run detection and validation
        </guided_mode>

        <check_only_mode>
          Report: "✗ Python 3.8+ not found"
          Continue to dependency 2
        </check_only_mode>
      </decision_gate>
    </if>
  </validation>
</dependency>

### 1. Python Validation

**Implementation note:** The above XML structure enforces the Python validation workflow. All conditional paths must be followed, and user decisions are required at decision gates.

<dependency name="build-tools" order="2" required="true" depends_on="1">
  <detection>
    <platform_specific>
      <macos>
        <command>bash .claude/skills/system-setup/assets/system-check.sh --check-xcode</command>
        <returns_json>
          {
            "found": true,
            "path": "/Library/Developer/CommandLineTools",
            "version": "15.0"
          }
        </returns_json>
      </macos>
      <linux>
        <command>bash .claude/skills/system-setup/assets/system-check.sh --check-build-tools</command>
      </linux>
      <windows>
        <command>bash .claude/skills/system-setup/assets/system-check.sh --check-msvc</command>
      </windows>
    </platform_specific>
  </detection>

  <validation>
    <if condition="found">
      <action>
        Display: "✓ Xcode Command Line Tools found (version [version])"
        Save to system-config.json: xcode_path, version
        Continue to dependency 3 (cmake)
      </action>
    </if>

    <if condition="not_found">
      <decision_gate type="mode_based" wait_required="true">
        <automated_mode>
          Display: "✗ Xcode Command Line Tools not found"
          Display: "These are required for compiling C++ code on macOS."

          Menu:
          1. Yes, install automatically
          2. No, show me manual instructions
          3. Skip (build will fail)

          Choose (1-3): _

          <option id="1">
            <critical_sequence enforcement="strict">
              <step id="launch_installer">
                Run: xcode-select --install
                NOTE: This launches a GUI installation dialog
              </step>
              <step id="wait_for_gui" depends_on="launch_installer">
                Display: "⏳ Please complete the Xcode Command Line Tools installation dialog."
                Display: "Press Enter when installation is complete..."
                WAIT for user input (read/pause)
                CRITICAL: User MUST complete GUI installation manually
              </step>
              <step id="verify" depends_on="wait_for_gui">
                Run: xcode-select -p
                IF success: Save to config, continue to dependency 3
                ELSE: Display error, offer option 2 (manual instructions)
              </step>
            </critical_sequence>
          </option>

          <option id="2">
            Display content from references/platform-requirements.md § Build Tools
            WAIT for user to complete installation
            Prompt: "Press Enter when build tools are installed..."
            Re-run detection and validation
          </option>

          <option id="3">
            Display warning: "C++ compilation will fail without build tools"
            Mark as skipped in config
            Continue to dependency 3
          </option>
        </automated_mode>

        <guided_mode>
          Display content from references/platform-requirements.md § Build Tools
          WAIT for user to complete installation
          Prompt: "Press Enter when build tools are installed..."
          Re-run detection and validation
        </guided_mode>

        <check_only_mode>
          Report: "✗ Xcode Command Line Tools not found"
          Continue to dependency 3
        </check_only_mode>
      </decision_gate>
    </if>
  </validation>
</dependency>

### 2. Build Tools Validation

**Implementation note:** The above XML structure enforces the build tools validation workflow. Note that macOS requires GUI interaction for Xcode CLI Tools installation.

<dependency name="cmake" order="3" required="true" depends_on="1,2">
  <detection>
    <command>bash .claude/skills/system-setup/assets/system-check.sh --check-cmake</command>
    <version_check minimum="3.15">
      <parse_pattern>cmake version (\d+\.\d+\.\d+)</parse_pattern>
      <returns_json>
        {
          "found": true,
          "version": "3.27.4",
          "path": "/usr/local/bin/cmake",
          "meets_minimum": true
        }
      </returns_json>
    </version_check>
  </detection>

  <validation>
    <if condition="found AND version >= 3.15">
      <action>
        Display: "✓ CMake [version] found at [path]"
        Save to system-config.json: cmake_path, cmake_version
        Continue to dependency 4 (juce)
      </action>
    </if>

    <if condition="found AND version < 3.15">
      <decision_gate type="upgrade" wait_required="true">
        Display: "✗ CMake [version] found, but version 3.15+ is required"

        Menu:
        1. Update via Homebrew (automated)
        2. Manual update instructions
        3. Continue anyway (build may fail)

        Choose (1-3): _

        <option id="1">
          <critical_sequence>
            <step>Run: brew upgrade cmake</step>
            <step>Verify: cmake --version</step>
            <step>IF success: Save to config, continue</step>
            <step>ELSE: Fall back to guided mode</step>
          </critical_sequence>
        </option>

        <option id="2">
          Display manual instructions from references/platform-requirements.md
          Wait for user confirmation
          Re-run detection
        </option>

        <option id="3">
          Display warning: "Build may fail with CMake < 3.15"
          Save current version to config with warning flag
          Continue to dependency 4
        </option>
      </decision_gate>
    </if>

    <if condition="not_found">
      <decision_gate type="mode_based" wait_required="true">
        <automated_mode>
          Display: "✗ CMake 3.15+ not found"
          Display: "CMake is required for building JUCE projects."

          Menu:
          1. Yes, install automatically
          2. No, download manually from cmake.org
          3. Skip (build will fail)

          Choose (1-3): _

          <option id="1">
            <critical_sequence enforcement="strict">
              <step id="check_homebrew">Check if Homebrew installed: which brew</step>
              <step id="install_homebrew" depends_on="check_homebrew">
                IF NOT installed: Run Homebrew installer with user confirmation
                ELSE: Continue
              </step>
              <step id="install_cmake" depends_on="install_homebrew">
                Run: brew install cmake
              </step>
              <step id="verify" depends_on="install_cmake">
                Run: cmake --version
                IF success: Save to config, continue to dependency 4
                ELSE: Display error, fall back to option 2 (manual instructions)
              </step>
            </critical_sequence>
          </option>

          <option id="2">
            Display content from references/platform-requirements.md § CMake
            WAIT for user to complete installation
            Prompt: "Press Enter when CMake is installed..."
            Re-run detection and validation
          </option>

          <option id="3">
            Display warning: "CMake required for building JUCE projects"
            Mark as skipped in config
            Continue to dependency 4
          </option>
        </automated_mode>

        <guided_mode>
          Display content from references/platform-requirements.md § CMake
          WAIT for user to complete installation
          Prompt: "Press Enter when CMake is installed..."
          Re-run detection and validation
        </guided_mode>

        <check_only_mode>
          Report: "✗ CMake 3.15+ not found"
          Continue to dependency 4
        </check_only_mode>
      </decision_gate>
    </if>
  </validation>
</dependency>

### 3. CMake Validation

**Implementation note:** The above XML structure enforces the CMake validation workflow with version checking.

<dependency name="juce" order="4" required="true" depends_on="2,3">
  <detection>
    <command>bash .claude/skills/system-setup/assets/system-check.sh --check-juce</command>
    <version_check minimum="8.0.0">
      <parse_pattern>JUCE v(\d+\.\d+\.\d+)</parse_pattern>
      <validation_file>modules/juce_core/juce_core.h</validation_file>
    </version_check>
    <standard_locations>
      <path>~/JUCE</path>
      <path>/Applications/JUCE</path>
      <path>/usr/local/JUCE</path>
    </standard_locations>
    <returns_json>
      {
        "found": true,
        "path": "/Users/lex/JUCE",
        "version": "8.0.3",
        "meets_minimum": true
      }
    </returns_json>
    <returns_json_if_not_found>
      {
        "found": false,
        "checked_paths": [
          "/Users/lex/JUCE",
          "/Applications/JUCE",
          "/usr/local/JUCE"
        ]
      }
    </returns_json_if_not_found>
  </detection>

  <validation>
    <if condition="found_in_standard_location AND version >= 8.0.0">
      <action>
        Display: "✓ JUCE [version] found at [path]"
        Save to system-config.json: juce_path, juce_version
        Continue to dependency 5 (pluginval)
      </action>
    </if>

    <if condition="not_found_in_standard_locations">
      <decision_gate type="custom_path" wait_required="true">
        Display: "✗ JUCE not found in standard locations"
        Display: "JUCE is required for plugin development."

        Menu:
        1. Yes, let me provide the path
        2. No, install it for me (automated mode only)
        3. No, show me how to install it manually

        Choose (1-3): _

        <option id="1">
          <validation_loop max_attempts="3">
            <step id="prompt_path">
              Prompt: "Enter the full path to your JUCE installation:"
              Prompt: "(e.g., /Users/lex/Development/JUCE)"
              Prompt: ""
              Prompt: "Path: _"
              READ user input into $CUSTOM_PATH
            </step>

            <step id="validate_path" depends_on="prompt_path">
              Run: bash .claude/skills/system-setup/assets/system-check.sh --validate-juce-path "$CUSTOM_PATH"

              <if condition="valid">
                Display: "✓ JUCE [version] found at $CUSTOM_PATH"
                Save to system-config.json: juce_path=$CUSTOM_PATH, juce_version
                Continue to dependency 5
                EXIT loop
              </if>

              <if condition="invalid">
                Display: "✗ Invalid JUCE installation at provided path"
                Display: "The path must contain JUCE modules (modules/juce_core/juce_core.h)."
                INCREMENT attempt counter

                <if condition="attempts < max_attempts">
                  <decision_gate wait_required="true">
                    Menu:
                    1. Try a different path
                    2. Install JUCE to default location (~/JUCE)
                    3. Show manual installation instructions

                    Choose (1-3): _

                    <sub_option id="1">RETRY from step "prompt_path"</sub_option>
                    <sub_option id="2">GOTO option id="2" (automated install)</sub_option>
                    <sub_option id="3">GOTO option id="3" (manual instructions)</sub_option>
                  </decision_gate>
                </if>

                <if condition="attempts >= max_attempts">
                  Display: "Maximum attempts reached."
                  Fall back to option 3 (manual instructions)
                </if>
              </if>
            </step>
          </validation_loop>
        </option>

        <option id="2">
          <decision_gate type="mode_check">
            <automated_mode>
              <critical_sequence enforcement="strict">
                <step id="download">
                  Display: "Installing JUCE 8.0.3 to ~/JUCE..."
                  Display: "⏳ Downloading JUCE from juce.com..."
                  Run: cd ~
                  Run: curl -L -o JUCE.zip "https://github.com/juce-framework/JUCE/releases/download/8.0.3/juce-8.0.3-osx.zip"
                </step>

                <step id="extract" depends_on="download">
                  Run: unzip -q JUCE.zip
                  Run: mv juce-8.0.3-osx JUCE
                </step>

                <step id="cleanup" depends_on="extract">
                  Run: rm JUCE.zip
                </step>

                <step id="verify" depends_on="cleanup">
                  Run: bash .claude/skills/system-setup/assets/system-check.sh --validate-juce-path ~/JUCE

                  <if condition="success">
                    Display: "✓ JUCE installed successfully"
                    Save to system-config.json: juce_path=~/JUCE, juce_version
                    Continue to dependency 5
                  </if>

                  <if condition="failure">
                    Display: "✗ Automated installation failed"
                    Fall back to option 3 (manual instructions from references/juce-setup-guide.md)
                  </if>
                </step>
              </critical_sequence>
            </automated_mode>

            <guided_mode>
              Display: "Automated installation not available in guided mode"
              Fall back to option 3 (manual instructions)
            </guided_mode>

            <check_only_mode>
              Display: "Installation not available in check-only mode"
              Report: "✗ JUCE 8.0+ not found"
              Continue to dependency 5
            </check_only_mode>
          </decision_gate>
        </option>

        <option id="3">
          Display content from references/juce-setup-guide.md
          WAIT for user to complete installation
          Prompt: "Press Enter when JUCE is installed..."
          Re-run detection and validation from beginning
        </option>
      </decision_gate>
    </if>
  </validation>
</dependency>

### 4. JUCE Validation

**Implementation note:** The above XML structure enforces the JUCE validation workflow with multiple search locations, custom path retry logic (max 3 attempts), and fallback to manual instructions. This is the most complex dependency check.

<dependency name="pluginval" order="5" required="false" depends_on="4">
  <detection>
    <command>bash .claude/skills/system-setup/assets/system-check.sh --check-pluginval</command>
    <standard_locations>
      <path>/usr/local/bin/pluginval</path>
      <path>~/bin/pluginval</path>
    </standard_locations>
    <returns_json>
      {
        "found": true,
        "version": "1.0.3",
        "path": "/usr/local/bin/pluginval"
      }
    </returns_json>
  </detection>

  <validation>
    <if condition="found">
      <action>
        Display: "✓ pluginval [version] found at [path]"
        Save to system-config.json: pluginval_path, pluginval_version
        Continue to configuration persistence
      </action>
    </if>

    <if condition="not_found">
      <decision_gate type="mode_based" wait_required="true">
        <automated_mode>
          Display: "✗ pluginval not found"
          Display: "pluginval is used to validate plugin compliance."

          Menu:
          1. Yes, install automatically
          2. No, show manual instructions
          3. Skip (validation will be limited)

          Choose (1-3): _

          <option id="1">
            <critical_sequence enforcement="strict">
              <step id="download">
                Display: "Downloading pluginval..."
                Run: cd /tmp
                Run: curl -L -o pluginval.zip "https://github.com/Tracktion/pluginval/releases/latest/download/pluginval_macOS.zip"
              </step>

              <step id="extract" depends_on="download">
                Run: unzip -q pluginval.zip
              </step>

              <step id="install" depends_on="extract">
                TRY: sudo mv pluginval.app/Contents/MacOS/pluginval /usr/local/bin/
                TRY: sudo chmod +x /usr/local/bin/pluginval

                <error_recovery>
                  <if condition="permission_denied">
                    <decision_gate wait_required="true">
                      Display: "✗ Failed to install to /usr/local/bin (permission denied)"

                      Menu:
                      1. Retry with sudo (will prompt for password)
                      2. Install to ~/bin instead (no sudo required)
                      3. Show manual instructions

                      Choose (1-3): _

                      <recovery_option id="1">RETRY install step with sudo confirmation</recovery_option>
                      <recovery_option id="2">
                        Run: mkdir -p ~/bin
                        Run: mv pluginval.app/Contents/MacOS/pluginval ~/bin/
                        Run: chmod +x ~/bin/pluginval
                        Save to config: pluginval_path=~/bin/pluginval
                      </recovery_option>
                      <recovery_option id="3">Fall back to option 2 (manual instructions)</recovery_option>
                    </decision_gate>
                  </if>
                </error_recovery>
              </step>

              <step id="cleanup" depends_on="install">
                Run: rm -rf /tmp/pluginval.zip /tmp/pluginval.app
              </step>

              <step id="verify" depends_on="cleanup">
                Run: pluginval --version OR ~/bin/pluginval --version

                <if condition="success">
                  Display: "✓ pluginval installed successfully"
                  Save to system-config.json: pluginval_path, pluginval_version
                  Continue to configuration persistence
                </if>

                <if condition="failure">
                  Display: "✗ Automated installation failed"
                  Fall back to option 2 (manual instructions)
                </if>
              </step>
            </critical_sequence>
          </option>

          <option id="2">
            Display content from references/platform-requirements.md § pluginval
            WAIT for user to complete installation
            Prompt: "Press Enter when pluginval is installed..."
            Re-run detection and validation
          </option>

          <option id="3">
            Display warning: "Plugin validation will be limited without pluginval"
            Mark as skipped in config
            Continue to configuration persistence
          </option>
        </automated_mode>

        <guided_mode>
          Display content from references/platform-requirements.md § pluginval
          WAIT for user to complete installation
          Prompt: "Press Enter when pluginval is installed..."
          Re-run detection and validation
        </guided_mode>

        <check_only_mode>
          Report: "✗ pluginval not found (optional)"
          Continue to configuration persistence
        </check_only_mode>
      </decision_gate>
    </if>
  </validation>
</dependency>

### 5. pluginval Validation

**Implementation note:** The above XML structure enforces the pluginval validation workflow with error recovery for permission issues. Note that pluginval is optional (required="false").

</critical_sequence>

---

## Configuration Persistence

After all dependencies are validated, create `.claude/system-config.json`:

```bash
# Generate config file
cat > .claude/system-config.json <<EOF
{
  "platform": "darwin",
  "platform_version": "14.0",
  "arch": "arm64",
  "python_path": "/usr/local/bin/python3",
  "python_version": "3.11.5",
  "xcode_path": "/Library/Developer/CommandLineTools",
  "cmake_path": "/usr/local/bin/cmake",
  "cmake_version": "3.27.4",
  "juce_path": "/Users/lex/JUCE",
  "juce_version": "8.0.3",
  "pluginval_path": "/usr/local/bin/pluginval",
  "pluginval_version": "1.0.3",
  "validated_at": "$(date -u +%Y-%m-%dT%H:%M:%SZ)"
}
EOF
```

**Add to .gitignore if not already present:**
```bash
grep -q "system-config.json" .gitignore || echo ".claude/system-config.json" >> .gitignore
```

---

## System Report

After configuration is saved, display comprehensive summary:

```
✓ System Setup Complete

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Platform: macOS 14.0 (arm64)

Dependencies validated:
✓ Python 3.11.5 (/usr/local/bin/python3)
✓ Xcode Command Line Tools 15.0
✓ CMake 3.27.4 (/usr/local/bin/cmake)
✓ JUCE 8.0.3 (/Users/lex/JUCE)
✓ pluginval 1.0.3 (/usr/local/bin/pluginval)

Configuration saved to:
.claude/system-config.json

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

What's next?
1. Create your first plugin (/dream)
2. View available commands (type /? or press Tab)
3. Read the documentation (@README.md)
4. Run system check again (/setup)
5. Exit

Choose (1-5): _
```

**Handle user choice:**
- Choice 1: Invoke plugin-ideation skill (same as `/dream`)
- Choice 2: Show command list via `ls .claude/commands/`
- Choice 3: Display README.md
- Choice 4: Re-run system-setup skill
- Choice 5: Exit with message

<state_management>
This skill is STANDALONE and does NOT:
- Update PLUGINS.md
- Create .continue-here.md
- Invoke other workflow skills (plugin-workflow, plugin-planning, etc.)
- Create or modify plugin directories

This skill ONLY:
- Creates/updates .claude/system-config.json
- Validates system dependencies
- Adds system-config.json to .gitignore
- Exits when validation complete

After setup completes successfully:
1. Configuration stored in .claude/system-config.json only
2. NO state file updates needed
3. NO plugin workflow initiated
4. User chooses next action from final menu
</state_management>

---

## Error Handling

### Automated Installation Failures

If any automated installation fails:

1. **Capture error output**
2. **Display error message**
3. **Fall back to guided mode**

Example:
```
✗ Failed to install CMake via Homebrew

Error: Homebrew is not installed and automatic installation failed.

Falling back to manual instructions...

[Display manual CMake installation steps from references/platform-requirements.md]

Press Enter to continue with manual installation...
```

### Missing Critical Dependencies

If critical dependencies are missing and user skips them:

```
⚠ Warning: Critical dependencies are missing

The following required dependencies were not installed:
- CMake 3.15+ (required for building)
- JUCE 8.0+ (required for plugin development)

You will not be able to build plugins until these are installed.

Would you like to:
1. Go back and install missing dependencies
2. Save current configuration anyway (not recommended)
3. Exit without saving

Choose (1-3): _
```

### Version Too Old

If a dependency is found but version is too old:

```
✗ CMake 3.10.2 found, but version 3.15+ is required

Would you like to:
1. Update CMake via Homebrew (automated)
2. Show manual update instructions
3. Continue anyway (build may fail)

Choose (1-3): _
```

### Permission Errors

If installation fails due to permissions:

```
✗ Failed to install pluginval: Permission denied

This usually means you need sudo access.

Would you like to:
1. Retry with sudo (will prompt for password)
2. Install to ~/bin instead (no sudo required)
3. Show manual instructions

Choose (1-3): _
```

---

## Checkpoint Protocol Integration

This skill follows the checkpoint protocol:

**After each major validation step:**
1. Display what was found/installed
2. Present numbered decision menu
3. Wait for user response
4. Execute chosen action

**Major checkpoint moments:**
- After platform detection
- After each dependency validation
- After configuration is saved
- At final system report

**Always use inline numbered menus, never AskUserQuestion tool.**

---

## Integration Points

**Invoked by:**
- `/setup` command (primary entry point)
- New user onboarding
- When build scripts detect missing dependencies

**Reads:**
- `.claude/system-config.json` (if exists, to show current config)
- `references/platform-requirements.md` (platform-specific installation guides)
- `references/juce-setup-guide.md` (detailed JUCE installation)

**Creates:**
- `.claude/system-config.json` (validated dependency paths)

**Uses:**
- `assets/system-check.sh` (bash validation script)

**May invoke:**
- `plugin-ideation` skill (if user chooses to create plugin after setup)

---

## Success Criteria

Setup is successful when:

- All required dependencies are detected or installed
- All versions meet minimum requirements
- All tools are validated as functional (not just present)
- Configuration is saved to `.claude/system-config.json`
- User receives clear system report
- Decision menus presented at appropriate points
- Errors are handled gracefully with fallback options

---

## Notes for Claude

<critical_requirements>
  <requirement>Check before installing (NEVER install if exists)</requirement>
  <requirement>Validate versions meet minimums (don't assume)</requirement>
  <requirement>Test functionality (not just presence)</requirement>
  <requirement>Get confirmation before automated installation</requirement>
  <requirement>Provide fallback if automation fails</requirement>
  <requirement>Use absolute paths (expand ~, validate existence)</requirement>
  <requirement>Mode-specific behavior (respect user's choice)</requirement>
  <requirement>Append test mode to all system-check.sh calls if TEST_MODE set</requirement>
</critical_requirements>

<anti_patterns>
  <anti_pattern>Installing without checking first</anti_pattern>
  <anti_pattern>Not validating version requirements</anti_pattern>
  <anti_pattern>Proceeding when critical dependencies missing</anti_pattern>
  <anti_pattern>Using relative paths in config</anti_pattern>
  <anti_pattern>Not testing executables actually run</anti_pattern>
  <anti_pattern>Auto-proceeding without user confirmation</anti_pattern>
  <anti_pattern>Using AskUserQuestion tool instead of inline menus</anti_pattern>
  <anti_pattern>Forgetting to append test mode to system-check.sh calls</anti_pattern>
  <anti_pattern>Not respecting MODE throughout execution</anti_pattern>
</anti_patterns>

**Automated vs Guided Mode:**

- **Automated mode:** Attempt installation with confirmation, fall back to guided if fails
- **Guided mode:** Show manual instructions immediately, no automation
- **Check-only mode:** Report what's found, make no changes

**Path handling:**

- Convert all paths to absolute paths
- Validate paths exist before saving to config
- Expand `~` to actual home directory path
- Check that JUCE path contains expected files (modules/juce_core/juce_core.h)

**macOS-specific notes:**

- Xcode CLI Tools installation requires GUI interaction
- Homebrew installation may require user interaction
- Some installations may need sudo (get permission first)
- `/usr/local/bin` may need sudo for writing

**Common pitfalls to AVOID:**

- Installing without checking if already present
- Not validating versions meet minimums
- Proceeding when critical dependencies missing
- Using relative paths in configuration
- Not testing if executables actually run
- Auto-proceeding without user confirmation
- Using AskUserQuestion instead of inline menus

<completion_criteria>
This skill is complete when:

1. ✓ Platform detected and confirmed by user
2. ✓ All 5 dependencies validated (or user explicitly skipped)
3. ✓ Configuration saved to .claude/system-config.json
4. ✓ Configuration file added to .gitignore
5. ✓ Final system report displayed
6. ✓ User selected action from final menu (exit, start new plugin, resume existing)

DO NOT auto-proceed to next action after final menu - user decides what's next.
</completion_criteria>
