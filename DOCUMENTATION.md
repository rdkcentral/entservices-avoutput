# AVOutput Plugin Documentation

## Overview

The **AVOutput** plugin is a Thunder/WPEFramework plugin developed and maintained by RDK Management (RDK Central) for RDK-based devices. This plugin provides comprehensive audio-video output management capabilities through a JSON-RPC interface, enabling applications to control and monitor various picture quality (PQ) parameters, video settings, and display configurations on both Set-Top Box (STB) and Smart TV devices.

### Purpose and Scope

AVOutput serves as the middleware layer between applications and the underlying TV/Display Hardware Abstraction Layer (HAL), providing:

- **Picture Quality Management**: Control over brightness, contrast, sharpness, saturation, hue, backlight, and advanced PQ features
- **Video Format Detection**: Real-time monitoring of video resolution, frame rate, format (SDR/HDR/Dolby Vision), and content type
- **Display Mode Control**: Aspect ratio, zoom mode, and low latency state management
- **Color Management**: Color temperature, color management system (CMS), white balance (WB), and HDR mode control
- **Advanced Features**: Auto backlight mode, AI super resolution, MEMC (Motion Estimation Motion Compensation), noise reduction, and more

The plugin integrates with the IARM (Inter-Application Resource Manager) bus for system-wide communication and utilizes TR-181 data model for persistent configuration storage.

---

## Architecture and Components

### Component Structure

The plugin follows an object-oriented architecture with platform-specific implementations:

```
AVOutput (Main Plugin)
    ├── AVOutputBase (Common Base Class)
    │   └── Provides JSONRPC interface and common initialization/deinitialization
    │
    ├── AVOutputTV (TV Platform Implementation)
    │   ├── Picture Quality APIs (get/set/reset for ~20+ parameters)
    │   ├── Capability APIs (V1 and V2 versions)
    │   ├── TV Settings HAL Integration
    │   ├── TR-181 Configuration Management
    │   └── Event Notification System
    │
    └── AVOutputSTB (STB Platform Implementation)
        └── Simplified interface for Set-Top Box devices
```

### Device Type Selection

The plugin automatically selects the appropriate implementation at compile time:

- **TV Platform**: Built with `AVOUTPUT_TV` flag, includes extensive PQ features and TV settings HAL
- **STB Platform**: Default build without additional flags, provides basic AV output functionality

### Key Files

| File | Purpose |
|------|---------|
| `AVOutput.cpp/h` | Main plugin entry point and initialization |
| `AVOutputBase.cpp/h` | Common base class with JSONRPC foundation |
| `AVOutputTV.cpp/h` | TV-specific implementation with full PQ feature set |
| `AVOutputSTB.cpp/h` | STB-specific implementation |
| `AVOutputTVHelper.cpp` | Helper utilities for TV platform |
| `Module.cpp/h` | WPEFramework module definitions |
| `CMakeLists.txt` | Build configuration |

### Dependencies

**Core Libraries:**
- WPEFramework (Thunder framework)
- IARM Bus (Inter-Application Resource Manager)
- Display Settings (DS) HAL
- GLib 2.0 (for data structures and threading)

**TV Platform Additional:**
- TV Settings HAL (`tvsettings-hal`)
- TR-181 API (`tr181api`)
- Boost (filesystem, property_tree for INI parsing)

---

## API Categories and Features

### 1. Picture Quality Parameters

The plugin provides comprehensive control over picture quality settings with context-aware capabilities:

**Basic PQ Parameters:**
- **Backlight**: 0-100 scale, controls display brightness
- **Brightness**: Adjusts black level
- **Contrast**: Adjusts white level  
- **Sharpness**: Edge enhancement control
- **Saturation**: Color intensity
- **Hue**: Color tint adjustment

**Advanced PQ Features:**
- **Color Temperature**: Warm, neutral, cool, custom settings
- **Backlight Dimming Mode**: Local, fixed, various dimming algorithms
- **Picture Mode**: Standard, vivid, sports, movie, filmmaker mode, etc.
- **Dolby Vision Mode**: Bright, dark, game modes
- **HDR Mode**: HDR10, HDR10+, HLG support
- **Zoom Mode**: Aspect ratio control (16:9, 4:3, auto, zoom, etc.)

**Image Enhancement:**
- **AI Super Resolution**: AI-powered upscaling
- **Precision Detail**: Detail enhancement
- **Local Contrast Enhancement**: Adaptive contrast
- **MPEG Noise Reduction**: Compression artifact reduction
- **Digital Noise Reduction**: General noise reduction
- **MEMC**: Motion smoothing
- **SDR Gamma**: Gamma curve adjustment

### 2. Video Information APIs

Real-time video stream information:

- `getVideoFormat`: Returns current format (SDR/HDR10/HDR10+/DolbyVision/HLG)
- `getVideoSource`: Current input source (HDMI, tuner, composite, etc.)
- `getVideoResolution`: Current resolution
- `getVideoFrameRate`: Current frame rate
- `getVideoContentType`: Content type detection (FMM - Filmmaker Mode)

### 3. Capability APIs

Two versions of capability APIs are provided:

**V1 APIs** (Legacy): Simple capability queries
- Example: `getBacklightCaps`, `getBrightnessCaps`

**V2 APIs** (Context-Aware): Advanced queries with picture mode, source, and format context
- Example: `getBacklightCapsV2`, `getPictureModeCapsV2`
- Returns capabilities specific to the requested context
- Supports filtering by picture mode, video source, and video format

### 4. Color Management System (CMS)

Fine-grained color control:
- **Components**: Hue, Saturation, Luma
- **Colors**: Red, Green, Blue, Cyan, Magenta, Yellow
- APIs: `getCMS`, `setCMS`, `resetCMS`, `getCMSCaps`

### 5. White Balance (WB)

Multi-point white balance calibration:
- **2-Point White Balance**: Red, Green, Blue gain/offset control
- **Multi-Point White Balance**: Advanced calibration with multiple color points
- APIs: `get2PointWB`, `set2PointWB`, `reset2PointWB`, `setWBCtrl`

---

## Configuration and Integration

### Build Configuration

**Basic Build (STB):**
```cmake
cmake ..
make
```

**TV Build:**
```cmake
cmake -DAVOUTPUT_TV=ON ..
make
```

### Installation

The plugin is installed to: `/lib/wpeframework/plugins/`

### Plugin Configuration

The plugin uses WPEFramework configuration with:
- **Callsign**: `org.rdk.AVOutput`
- **Autostart**: Enabled by default
- **Configuration Files**: 
  - `AVOutput.config` - WPEFramework plugin config
  - `pq_capabilities.ini` - Picture quality capabilities (platform-specific)
  - `AVOutput.ini` - Override settings (optional, `/opt/panel/`)

### TR-181 Integration

The TV platform implementation integrates with TR-181 data model for persistent storage:

- **Parameter Namespace**: `Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.AVOutput.*`
- **Automatic Sync**: Settings synchronized to TR-181 on set/reset operations
- **Boot-time Restoration**: Last saved settings restored during initialization

### RFC (Remote Feature Control)

Several features can be controlled via RFC:
- Auto Backlight Mode
- Dolby Vision Mode
- HLG Mode
- HDR10 Mode
- Dimming Mode
- Picture Mode
- Aspect Ratio (Zoom Mode)
- Dynamic Auto Latency Service (DALS)

### Event Notifications

The plugin publishes events for:
- Video format changes
- Video resolution changes
- Video frame rate changes
- Filmmaker Mode detection
- Low latency mode changes

---

## API Usage Examples

### Setting Picture Parameters

```json
// Set backlight for current picture mode
{
  "jsonrpc": "2.0",
  "method": "org.rdk.AVOutput.setBacklight",
  "params": {
    "backlight": 50
  }
}

// Set backlight with context (V2)
{
  "jsonrpc": "2.0",
  "method": "org.rdk.AVOutput.setBacklight",
  "params": {
    "backlight": 50,
    "pictureMode": "standard",
    "videoSource": "hdmi",
    "videoFormat": "sdr"
  }
}
```

### Getting Video Information

```json
// Get current video format
{
  "jsonrpc": "2.0",
  "method": "org.rdk.AVOutput.getVideoFormat",
  "params": {}
}

// Response
{
  "result": {
    "currentVideoFormat": "HDR10",
    "success": true
  }
}
```

### Querying Capabilities

```json
// Get backlight capabilities with context
{
  "jsonrpc": "2.0",
  "method": "org.rdk.AVOutput.getBacklightCapsV2",
  "params": {
    "pictureModes": ["standard", "vivid"],
    "videoSources": ["hdmi"],
    "videoFormats": ["sdr", "hdr10"]
  }
}

// Response includes min, max, default values per context
{
  "result": {
    "capabilities": [
      {
        "pictureMode": "standard",
        "videoSource": "hdmi",
        "videoFormat": "sdr",
        "min": 0,
        "max": 100,
        "default": 50
      }
    ],
    "success": true
  }
}
```

---

## Development and Testing

### Helper Utilities

The `helpers/` directory contains utility modules:
- **UtilsLogging**: Logging macros and functions
- **UtilsJsonRpc**: JSON-RPC helper utilities
- **UtilsIarm**: IARM bus utilities
- **UtilsTelemetry**: Telemetry reporting
- **UtilsFile**: File operations
- **UtilsThreadRAII**: Thread management
- And more...

### Thread Safety

The TV implementation includes a thread pool for non-blocking parameter updates:
- Worker thread processes parameter update queue
- Mutex and condition variable for synchronization
- Atomic flags for thread control

### Error Handling

Comprehensive error handling with:
- TV Settings HAL error code translation
- Detailed error strings via `getErrorString()`
- Input validation for all parameters
- Capability checking before parameter operations

---

## Version History

Current version: **1.0.0**

Notable milestones:
- Initial release with Thunder integration
- Advanced PQ parameters implementation (V2 APIs)
- CMS and White Balance support
- Dynamic capability system
- Multi-context parameter management

For detailed changelog, see `CHANGELOG.md`.

---

## License

Copyright 2024 RDK Management

Licensed under the Apache License, Version 2.0. See file headers for full license text.

---

## Contributing

This repository is maintained by RDK Central. For issues, pull requests, or questions:

- **Repository**: https://github.com/rdkcentral/entservices-avoutput
- **Code Owners**: See `.github/CODEOWNERS`

---

*This documentation reflects the state of the AVOutput plugin as of version 1.0.0. For the most up-to-date information, please refer to the source code and inline documentation.*
