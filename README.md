# FaoxiAmp

A guitar amp simulation plugin (VST3 / AU / Standalone) inspired by Fender clean amps, written in C++ with JUCE.

> Work in progress — V1 targeted for the end of October 2026.

## Signal chain (V1)

```
mono in → gain → tanh waveshaper → 3-band EQ → cabinet IR → volume → stereo out
```

## Parameters

| Parameter | Range | Role |
|---|---|---|
| Gain | 0 to 10 | preamp drive, from clean to crunch |
| Bass / Mid / Treble | 0 to 10 (5 = flat) | tone shaping |
| Volume | −48 to +12 dB | output level |
| Cab | on / off | cabinet simulation (built-in impulse response) |

Gain and tone controls use amp-style 0–10 knobs, like a real amp front panel: in V1 they are mapped to dB internally, and in V2 they will drive a modelled tone stack. All parameters are smoothed to avoid clicks.

## V1 specification

- Plugin formats: VST3, AU, Standalone (tested in Logic Pro)
- Mono input, stereo output (the processed signal is duplicated, so both sides stay in phase)
- Real-time safe audio thread: no memory allocation in `processBlock()`
- Every DSP block is prototyped in Python first, then implemented in C++
- Built-in cabinet IR under a license that allows redistribution

**Out of scope for V1** (planned for V2): oversampling (anti-aliasing), modelled Fender tone stack, state-variable filters, user-loadable IRs.

## Building

Requirements: CMake 3.25+, a C++23 compiler (Xcode on macOS). JUCE is downloaded automatically on first configure.

```bash
cmake --preset default
cmake --build --preset default
```

With tests:

```bash
cmake --preset default-with-tests
cmake --build --preset default-with-tests
ctest --preset default-with-tests
```

On macOS, the AU and VST3 are copied to `~/Library/Audio/Plug-Ins` after each build.

## Project structure

```
FaoxiAmp/
├── CMakeLists.txt          # plugin target, formats, JUCE download
├── CMakePresets.json       # build configurations
├── cmake/                  # CPM package manager, compiler warnings
├── faoxiamp_plugin/        # plugin core, as a JUCE module
│   ├── include/FaoxiAmp/   # headers
│   └── source/             # implementation
└── test/                   # GoogleTest unit tests
```
