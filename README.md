# Autumn Aftertouch Ensemble

**Today’s Instrument: Autumn Aftertouch Ensemble**

A polyphonic string-like synthesizer that responds to aftertouch for expressive vibrato and filter opening, and to the sustain pedal for lingering resonant tails, producing a live, cinematic ensemble texture.

## Features
- 8-voice polyphony
- Dual detuned square oscillators with simple low-pass filtering
- ADSR envelope
- Channel aftertouch modulates vibrato depth and filter cutoff
- Sustain pedal (CC 64) holds notes
- Parameter smoothing
- Safety limiter
- Full state save/load via APVTS
- Dark cinematic UI (black background, amber accents)

## Build Notes
Requires CMake 3.22+ and a C++20 compiler.

JUCE 8 is fetched automatically via FetchContent (tag 8.0.10).

### Local build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Outputs appear in `build/AutumnAftertouchEnsemble_artefacts/Release/`.

- VST3: `.../VST3/AutumnAftertouchEnsemble.vst3`
- Standalone: `.../Standalone/AutumnAftertouchEnsemble.app` (macOS) or `.exe` (Windows)

### GitHub Actions
The included workflow builds Release VST3 + Standalone on macOS and Windows, packages them as zips, and attaches them to a GitHub Release.

Company: MPJRecords  
Manufacturer code: MPJR  
Plugin code: A23s
