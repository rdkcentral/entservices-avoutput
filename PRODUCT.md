# AVOutput Plugin - Product Overview

## Product Description

AVOutput is a Thunder (WPEFramework) plugin for managing video output and picture-quality controls on RDK devices.
It exposes a JSON-RPC interface for applications and operators to query output state, apply picture settings, and receive runtime video-format notifications.

The implementation is device-profile aware and build-profile aware:
- Runtime profile gating in plugin entry logic limits unsupported profiles.
- Build-time selection routes to `AVOutputTV` or `AVOutputSTB`, with the TV path providing the full feature set.

## Key Features

### 1. PQ Parameter Controls
- Backlight, brightness, contrast, sharpness, saturation, and hue controls
- Backlight dimming mode and auto backlight mode
- Get/set/reset operations plus capabilities queries for each control (`get*Caps`, `get*CapsV2`)

### 2. Picture Mode Preset Control
- Get/set/reset the active picture mode (`getPictureMode`, `setPictureMode`, `resetPictureMode`)
- Discover supported picture modes and their capabilities (`getSupportedPictureModes`, `getPictureModeCaps`, `getPictureModeCapsV2`)

### 3. Per Picture Mode Customisation of PQ Features
- All PQ feature values (backlight, brightness, contrast, etc.) are stored and applied per picture mode, enabling independent tuning for each mode (e.g. Standard, Movie, Game)
- Backed by `PicMode.` key prefix in persistent storage via `tr181api`

### 4. Per Video Format Customisation of PQ Features
- PQ settings are tracked per video format (SDR, HDR10, HLG, Dolby Vision), allowing format-aware picture tuning
- Backed by `Format.` key prefix in persistent storage

### 5. Per Video Source Customisation of PQ Features
- PQ settings are tracked per video source (HDMI1, HDMI2, composite, etc.), enabling source-specific picture profiles
- Backed by `Source.` key prefix in persistent storage

### 6. Enhancement Controls
- SDR gamma, precision detail, local contrast enhancement
- MPEG and digital noise reduction, MEMC, AI super resolution
- Zoom mode and low latency (game mode) management

### 7. HDR and Dolby Vision
- HDR mode get/set/reset and capability queries
- Dolby Vision mode management and supported-mode listing
- Dolby Vision calibration capability exposure

### 8. White Balance and Color Tuning
- CMS (color management system) controls
- 2-point white balance controls and WB mode management

### 9. Runtime Video State
- Query current runtime state: video format, source, frame rate, resolution, and content type

### 10. Event Callbacks
- `onVideoFormatChanged` — fired via `NotifyVideoFormatChange()`
- `onVideoContentChanged` — fired via `NotifyVideoContentChange()`
- `onVideoResolutionChanged` — fired via `NotifyVideoResolutionChange()`
- `onVideoFrameRateChanged` — fired via `NotifyVideoFrameRateChange()`
- `gameModeEvent` — fired via `broadcastLowLatencyModeChangeEvent()`

## API Surface

The TV implementation registers an extensive JSON-RPC API under the AVOutput plugin callsign.
Representative method families include:
- `get*`, `set*`, `reset*` triplets for controllable settings
- `get*Caps` — flat linear capability response: returns `rangeInfo`, `pictureModeInfo`, `videoSourceInfo`, and `videoFormatInfo` as independent flat arrays listing what is supported
- `get*CapsV2` — nested context aware capability response: returns a `context` object structured as `pictureMode → videoFormat → videoSource`, showing which source/format combinations are valid within each picture mode
- Runtime signal/query methods for current output context

## Architecture Summary

- `AVOutput` is the Thunder entry class (`PluginHost::IPlugin` + JSON-RPC dispatcher behavior).
- `AVOutputBase` provides shared JSON-RPC base behavior and lifecycle hook structure.
- `AVOutputTV` contains the primary feature implementation and notification publishing.
- `AVOutputSTB` is a reduced STB-side implementation path.

For deeper component and lifecycle details, see `ARCHITECTURE.md`.

## Platform and Dependency Profile

### Core Framework
- WPEFramework / Thunder plugin framework
- JSON-RPC dispatch through Thunder plugin APIs

### TV Build Dependencies
- `tvsettings-hal`
- `IARMBus`
- `tr181api`
- `ds` (Device Settings)
- `glib`, `pthread`

## Integration Benefits

- Unified JSON-RPC control plane for output and picture settings
- Build-time specialization keeps one plugin identity with platform-specific implementation depth
- Callback-driven notification model for UI reactivity when runtime video characteristics change

## Versioning

The plugin is versioned using explicit API version macros in `plugin/AVOutput.cpp` and registered via `SERVICE_REGISTRATION`.
Current declared API version is:
- Major: 1
- Minor: 1
- Patch: 0

## Typical Use Cases

- TV settings UIs that need complete picture control coverage
- Device setup experiences that tune output profiles and latency behavior
- Runtime overlays that react to frame rate, resolution, or content-type changes
- Operator diagnostics that query capabilities and current output state
