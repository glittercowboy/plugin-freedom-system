---
plugin: JUCE
date: 2025-11-08
problem_type: api_misuse
component: webview
symptoms:
  - HTML slider moves but audio parameter does not change
  - Parameter visible in DAW automation but not controlled by WebView
  - JavaScript events sent to JUCE backend but no effect on audio
  - WebSliderRelay created but parameter stays at initial value
root_cause: wrong_api
juce_version: 8.0.9
resolution_type: code_fix
severity: critical
tags: [webview, webbrowser, parameter, javascript, websliderrelay, event-format, juce8]
---

# Troubleshooting: WebView HTML Slider Not Updating JUCE Parameter

## Problem
HTML slider in WebBrowserComponent moves and sends events to JUCE backend, but the actual audio parameter does not change. The parameter is accessible via DAW automation controls and works when changed there, but JavaScript slider changes have no effect.

## Environment
- JUCE Version: 8.0.9
- Affected: WebBrowserComponent with WebSliderRelay, PluginEditor
- Platform: macOS (WKWebView backend)
- Date: 2025-11-08

## Symptoms
- HTML slider moves smoothly in the WebView UI
- JavaScript console shows events being sent to backend
- Audio parameter visible in DAW automation and works when changed manually
- Moving HTML slider produces no audible change (parameter value not updating)
- WebSliderRelay and WebSliderParameterAttachment created correctly in C++
- No console errors or warnings

## What Didn't Work

**Attempted Solution 1:** Used incorrect custom event format
```javascript
// ❌ Wrong approach
window.__JUCE__.backend.emitEvent('__juce__sliderValueChanged', {
    sliderName: 'gain',
    value: value
});
```
- **Why it failed:** This is not the event format JUCE's WebSliderRelay expects. The event ID and payload structure must match JUCE's internal protocol.

**Attempted Solution 2:** Tried using `new __JUCE__.WebSliderRelay()` constructor
```javascript
// ❌ Wrong approach - this API doesn't exist
const gainSlider = new __JUCE__.WebSliderRelay('gain');
gainSlider.valueChanged(value);
```
- **Why it failed:** There is no `WebSliderRelay` constructor exposed to JavaScript. The JUCE 8 frontend API uses `getSliderState()` from the ES6 module, which internally manages the correct event protocol.

## Solution

The JavaScript must send events using the exact format that `WebSliderRelay` expects:

**Event ID format:** `"__juce__slider" + sliderName`
**Payload format:** `{ eventType: "valueChanged", value: scaledValue }`

### Working Code

**JavaScript (correct format):**
```javascript
// Send parameter change to JUCE
const sliderName = 'gain';
const eventId = '__juce__slider' + sliderName;  // "__juce__slidergain"

window.__JUCE__.backend.emitEvent(eventId, {
    eventType: 'valueChanged',
    value: 0.75  // Normalized value 0-1
});
```

**Or use the official JUCE ES6 module API:**
```javascript
// Better approach - use JUCE's provided API
import { getSliderState } from './js/juce/index.js';

const gainState = getSliderState('gain');

// Update parameter
slider.addEventListener('input', (e) => {
    const value = parseFloat(e.target.value);
    gainState.setNormalisedValue(value);  // Handles event format internally
});

// Listen for C++ changes (automation)
gainState.valueChangedEvent.addListener((newValue) => {
    slider.value = newValue;
});
```

**C++ setup (required):**
```cpp
// PluginEditor.h
class MyPluginEditor : public juce::AudioProcessorEditor
{
private:
    juce::WebSliderRelay gainRelay { "gain" };  // Name must match JavaScript

    juce::WebBrowserComponent browser {
        juce::WebBrowserComponent::Options{}
            .withNativeIntegrationEnabled()
            .withOptionsFrom (gainRelay)
            .withResourceProvider ([this](const juce::String& path) {
                return handleResourceRequest (path);
            })
    };

    juce::WebSliderParameterAttachment gainAttachment;
};

// PluginEditor.cpp constructor
MyPluginEditor::MyPluginEditor (MyProcessor& p)
    : AudioProcessorEditor (&p),
      audioProcessor (p),
      gainAttachment (*audioProcessor.parameters.getParameter ("gain"),
                      gainRelay,
                      audioProcessor.parameters.undoManager)  // Can be nullptr
{
    addAndMakeVisible (browser);
    browser.goToURL (juce::WebBrowserComponent::getResourceProviderRoot());
}
```

## Why This Works

### Event Protocol

JUCE's `WebSliderRelay` listens for events with a specific format:

1. **Event ID:** Constructed as `"__juce__slider" + sliderName`
   - For a relay named `"gain"`, the event ID is `"__juce__slidergain"`
   - This identifier must match exactly

2. **Payload structure:**
   ```javascript
   {
       eventType: "valueChanged",  // Constant string
       value: <float>              // Normalized 0-1 value
   }
   ```

3. **Event constants:** Defined in `/Applications/JUCE/modules/juce_gui_extra/native/javascript/index.js`:
   ```javascript
   const BasicControl_valueChangedEventId = "valueChanged";
   ```

### Why ES6 Module Is Preferred

The `getSliderState()` API from JUCE's JavaScript module:
- Handles event ID construction automatically
- Manages payload formatting
- Provides bidirectional sync (C++ → JS via listeners)
- Handles edge cases (clamping, snapping to steps)

**Manual event sending works** but requires you to replicate all this logic correctly.

### C++ Side Requirements

1. **WebSliderRelay** must be constructed with the same name used in JavaScript
2. **`.withNativeIntegrationEnabled()`** injects the `window.__JUCE__` object
3. **`.withOptionsFrom(relay)`** registers the relay with the WebView
4. **WebSliderParameterAttachment** connects relay to actual audio parameter
5. **Order matters:** Relay must be initialized before browser in member initializer list

## Prevention

### When Using WebBrowserComponent with Parameters:

1. **Always use JUCE's JavaScript module** (`/js/juce/index.js`):
   ```javascript
   import { getSliderState } from './js/juce/index.js';
   const gainState = getSliderState('gain');
   gainState.setNormalisedValue(value);
   ```

2. **If manually sending events**, match the exact format:
   - Event ID: `"__juce__slider" + name`
   - Payload: `{ eventType: "valueChanged", value: <float> }`

3. **Verify event names match**:
   ```cpp
   // C++
   WebSliderRelay gainRelay { "gain" };
   ```
   ```javascript
   // JavaScript - must use same name
   const gainState = getSliderState('gain');
   ```

4. **Debug with console logging**:
   ```javascript
   console.log('window.__JUCE__:', window.__JUCE__);
   console.log('Sliders registered:', window.__JUCE__.initialisationData.__juce__sliders);
   console.log('Sending event:', eventId, payload);
   ```

5. **Test parameter changes in standalone app** before testing in DAW:
   ```bash
   cmake --build build --config Release --target MyPlugin_Standalone
   open "build/plugins/MyPlugin/MyPlugin_artefacts/Release/Standalone/My Plugin.app"
   ```
   Check Console.app for DBG() output to verify events received.

6. **Check WebSliderParameterAttachment** is created correctly:
   ```cpp
   // Must pass actual parameter pointer, not null
   auto* param = audioProcessor.parameters.getParameter("gain");
   if (param == nullptr) {
       DBG("ERROR: Parameter 'gain' not found!");
   }

   gainAttachment = std::make_unique<WebSliderParameterAttachment>(
       *param, gainRelay, audioProcessor.parameters.undoManager);
   ```

7. **Use resource provider, not file:// URLs**:
   - See webbrowser-local-html-url-cant-be-shown.md for details
   - File URLs don't work with WKWebView security model in JUCE 8

## Related Issues

- See also: [webview-knobs-frozen-multiple-causes-TapeAge-20251111.md](../gui-issues/webview-knobs-frozen-multiple-causes-TapeAge-20251111.md) - Different WebView parameter freeze (root cause: missing nullptr in WebSliderParameterAttachment + missing check_native_interop.js + duplicate PLUGIN_CODE)
- See also: webbrowser-local-html-url-cant-be-shown.md - Resource provider pattern required for HTML loading
- JUCE 8 WebView blog: https://juce.com/blog/juce-8-feature-overview-webview-uis/
- Official example: `/Applications/JUCE/examples/Plugins/WebViewPluginDemo.h`
