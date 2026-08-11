# AVOutput Plugin Architecture

## Overview

AVOutput is a Thunder plugin that exposes picture and video-output controls for RDK devices.
It provides one plugin entry class (`AVOutput`) and delegates platform behavior to a build-selected device implementation (`AVOutputTV` or `AVOutputSTB`).

The plugin is registered with Thunder using:
- `SERVICE_REGISTRATION(AVOutput, API_VERSION_NUMBER_MAJOR, API_VERSION_NUMBER_MINOR, API_VERSION_NUMBER_PATCH)`

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│              Client Applications (JSON-RPC)                 │
└──────────────────┬──────────────────────────────────────────┘
           │
┌──────────────────▼──────────────────────────────────────────┐
│              Thunder / WPEFramework                         │
│  ┌───────────────────────────────────────────────────────┐  │
│  │               AVOutput Plugin (PluginHost)           │  │
│  │  ┌─────────────────────────────────────────────────┐  │  │
│  │  │ AVOutput (IPlugin + JSONRPC entry)             │  │  │
│  │  └────────────────┬────────────────────────────────┘  │  │
│  │                   │                                    │  │
│  │  ┌────────────────▼────────────────────────────────┐   │  │
│  │  │ DEVICE_TYPE (build-time selection)              │   │  │
│  │  │  - AVOutputTV (TV runtime path)                 │   │  │
│  │  │  - AVOutputSTB (STB runtime path)               │   │  │
│  │  └────────────────┬────────────────────────────────┘   │  │
│  │                   │                                    │  │
│  │  ┌────────────────▼────────────────────────────────┐   │  │
│  │  │ AVOutputBase (shared JSONRPC/plugin base)       │   │  │
│  │  │  - IARMBus integration                           │   │  │
│  │  │  - Runtime notifications (sendNotify)            │   │  │
│  │  └────────────────┬────────────────────────────────┘   │  │
│  └───────────────────┼────────────────────────────────────┘  │
└──────────────────────┼────────────────────────────────────────┘
             │
┌──────────────────────▼───────────────────────────────────────┐
│                 Platform Dependency Layer                    │
│  ┌─────────────────────────────────────────────────────────┐ │
│  │ HAL: tvsettings-hal (common TV-output HAL interface)   │ │
│  │  - panel output and backlight output controls (set/get APIs) │ │
│  │  - video callbacks (format/content/resolution/fps)      │ │
│  └─────────────────────────────────────────────────────────┘ │
│                                                              │
│  ┌──────────────────────┐ ┌──────────────────────────────┐  │
│  │ Platform service: ds │ │ Platform service: tr181api   │  │
│  │ - device state       │ │ - runtime parameters         │  │
│  │ - settings access    │ │ - RFC/param access           │  │
│  └──────────────────────┘ └──────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
```

Layer summary:
- Control plane: JSON-RPC requests enter through Thunder and are routed to `AVOutput`.
- Runtime specialization: `DEVICE_TYPE` selects `AVOutputTV` (TV path) or `AVOutputSTB` (STB path) at build time.
- Platform abstraction: TV runtime uses `tvsettings-hal` for HAL operations and separate platform services (`ds`, `tr181api`) for state and parameter access.

## Build-Time Composition

The plugin implementation is selected in `plugin/CMakeLists.txt`:
- If `AVOUTPUT_TV` is enabled:
  - Builds `AVOutputTV.cpp` + `AVOutputTVHelper.cpp`
  - Defines `DEVICE_TYPE=AVOutputTV`
  - Links TV platform dependencies: `IARMBus`, `tvsettings-hal`, `tr181api`, `ds`
- Otherwise:
  - Builds `AVOutputSTB.cpp`
  - Uses default `DEVICE_TYPE=AVOutputSTB`

This creates a single plugin module named `${NAMESPACE}AVOutput`.

## Core Components

### 1. AVOutput (Plugin Entry)

`AVOutput` is the Thunder-facing plugin entry class.

Responsibilities:
- Plugin lifecycle entry (`Initialize`, `Deinitialize`)
- Profile gate check via `searchRdkProfile()`
- Shell-derived setup (`_skipURL = service->WebPrefix().length()`)
- Delegation to selected device type:
  - `DEVICE_TYPE::Initialize()`
  - `DEVICE_TYPE::Deinitialize()`

### 2. AVOutputBase (Shared Base)

`AVOutputBase` derives from:
- `PluginHost::IPlugin`
- `PluginHost::JSONRPC`

Responsibilities:
- Shared JSON-RPC inheritance for device implementations
- Base lifecycle hooks (`Initialize`, `Deinitialize`)
- IARM hook points (`InitializeIARM`, `DeinitializeIARM`)
- Shared state such as `_skipURL`

### 3. AVOutputTV (TV Runtime Path)

`AVOutputTV` is the main feature implementation.

Responsibilities:
- Registers a large JSON-RPC surface in constructor using `registerMethod(...)`
- Initializes IARM and registers IARM event handlers
- Handles TV platform lifecycle initialization and termination (`TvInit`, `TvTerm`)
- Registers TV callbacks:
  - Video format
  - Video content type
  - Video resolution
  - Video frame rate
- Sends runtime notifications using `sendNotify(...)`, for example:
  - `onVideoFormatChanged`
  - `onVideoContentChanged`
  - `onVideoResolutionChanged`
  - `onVideoFrameRateChanged`
  - `gameModeEvent`
- Maintains worker thread for asynchronous parameter updates

### 4. AVOutputSTB (STB Runtime Path)

`AVOutputSTB` is a lightweight STB path implementation that derives from `AVOutputBase`.
It provides constructor/destructor and instance access with smaller runtime behavior than the TV path.

## Runtime Flows

### Initialization Flow

1. Thunder activates plugin and calls `AVOutput::Initialize(service)`
2. Plugin validates runtime profile
3. Plugin stores web prefix offset (`_skipURL`)
4. Plugin delegates to selected device implementation
5. TV path performs:
   - IARM initialization and handler registration
   - HAL initialization (`TvInit`)
   - callback registration
   - initial sync of AV output parameters

### Deinitialization Flow

1. Thunder deactivates plugin and calls `AVOutput::Deinitialize(service)`
2. Plugin validates runtime profile
3. Plugin delegates to selected device implementation
4. TV path performs:
   - HAL termination (`TvTerm`)
   - worker-thread stop/join in destructor
   - IARM handler removal and deinit

### Event Notification Flow (TV)

1. HAL or IARM callback fires (for format/content/resolution/frame-rate changes)
2. AVOutputTV callback handler builds `JsonObject` payload
3. `sendNotify(eventName, payload)` publishes JSON-RPC event to subscribers

## Configuration and Packaging

Configuration artifacts:
- `plugin/AVOutput.conf.in` for base Thunder config template
- `plugin/AVOutput.config` for resolved runtime defaults (for example callsign/autostart)

Packaging:
- `write_config(${PLUGIN_NAME})` generates/installs plugin configuration
- Plugin shared object is installed under Thunder plugin path

## External Dependencies

Primary dependencies (TV path):
- Thunder plugin framework (`${NAMESPACE}Plugins`)
- IARMBus
- tvsettings-hal
- tr181api
- ds (Device Settings)
- glib, pthread

## HAL Integration Details

AVOutput TV path uses a layered HAL interaction model comparable to AVInput, where plugin-facing APIs are separated from platform and hardware adaptation:

1. Control and policy entry (plugin layer)
- JSON-RPC methods are exposed by AVOutput runtime classes.
- Validation, request parsing, and event payload shaping are handled in plugin code before HAL invocation.

2. HAL adaptation layer (platform bridge)
- `tvsettings-hal` provides the common TV-output HAL API consumed by `AVOutputTV`.
- `ds` exposes device settings/state used by output capability and status paths.
- `tr181api` is used where runtime TR-181 parameters are required.

3. Event propagation layer (upstream notifications)
- HAL or IARM callbacks are transformed into JSON payloads.
- `sendNotify(...)` publishes runtime events to Thunder subscribers.

This separation keeps external API behavior stable while allowing SoC/platform implementations to evolve behind HAL interfaces.

## Design Notes

- The plugin intentionally uses build-time device specialization (`DEVICE_TYPE`) to keep one public plugin entry while allowing different platform behaviors.
- JSON-RPC methods are registered explicitly in implementation constructors, which makes ownership and method locality clear.
- Event publication is callback-driven and uses JSON payloads through Thunder notification APIs.
