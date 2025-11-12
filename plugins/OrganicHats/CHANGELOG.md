# Changelog

All notable changes to OrganicHats will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.0.0] - 2025-11-12

### Added
- Initial release of OrganicHats hi-hat synthesizer
- Warm, organic noise-based hi-hat synthesis
- Separate closed and open hi-hat sounds (C1 and D1 MIDI triggers)
- 6 parameters: Tone, Decay, Noise Color (closed) + Tone, Release, Noise Color (open)
- Instant choke behavior (closed chokes open)
- Fixed resonance peaks at 7kHz, 10kHz, 13kHz for organic body
- Velocity sensitivity (affects volume and tone brightness)
- WebView UI with studio hardware aesthetic (600×590px)
- Power LED indicator with pulse animation
- Dual-panel layout (closed left, open right)
- VST3, AU, and Standalone formats
