# Parameter Specification (Draft)

**Status:** Draft - Awaiting UI mockup for full specification
**Created:** 2025-11-17
**Source:** Quick capture during ideation

This is a lightweight specification to enable parallel DSP research.
Full specification will be generated from finalized UI mockup.

## Parameters

### gain
- **Type:** Float
- **Range:** 0 to 10 (maps to 3dB to 40dB internally)
- **Default:** 5.0
- **DSP Purpose:** Drive amount controlling pre-clipping filter emphasis and nonlinear section intensity. Low settings (1-4) = boost mode for pushing already dirty amps. High settings (6-10) = distortion mode for clean amps.

### tone
- **Type:** Float
- **Range:** 0 to 10 (maps to -12dB to +12dB tilt filter @ 680Hz)
- **Default:** 5.0
- **DSP Purpose:** Tilt filter controlling frequency balance. Low values darken tone (cut highs/boost lows), high values brighten tone (boost highs/cut lows). Center frequency at 680Hz.

### level
- **Type:** Float
- **Range:** 0 to 10 (maps to -∞ to 0dB, mute to unity)
- **Default:** 8.0
- **DSP Purpose:** Output level control providing makeup gain after clipping stage. Assumes nonlinear section clips at ±1. Full range from mute to unity gain.

## Next Steps

- [ ] Complete UI mockup workflow (/dream → option 3)
- [ ] Finalize design and generate full parameter-spec.md
- [ ] Validate consistency between draft and final spec
