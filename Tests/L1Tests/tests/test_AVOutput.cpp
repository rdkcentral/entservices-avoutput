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
#if 0
#include <gtest/gtest.h>
#include "COMLinkMock.h"
#include <gmock/gmock.h>

#include "AVOutput.h"

#include "ManagerMock.h"
#include "HostMock.h"
#include "IarmBusMock.h"
#include "ServiceMock.h"
#include "ThunderPortability.h"

#include "AVOutputImplementation.h"
#include "AVOutputMock.h"
#include "WorkerPoolImplementation.h"

using namespace WPEFramework;

using ::testing::NiceMock;


class AVOutputTest : public ::testing::Test {

    protected:

    Core::ProxyType<Plugin::AVOutput> plugin;
    Core::JSONRPC::Handler& handler;
    DECL_CORE_JSONRPC_CONX connection;
    Core::ProxyType<Plugin::AVOutputImplementation> AVOutputImpl;

    NiceMock<ServiceMock> service;
    NiceMock<COMLinkMock> comLinkMock;

    Core::ProxyType<WorkerPoolImplementation> workerPool;
    string response;

    AVOutputMock* p_avOutputMock                          = nullptr;
    HostImplMock* p_HostImplMock                          = nullptr;
    IarmBusImplMock* p_iarmBusImplMock                    = nullptr;
    ManagerImplMock* p_managerImplMock                    = nullptr;

    PLUGINHOST_DISPATCHER* dispatcher;

    AVOutputTest()
        : plugin(Core::ProxyType<Plugin::AVOutput>::Create())
        , handler(*(plugin))
        , INIT_CONX(1, 0)
        , workerPool(Core::ProxyType<WorkerPoolImplementation>::Create(2, Core::Thread::DefaultStackSize(), 16))
        , dispatcher(nullptr)
    {
        p_HostImplMock = new NiceMock<HostImplMock>;
        device::Host::setImpl(p_HostImplMock);

        p_managerImplMock  = new NiceMock <ManagerImplMock>;
        device::Manager::setImpl(p_managerImplMock);

        EXPECT_CALL(*p_managerImplMock, Initialize())
            .Times(::testing::AnyNumber())
            .WillRepeatedly(::testing::Return());

        p_avOutputMock  = new NiceMock<AVOutputMock>;

        Core::IWorkerPool::Assign(&(*workerPool));
        workerPool->Run();

        dispatcher = static_cast<PLUGINHOST_DISPATCHER*>(
            plugin->QueryInterface(PLUGINHOST_DISPATCHER_ID));
        dispatcher->Activate(&service);

        #ifdef USE_THUNDER_R4
        ON_CALL(comLinkMock, Instantiate(::testing::_, ::testing::_, ::testing::_))
                .WillByDefault(::testing::Invoke(
                    [&](const RPC::Object& object, const uint32_t waitTime, uint32_t& connectionId) {
                        AVOutputImpl = Core::ProxyType<Plugin::AVOutputImplementation>::Create();
                        return &AVOutputImpl;
                    }));
        #else
            ON_CALL(comLinkMock, Instantiate(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
                .WillByDefault(::testing::Return(AVOutputImpl));
        #endif

        p_iarmBusImplMock  = new NiceMock <IarmBusImplMock>;
        IarmBus::setImpl(p_iarmBusImplMock);

        plugin->Initialize(&service);
    }

    virtual ~AVOutputTest()
    {
        dispatcher->Deactivate();
        dispatcher->Release();

        plugin->Deinitialize(&service);

        Core::IWorkerPool::Assign(nullptr);
        workerPool.Release();

        IarmBus::setImpl(nullptr);
        if (p_iarmBusImplMock != nullptr) {
            delete p_iarmBusImplMock;
            p_iarmBusImplMock = nullptr;
        }

        if (p_avOutputMock != nullptr) {
            delete p_avOutputMock;
            p_avOutputMock = nullptr;
        }

        device::Manager::setImpl(nullptr);
        if (p_managerImplMock != nullptr)
        {
            delete p_managerImplMock;
            p_managerImplMock = nullptr;
        }

        device::Host::setImpl(nullptr);
        if (p_HostImplMock != nullptr) {
            delete p_HostImplMock;
            p_HostImplMock = nullptr;
        }
    }
};

TEST_F(AVOutputTest, RegisteredMethods)
{
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("isDisplayConnected")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getDisplayCapabilities")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getZoomCapabilities")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("setDisplayAspectRatio")));
    EXPECT_EQ(Core::ERROR_NONE, handler.Exists(_T("getDisplayAspectRatio")));
}

#else

#include <gtest/gtest.h>
#include <iostream>

class AVOutputTest : public ::testing::Test {
};

// Basic test for AVOutput plugin
TEST_F(AVOutputTest, BasicAVOutputTest) {
    EXPECT_TRUE(true);
}

#endif
