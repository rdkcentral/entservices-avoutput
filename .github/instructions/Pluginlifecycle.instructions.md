---
description: Lifecycle requirements for Thunder plugins.
applyTo: "**/*.cpp,**/*.h"
---

### Mandatory Lifecycle Methods

Every plugin must implement:

- Initialize(IShell* service) -> Called when the plugin is activated.

- Deinitialize(IShell* service) -> Called when the plugin is deactivated.

### Initialization

### Requirement

- `AVOutput::Initialize()` validates the runtime profile, updates `_skipURL` from the shell web prefix, and delegates to the selected device-type implementation.
- `AVOutputTV::Initialize()` performs platform setup, registers TV callbacks, and synchronizes TV parameters after the plugin entry point hands control to the device type.
- `AVOutputTV` also initializes IARM and method registration in its constructor before `AVOutput::Initialize()` calls `DEVICE_TYPE::Initialize()`.

### Example

```cpp
const string AVOutput::Initialize(PluginHost::IShell* service)
{
    profileType = searchRdkProfile();
    if (profileType == STB || profileType == NOT_FOUND) {
        return std::string("Not supported");
    }

    if (service == nullptr) {
        return _T("No service.");
    }

    _skipURL = static_cast<uint8_t>(service->WebPrefix().length());
    DEVICE_TYPE::Initialize();

    return string();
}
```

TV-specific setup continues in the device implementation:

```cpp
void AVOutputTV::Initialize()
{
    tvError_t ret = TvInit();

    tvVideoFormatCallbackData callbackData = {this, tvVideoFormatChangeHandler};
    RegisterVideoFormatChangeCB(&callbackData);

    tvVideoContentCallbackData conCallbackData = {this, tvFilmMakerModeChangeHandler};
    RegisterVideoContentChangeCB(&conCallbackData);

    tvVideoResolutionCallbackData resCallbackData = {this, tvVideoResolutionChangeHandler};
    RegisterVideoResolutionChangeCB(&resCallbackData);

    tvVideoFrameRateCallbackData fpsCallbackData = {this, tvVideoFrameRateChangeHandler};
    RegisterVideoFrameRateChangeCB(&fpsCallbackData);

    syncAvoutputTVParamsToHAL("none", "none", "none");
    syncAvoutputTVPQModeParamsToHAL("Current", "none", "none");
    initializePictureMode();
}
```

### Deinitialize and Cleanup

### Requirement

- `AVOutput::Deinitialize()` rechecks the runtime profile and delegates cleanup to the selected device-type implementation.
- `AVOutputTV::~AVOutputTV()` stops the worker thread and tears down IARM registrations.
- `AVOutputTV::Deinitialize()` terminates the TV platform with `TvTerm()`.

### Example

```cpp
void AVOutput::Deinitialize(PluginHost::IShell* service)
{
    profileType = searchRdkProfile();
    if (profileType == STB || profileType == NOT_FOUND) {
        return;
    }

    DEVICE_TYPE::Deinitialize();
}
```

TV implementation cleanup is split between `Deinitialize()` and the destructor:

```cpp
void AVOutputTV::Deinitialize()
{
    TvTerm();
}
```

### Example

```cpp
AVOutputTV::~AVOutputTV()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        shouldStopWorker = true;
    }
    queueCondition.notify_all();

    if (workerThread.joinable()) {
        workerThread.join();
    }

    DeinitializeIARM();
}
```
