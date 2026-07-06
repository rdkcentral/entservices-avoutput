# If not stated otherwise in this file or this component's LICENSE file the
# following copyright and licenses apply:
#
# Copyright 2024 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required (VERSION 2.6)

Project(AVOUTPUT-PLUGINS)

set(CMAKE_BUILD_TYPE "Debug")

set(PLUGIN_NAME AVOutput)

if(NOT DEFINED NAMESPACE OR NAMESPACE STREQUAL "")
    set(NAMESPACE "WPEFramework" CACHE STRING "WPEFramework namespace")
endif()

set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})

set(PLUGIN_AVOUTPUT_AUTOSTART "true" CACHE STRING "Automatically start AVOutput plugin")

find_package(WPEFramework)
find_package(${NAMESPACE}Plugins REQUIRED)

set (CMAKE_CXX_STANDARD 11)

find_path (STAGING_INCDIR glib-2.0)
include_directories(${STAGING_INCDIR})
include_directories(${STAGING_INCDIR}/glib-2.0)
include_directories(${STAGING_INCDIR}/../lib/glib-2.0/include)
include_directories(${STAGING_INCDIR}/rdk/iarmbus)
include_directories(${STAGING_INCDIR}/rdk/iarmmgrs-hal)
include_directories(${STAGING_INCDIR}/rdk/tv-hal)
include_directories(${STAGING_INCDIR}/rdk/ds-hal)
include_directories(${STAGING_INCDIR}/rdk/ds-rpc)
include_directories(${STAGING_INCDIR}/rdk/ds)

# -----------------------------------------------------------------------
# DS_COMRPC: build AVOutputTV against the DeviceSettings Thunder plugin
#            via COM-RPC (sources in DS_COMRPC/, no libds / IARM dependency)
#            Only applies when AVOUTPUT_TV=ON.
# DS_IARM:   build against libds / IARM directly (sources in DS_IARM/).
#
# Select at configure time:
#   cmake -DDS_COMRPC=ON    (default: OFF -> DS_IARM is used)
# In Yocto:
#   EXTRA_OECMAKE += "-DDS_COMRPC=ON"
# -----------------------------------------------------------------------
option(DS_COMRPC
    "Build AVOutput TV using the DeviceSettings COM-RPC plugin (DS_COMRPC/).  OFF = build using libds/IARM (DS_IARM/)."
    OFF)

set(PLUGIN_AVOUTPUT_DEVICESETTINGS_CALLSIGN "org.rdk.DeviceSettings"
    CACHE STRING "Callsign of the DeviceSettings plugin (used by DS_COMRPC build)")

# ---------------------------------------------------------------------------
# Source directory selection (only relevant for AVOUTPUT_TV)
# DS_COMRPC/: AVOutputTV.cpp + AVOutputTVHelper.cpp (COM-RPC, no libds)
# DS_IARM/:   AVOutputTV.cpp only (libds/IARM); AVOutputTVHelper.cpp from root
# ---------------------------------------------------------------------------
if (DS_COMRPC)
    message(STATUS "AVOutput: DS_COMRPC=ON -- building AVOutputTV from DS_COMRPC/ (callsign=${PLUGIN_AVOUTPUT_DEVICESETTINGS_CALLSIGN})")
    set(DS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DS_COMRPC)
    add_definitions(-DDS_COMRPC)
    add_definitions(-DPLUGIN_AVOUTPUT_DEVICESETTINGS_CALLSIGN="${PLUGIN_AVOUTPUT_DEVICESETTINGS_CALLSIGN}")
else()
    message(STATUS "AVOutput: DS_COMRPC=OFF -- building AVOutputTV from DS_IARM/ (libds / IARM)")
    set(DS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DS_IARM)
endif()

if (AVOUTPUT_TV)
    add_library(${MODULE_NAME} SHARED
        ${DS_SOURCE_DIR}/AVOutputTV.cpp
        AVOutputTVHelper.cpp
        AVOutput.cpp
        AVOutputBase.cpp
        Module.cpp)
else()
    add_library(${MODULE_NAME} SHARED
        AVOutputSTB.cpp
        AVOutput.cpp
        AVOutputBase.cpp
        Module.cpp)
endif()

set_target_properties(${MODULE_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES)

# DS_SOURCE_DIR so the compiler finds AVOutputTV.h from the right subdirectory
target_include_directories(${MODULE_NAME} PRIVATE ${DS_SOURCE_DIR})
target_include_directories(${MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/helpers)

# ---------------------------------------------------------------------------
# Link dependencies depend on which implementation is selected
# ---------------------------------------------------------------------------
if (AVOUTPUT_TV)
    if (DS_COMRPC)
        # COM-RPC path: entservices-helpers provides DeviceSettingsClientHelper.h
        find_package(${NAMESPACE}Helpers REQUIRED)
        target_link_libraries(${MODULE_NAME}
            PRIVATE
                ${NAMESPACE}Plugins::${NAMESPACE}Plugins
                ${NAMESPACE}::${NAMESPACE}Helpers
                glib-2.0
                pthread
                tvsettings-hal
                tr181api)
    else()
        # libds / IARM path
        find_package(DS)
        target_include_directories(${MODULE_NAME} PRIVATE ${DS_INCLUDE_DIRS})
        target_link_libraries(${MODULE_NAME}
            PRIVATE
                ${NAMESPACE}Plugins::${NAMESPACE}Plugins
                glib-2.0
                pthread
                IARMBus
                tvsettings-hal
                tr181api
                ds)
    endif()
else()
    find_package(DS)
    target_include_directories(${MODULE_NAME} PRIVATE ${DS_INCLUDE_DIRS})
    target_link_libraries(${MODULE_NAME}
        PRIVATE
            ${NAMESPACE}Plugins::${NAMESPACE}Plugins
            glib-2.0
            pthread
            IARMBus
            ds)
endif()

install(TARGETS ${MODULE_NAME} 
    DESTINATION lib/wpeframework/plugins)

write_config(${PLUGIN_NAME})
