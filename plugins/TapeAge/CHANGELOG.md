# Changelog

All notable changes to TapeAge will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.1] - 2025-11-11

### Fixed

- Parameter state now persists correctly when closing and reopening Ableton Live
- Changed `getNumPrograms()` to return 0 to prevent Ableton from interfering with state restoration
- Fixes issue where parameters would reset to defaults on project reload

## [1.0.0] - 2025-11-11

### Added

- Initial release
- Vintage tape saturator with warm harmonic saturation
- Tape degradation effects: wow, flutter, dropout, and noise
- Three parameters: Drive (saturation), Age (degradation), Mix (dry/wet)
- Drive: 0-100%, default 50% - Controls tape saturation intensity
- Age: 0-100%, default 25% - Controls tape degradation artifacts (wow, flutter, dropout, noise)
- Mix: 0-100%, default 100% - Dry/wet blend control
- Factory presets: Unity (bypass), Subtle Warmth, Classic Tape, Worn Out, Destroyed

### Audio Processing

- Warm harmonic saturation with 2x oversampling for aliasing reduction
- Custom tanh transfer function for vintage tape character
- Wow and flutter modulation (±10 cents) using delay line with Lagrange interpolation
- Subtle tape noise generation (musical even at maximum settings)
- Random dropout simulation for authentic tape artifacts
- Equal-power dry/wet crossfade with latency compensation
- Real-time safe processing (no allocations in audio callback)

### User Interface

- WebView-based vintage hardware aesthetic
- Vintage VU meter with peak metering and ballistic animation (30 FPS C++ + 60 FPS JS)
- Three brass rotary knobs with gold indicators (horizontal layout)
- Creamy beige textured background with burnt orange/brown accents
- Clean sans-serif all-caps typography
- 500x450px window size

### Validation

- Passed pluginval strict validation (strictness level 10)
- VST3: All tests passed
- AU: All tests passed + auval validation
- Tested in Ableton Live and Standalone mode
- Thread safety verified
- State save/restore verified
- Parameter automation verified

### Formats

- VST3
- AU (Audio Unit)
- Standalone application
