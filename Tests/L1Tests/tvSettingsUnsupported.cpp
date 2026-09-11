#include <cstddef>
#include <cstring>

#include <tvSettings.h>

namespace {
tvConfigContext_t numericPictureContext = {
    PQ_MODE_STANDARD,
    VIDEO_FORMAT_SDR,
    VIDEO_SOURCE_IP
};

tvContextCaps_t numericPictureCapabilities = {
    1,
    &numericPictureContext
};

tvError_t getNumericCapabilities(int* maximum, tvContextCaps_t** capabilities)
{
    if (maximum == nullptr || capabilities == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *maximum = 100;
    *capabilities = &numericPictureCapabilities;
    return tvERROR_NONE;
}
}

extern "C" {

#define UNSUPPORTED_TV_SETTING(name, ...) \
    tvError_t name(__VA_ARGS__) { return tvERROR_OPERATION_NOT_SUPPORTED; }
#define SUCCESSFUL_TV_SETTING(name, ...) \
    tvError_t name(__VA_ARGS__) { return tvERROR_NONE; }

SUCCESSFUL_TV_SETTING(TvInit)
SUCCESSFUL_TV_SETTING(TvTerm)

tvError_t RegisterVideoFormatChangeCB(tvVideoFormatCallbackData* callbackData)
{
    return callbackData != nullptr && callbackData->cb != nullptr && callbackData->userdata != nullptr
        ? tvERROR_NONE
        : tvERROR_INVALID_PARAM;
}

SUCCESSFUL_TV_SETTING(RegisterVideoContentChangeCB, tvVideoContentCallbackData*)
SUCCESSFUL_TV_SETTING(RegisterVideoResolutionChangeCB, tvVideoResolutionCallbackData*)
SUCCESSFUL_TV_SETTING(RegisterVideoFrameRateChangeCB, tvVideoFrameRateCallbackData*)
UNSUPPORTED_TV_SETTING(GetTVSupportedVideoFormats, tvVideoFormatType_t*[], unsigned short*)

tvError_t GetCurrentVideoFormat(tvVideoFormatType_t* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *value = VIDEO_FORMAT_SDR;
    return tvERROR_NONE;
}

tvError_t GetCurrentVideoResolution(tvResolutionParam_t* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    std::memset(value, 0, sizeof(*value));
    return tvERROR_NONE;
}

tvError_t GetCurrentVideoFrameRate(tvVideoFrameRate_t* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *value = tvVideoFrameRate_NONE;
    return tvERROR_NONE;
}

tvError_t GetCurrentVideoSource(tvVideoSrcType_t* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *value = VIDEO_SOURCE_IP;
    return tvERROR_NONE;
}

tvError_t GetBacklight(int* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *value = 50;
    return tvERROR_NONE;
}

tvError_t GetBrightness(int* value)
{
    return GetBacklight(value);
}

tvError_t GetContrast(int* value)
{
    return GetBacklight(value);
}

SUCCESSFUL_TV_SETTING(SetBacklight, int)
SUCCESSFUL_TV_SETTING(SaveBacklight, tvVideoSrcType_t, int, tvVideoFormatType_t, int)
SUCCESSFUL_TV_SETTING(SetCurrentBacklightMode, tvBacklightMode_t)
SUCCESSFUL_TV_SETTING(SetTVDimmingMode, const char*)
SUCCESSFUL_TV_SETTING(SaveTVDimmingMode, tvVideoSrcType_t, int, tvVideoFormatType_t, tvDimmingMode_t)
SUCCESSFUL_TV_SETTING(SetBrightness, int)
SUCCESSFUL_TV_SETTING(SaveBrightness, tvVideoSrcType_t, int, tvVideoFormatType_t, int)
SUCCESSFUL_TV_SETTING(SetContrast, int)
SUCCESSFUL_TV_SETTING(SaveContrast, tvVideoSrcType_t, int, tvVideoFormatType_t, int)
SUCCESSFUL_TV_SETTING(SetSharpness, int)
SUCCESSFUL_TV_SETTING(SaveSharpness, tvVideoSrcType_t, int, tvVideoFormatType_t, int)
SUCCESSFUL_TV_SETTING(SetSaturation, int)
SUCCESSFUL_TV_SETTING(SaveSaturation, tvVideoSrcType_t, int, tvVideoFormatType_t, int)
SUCCESSFUL_TV_SETTING(SetHue, int)
SUCCESSFUL_TV_SETTING(SaveHue, tvVideoSrcType_t, int, tvVideoFormatType_t, int)
SUCCESSFUL_TV_SETTING(SetColorTemperature, tvColorTemp_t)
SUCCESSFUL_TV_SETTING(SaveColorTemperature, tvVideoSrcType_t, int, tvVideoFormatType_t, tvColorTemp_t)
SUCCESSFUL_TV_SETTING(SetAspectRatio, tvDisplayMode_t)
tvError_t GetAspectRatio(tvDisplayMode_t* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *value = tvDisplayMode_NORMAL;
    return tvERROR_NONE;
}
SUCCESSFUL_TV_SETTING(SaveAspectRatio, tvVideoSrcType_t, int, tvVideoFormatType_t, tvDisplayMode_t)
SUCCESSFUL_TV_SETTING(SetLowLatencyState, int)
tvError_t GetLowLatencyState(int* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *value = 0;
    return tvERROR_NONE;
}
SUCCESSFUL_TV_SETTING(SaveLowLatencyState, tvVideoSrcType_t, int, tvVideoFormatType_t, int)
UNSUPPORTED_TV_SETTING(GetTVSupportedDolbyVisionModes, tvDolbyMode_t*[], unsigned short*)
SUCCESSFUL_TV_SETTING(SetTVDolbyVisionMode, tvDolbyMode_t)
SUCCESSFUL_TV_SETTING(SaveTVDolbyVisionMode, tvVideoSrcType_t, int, tvVideoFormatType_t, tvDolbyMode_t)
UNSUPPORTED_TV_SETTING(GetTVSupportedPictureModes, pic_modes_t*[], unsigned short*)

tvError_t GetTVPictureMode(char* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    std::strncpy(value, "Standard", PIC_MODE_NAME_MAX - 1);
    value[PIC_MODE_NAME_MAX - 1] = '\0';
    return tvERROR_NONE;
}
SUCCESSFUL_TV_SETTING(SetTVPictureMode, const char*)
SUCCESSFUL_TV_SETTING(SaveSourcePictureMode, tvVideoSrcType_t, tvVideoFormatType_t, int)
SUCCESSFUL_TV_SETTING(SetCurrentComponentSaturation, tvDataComponentColor_t, int)
SUCCESSFUL_TV_SETTING(SetCurrentComponentHue, tvDataComponentColor_t, int)
SUCCESSFUL_TV_SETTING(SetCurrentComponentLuma, tvDataComponentColor_t, int)
SUCCESSFUL_TV_SETTING(SaveCMS, tvVideoSrcType_t, int, tvVideoFormatType_t, tvComponentType_t, tvDataComponentColor_t, int)
SUCCESSFUL_TV_SETTING(SetCMSState, bool)

tvError_t GetDefaultPQParams(int, tvVideoSrcType_t, tvVideoFormatType_t, tvPQParameterIndex_t, int* value)
{
    if (value == nullptr) {
        return tvERROR_INVALID_PARAM;
    }

    *value = 50;
    return tvERROR_NONE;
}

SUCCESSFUL_TV_SETTING(SetCustom2PointWhiteBalance, tvWBColor_t, tvWBControl_t, int)
SUCCESSFUL_TV_SETTING(SaveCustom2PointWhiteBalance, tvVideoSrcType_t, int, tvVideoFormatType_t, tvWBColor_t, tvWBControl_t, int)

UNSUPPORTED_TV_SETTING(GetTVSupportedVideoSources, tvVideoSrcType_t*[], unsigned short*)
tvError_t GetBacklightCaps(int* maximum, tvContextCaps_t** capabilities)
{
    return getNumericCapabilities(maximum, capabilities);
}
UNSUPPORTED_TV_SETTING(GetTVDimmingModeCaps, tvDimmingMode_t**, size_t*, tvContextCaps_t**)
tvError_t GetBrightnessCaps(int* maximum, tvContextCaps_t** capabilities)
{
    return getNumericCapabilities(maximum, capabilities);
}
tvError_t GetContrastCaps(int* maximum, tvContextCaps_t** capabilities)
{
    return getNumericCapabilities(maximum, capabilities);
}
UNSUPPORTED_TV_SETTING(GetSharpnessCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetSaturationCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetHueCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetColorTemperatureCaps, tvColorTemp_t**, size_t*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetAspectRatioCaps, tvDisplayMode_t**, size_t*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetLowLatencyStateCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetTVPictureModeCaps, tvPQModeIndex_t**, size_t*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(EnableWBCalibrationMode, bool)
UNSUPPORTED_TV_SETTING(GetCurrentWBCalibrationMode, bool*)
UNSUPPORTED_TV_SETTING(GetPrecisionDetailCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetPrecisionDetail, tvVideoSrcType_t, tvPQModeIndex_t, tvVideoFormatType_t, int)
UNSUPPORTED_TV_SETTING(GetSdrGammaCaps, tvSdrGamma_t**, size_t*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetSdrGamma, tvVideoSrcType_t, tvPQModeIndex_t, tvSdrGamma_t)
UNSUPPORTED_TV_SETTING(GetLocalContrastEnhancementCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetLocalContrastEnhancement, tvVideoSrcType_t, tvPQModeIndex_t, tvVideoFormatType_t, int)
UNSUPPORTED_TV_SETTING(GetMPEGNoiseReductionCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetMPEGNoiseReduction, tvVideoSrcType_t, tvPQModeIndex_t, tvVideoFormatType_t, int)
UNSUPPORTED_TV_SETTING(GetDigitalNoiseReductionCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetDigitalNoiseReduction, tvVideoSrcType_t, tvPQModeIndex_t, tvVideoFormatType_t, int)
UNSUPPORTED_TV_SETTING(GetAISuperResolutionCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetAISuperResolution, tvVideoSrcType_t, tvPQModeIndex_t, tvVideoFormatType_t, int)
UNSUPPORTED_TV_SETTING(GetMEMCCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetMEMC, tvVideoSrcType_t, tvPQModeIndex_t, tvVideoFormatType_t, int)
UNSUPPORTED_TV_SETTING(GetMultiPointWBCaps, int*, int*, int*, int*, double**, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetCMSCaps, int*, int*, int*, tvDataComponentColor_t**, tvComponentType_t**, size_t*, size_t*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(GetBacklightModeCaps, tvBacklightMode_t**, size_t*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SaveBacklightMode, tvVideoSrcType_t, int, tvVideoFormatType_t, tvBacklightMode_t)
UNSUPPORTED_TV_SETTING(Set2PointWB, tvColorTemp_t, tvWBColor_t, tvWBControl_t, int)
UNSUPPORTED_TV_SETTING(Get2PointWBCaps, int*, int*, int*, int*, tvWBColor_t**, tvColorTemp_t**, tvWBControl_t**, size_t*, size_t*, size_t*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(Save2PointWB, tvVideoSrcType_t, int, tvVideoFormatType_t, tvColorTemp_t, tvWBColor_t, tvWBControl_t, int)
UNSUPPORTED_TV_SETTING(GetDefault2PointWB, tvVideoSrcType_t, int, tvVideoFormatType_t, tvColorTemp_t, tvWBColor_t, tvWBControl_t, int*)
UNSUPPORTED_TV_SETTING(GetBacklightDimmingLevelCaps, int*, tvContextCaps_t**)
UNSUPPORTED_TV_SETTING(SetBacklightDimmingLevel, tvVideoSrcType_t, tvPQModeIndex_t, tvVideoFormatType_t, int)

#undef UNSUPPORTED_TV_SETTING
#undef SUCCESSFUL_TV_SETTING

}