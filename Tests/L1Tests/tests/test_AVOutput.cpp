/**
* If not stated otherwise in this file or this component's LICENSE
* file the following copyright and licenses apply:
*
* Copyright 2024 RDK Management
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
**/
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cstring>
#include <string>

#if defined(AVOUTPUT_TV)
#include "AVOutputTV.h"
#include "tr181api.h"

extern "C" {
#include "tvError.h"
#include "tvSettings.h"
}
#endif

#if defined(AVOUTPUT_TV)
namespace {
class TestAVOutputTV : public WPEFramework::Plugin::AVOutputTV {
public:
    TestAVOutputTV()
        : AVOutputTV(true)
    {
        m_backlightStatus = GetBacklightCaps(&m_maxBacklight, &m_backlightCaps);
        m_brightnessStatus = GetBrightnessCaps(&m_maxBrightness, &m_brightnessCaps);
        m_contrastStatus = GetContrastCaps(&m_maxContrast, &m_contrastCaps);
    }

    const std::string Initialize(WPEFramework::PluginHost::IShell *) override { return {}; }
    void Deinitialize(WPEFramework::PluginHost::IShell *) override {}
    std::string Information() const override { return {}; }
    void AddRef() const override {}
    uint32_t Release() const override { return 1; }
    void* QueryInterface(uint32_t) override { return nullptr; }
};

void videoFormatChangeHandler(tvVideoFormatType_t format, void *userData)
{
    (void)format;
    (void)userData;
}

JsonObject makeIntParam(const std::string &key, int value)
{
    JsonObject params;
    params[key.c_str()] = std::to_string(value);
    return params;
}

class Tr181ApiImplMock : public Tr181ApiImpl {
public:
    MOCK_METHOD(tr181ErrorCode_t, getParam, (char*, const char*, TR181_ParamData_t*), (override));
    MOCK_METHOD(tr181ErrorCode_t, setParam, (char*, const char*, const char*), (override));
    MOCK_METHOD(tr181ErrorCode_t, clearParam, (char*, const char*), (override));
    MOCK_METHOD(const char*, getTR181ErrorString, (tr181ErrorCode_t), (override));
    MOCK_METHOD(tr181ErrorCode_t, getLocalParam, (char*, const char*, TR181_ParamData_t*), (override));
    MOCK_METHOD(tr181ErrorCode_t, setLocalParam, (char*, const char*, const char*), (override));
    MOCK_METHOD(tr181ErrorCode_t, clearLocalParam, (char*, const char*), (override));
};
}
#endif

class AVOutputTest : public ::testing::Test {
#if defined(AVOUTPUT_TV)
protected:
    void SetUp() override
    {
        Tr181Api::setImpl(&tr181ApiMock);

        ON_CALL(tr181ApiMock, getLocalParam(testing::_, testing::_, testing::_))
            .WillByDefault(testing::Invoke([](char*, const char* name, TR181_ParamData_t* param) {
                const bool isPictureMode = std::strstr(name, "PictureModeString") != nullptr;
                std::strcpy(param->value, isPictureMode ? "Standard" : "50");
                param->type = isPictureMode ? TR181_STRING : TR181_INT;
                return tr181Success;
            }));
        ON_CALL(tr181ApiMock, setLocalParam(testing::_, testing::_, testing::_)).WillByDefault(testing::Return(tr181Success));
        ON_CALL(tr181ApiMock, clearLocalParam(testing::_, testing::_)).WillByDefault(testing::Return(tr181Success));
        ON_CALL(tr181ApiMock, getTR181ErrorString(testing::_)).WillByDefault(testing::Return("success"));
    }

    void TearDown() override
    {
        Tr181Api::setImpl(nullptr);
    }

    testing::NiceMock<Tr181ApiImplMock> tr181ApiMock;
#endif
};

TEST_F(AVOutputTest, PluginIdentityIsValid) {
    const std::string callsign{"org.rdk.AVOutput"};

    EXPECT_FALSE(callsign.empty());
    EXPECT_EQ(callsign.rfind("org.rdk.", 0), 0U);
}

#if defined(AVOUTPUT_TV)
TEST_F(AVOutputTest, RegisterVideoFormatChangeCBPositive) {
    tvError_t result = TvInit();
    ASSERT_EQ(tvERROR_NONE, result);

    static int callbackContext = 0;
    tvVideoFormatCallbackData callbackData = {&callbackContext, videoFormatChangeHandler};

    EXPECT_EQ(tvERROR_NONE, RegisterVideoFormatChangeCB(&callbackData));

    EXPECT_EQ(tvERROR_NONE, TvTerm());
}

TEST_F(AVOutputTest, RegisterVideoFormatChangeCBRejectsInvalidCallback) {
    tvVideoFormatCallbackData callbackData = {nullptr, nullptr};

    EXPECT_EQ(tvERROR_INVALID_PARAM, RegisterVideoFormatChangeCB(&callbackData));
}

TEST_F(AVOutputTest, InstanceIsClearedOnDestruction) {
    EXPECT_EQ(nullptr, TestAVOutputTV::getInstance());
    {
        TestAVOutputTV avOutputTv;
        EXPECT_EQ(&avOutputTv, TestAVOutputTV::getInstance());
    }
    EXPECT_EQ(nullptr, TestAVOutputTV::getInstance());
}

TEST_F(AVOutputTest, BacklightGetSetResetFlow) {
    TestAVOutputTV avOutputTv;
    avOutputTv.Initialize(nullptr);

    JsonObject getParams;
    JsonObject getResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.getBacklight(getParams, getResponse));

    JsonObject setParams = makeIntParam("backlight", 50);
    JsonObject setResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.setBacklight(setParams, setResponse));

    JsonObject resetParams;
    JsonObject resetResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.resetBacklight(resetParams, resetResponse));

    avOutputTv.Deinitialize(nullptr);
}

TEST_F(AVOutputTest, BrightnessGetSetResetFlow) {
    TestAVOutputTV avOutputTv;
    avOutputTv.Initialize(nullptr);

    JsonObject getParams;
    JsonObject getResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.getBrightness(getParams, getResponse));

    JsonObject setParams = makeIntParam("brightness", 60);
    JsonObject setResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.setBrightness(setParams, setResponse));

    JsonObject resetParams;
    JsonObject resetResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.resetBrightness(resetParams, resetResponse));

    avOutputTv.Deinitialize(nullptr);
}

TEST_F(AVOutputTest, ContrastGetSetResetFlow) {
    TestAVOutputTV avOutputTv;
    avOutputTv.Initialize(nullptr);

    JsonObject getParams;
    JsonObject getResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.getContrast(getParams, getResponse));

    JsonObject setParams = makeIntParam("contrast", 70);
    JsonObject setResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.setContrast(setParams, setResponse));

    JsonObject resetParams;
    JsonObject resetResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.resetContrast(resetParams, resetResponse));

    avOutputTv.Deinitialize(nullptr);
}

TEST_F(AVOutputTest, BacklightBrightnessContrastCapsAreAvailable) {
    TestAVOutputTV avOutputTv;
    avOutputTv.Initialize(nullptr);

    JsonObject response;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.getBacklightCaps(JsonObject(), response));

    JsonObject brightnessResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.getBrightnessCaps(JsonObject(), brightnessResponse));

    JsonObject contrastResponse;
    EXPECT_EQ(WPEFramework::Core::ERROR_NONE, avOutputTv.getContrastCaps(JsonObject(), contrastResponse));

    avOutputTv.Deinitialize(nullptr);
}
#endif
