---
applyTo: "**/**Implementation.cpp,**/**Implementation.h,**/**.cpp,**/**.h"
---

# Instruction Summary
1. Open Source Contribution Guidance
2. JSON-RPC Method Registration
3. Event Notifications
4. IARM Integration

### Open Source Contribution Guidance

### Requirement

This instruction file should guide contributors on maintainable plugin behavior, not only describe current implementation internals.

- Prefer documenting contribution-safe patterns (where to register handlers, where to clean up resources, where to add new plugin interactions).
- Keep examples minimal and tied to stable extension points used in this repository.
- When adding new behavior, include notes about test impact in `Tests/` and any workflow flag changes needed for CI.

### JSON-RPC Method Registration

### Requirement

AVOutput implementation classes register JSON-RPC handlers manually. In the TV path, registrations are grouped in the `AVOutputTV` constructor and use the local `registerMethod` helper so every active handler returned by `GetHandler(i)` receives the same method mapping.

### Example

```cpp
#define registerMethod(...) for (uint8_t i = 1; GetHandler(i); i++) GetHandler(i)->Register<JsonObject, JsonObject>(__VA_ARGS__)

AVOutputTV::AVOutputTV()
    : m_currentHdmiInResoluton(dsVIDEO_PIXELRES_1920x1080)
    , m_videoZoomMode(tvDisplayMode_NORMAL)
    , m_isDisabledHdmiIn4KZoom(false)
    , rfc_caller_id()
{
    InitializeIARM();

    registerMethod("getBacklight", &AVOutputTV::getBacklight, this);
    registerMethod("getVideoFormat", &AVOutputTV::getVideoFormat, this);
    registerMethod("setZoomMode", &AVOutputTV::setZoomMode, this);
    registerMethod("resetZoomMode", &AVOutputTV::resetZoomMode, this);
}
```

### Event Notifications

### Requirement

AVOutputTV publishes JSON-RPC events with `sendNotify(...)` from dedicated notification helpers. Keep event payload construction local to the helper and emit the framework event only after the response object is complete.

### Example

```cpp
void AVOutputTV::NotifyVideoFormatChange(tvVideoFormatType_t format)
{
    JsonObject response;
    response["currentVideoFormat"] = getVideoFormatTypeToString(format);
    sendNotify("onVideoFormatChanged", response);
}
```

### IARM Integration

### Requirement

AVOutputTV owns its IARM event hookups. Initialize IARM in the constructor path and remove every registered handler in the matching deinitialization path.

### Example

```cpp
void AVOutputTV::InitializeIARM()
{
    AVOutputBase::InitializeIARM();
#if !defined (HDMIIN_4K_ZOOM)
    if (Utils::IARM::init()) {
        IARM_Result_t res;
        IARM_CHECK(IARM_Bus_RegisterEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_STATUS, dsHdmiStatusEventHandler));
        IARM_CHECK(IARM_Bus_RegisterEventHandler(IARM_BUS_DSMGR_NAME, IARM_BUS_DSMGR_EVENT_HDMI_IN_VIDEO_MODE_UPDATE, dsHdmiVideoModeEventHandler));
    }
#endif
}
```

Apply the same symmetry in `DeinitializeIARM()` by removing each handler registered in `InitializeIARM()`.
