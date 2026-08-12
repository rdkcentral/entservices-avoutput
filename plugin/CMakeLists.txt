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

# Added via add_subdirectory() from the repository root.
# Avoid calling project()/cmake_minimum_required() or forcing CMAKE_BUILD_TYPE here.

set(PLUGIN_NAME AVOutput)

if(NOT DEFINED NAMESPACE OR NAMESPACE STREQUAL "")
    set(NAMESPACE "WPEFramework" CACHE STRING "WPEFramework namespace")
endif()

set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})

set(PLUGIN_AVOUTPUT_AUTOSTART "true" CACHE STRING "Automatically start AVOutput plugin")

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
# DS_COMRPC/: AVOutputTV.cpp only (COM-RPC, no libds)
# DS_IARM/:   AVOutputTV.cpp only (libds/IARM)
# Root AVOutputTVHelper.cpp handles both paths via #ifndef DS_COMRPC compile-time macros
# ---------------------------------------------------------------------------
if (DS_COMRPC)
    message(STATUS "AVOutput: DS_COMRPC=ON -- building AVOutputTV from DS_COMRPC/ (callsign=${PLUGIN_AVOUTPUT_DEVICESETTINGS_CALLSIGN})")
    # entservices-helpers must be in the sysroot (add to recipe DEPENDS)
    find_package(${NAMESPACE}Helpers REQUIRED)
    set(DS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DS_COMRPC)
    add_definitions(-DDS_COMRPC)
    add_definitions(-DPLUGIN_AVOUTPUT_DEVICESETTINGS_CALLSIGN="${PLUGIN_AVOUTPUT_DEVICESETTINGS_CALLSIGN}")
else()
    message(STATUS "AVOutput: DS_COMRPC=OFF -- building AVOutputTV from DS_IARM/ (libds / IARM)")
    set(DS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/DS_IARM)
endif()

if (AVOUTPUT_TV)
add_library(${MODULE_NAME} SHARED
	AVOutputBase.cpp
	${DS_SOURCE_DIR}/AVOutputTV.cpp
        AVOutputTVHelper.cpp	
        AVOutput.cpp
        Module.cpp)
target_compile_definitions(${MODULE_NAME}
    PRIVATE
        AVOUTPUT_TV=1
        DEVICE_TYPE=AVOutputTV)
target_link_libraries(${MODULE_NAME}
    PRIVATE
        glib-2.0
        ${NAMESPACE}::${NAMESPACE}Helpers
        pthread
        IARMBus
        tvsettings-hal
        tr181api)
else()
add_library(${MODULE_NAME} SHARED
	AVOutputBase.cpp
	AVOutputSTB.cpp
        AVOutput.cpp
        Module.cpp)
target_link_libraries(${MODULE_NAME}
    PRIVATE
        glib-2.0
        pthread
)
endif()

set_target_properties(${MODULE_NAME} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES)

if (NOT DS_COMRPC)
find_package(DS)
target_include_directories(${MODULE_NAME} PRIVATE ${DS_INCLUDE_DIRS})
target_link_libraries(${MODULE_NAME} PRIVATE ds)
endif()

target_include_directories(${MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${DS_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../helpers)
target_link_libraries(${MODULE_NAME} 
    PRIVATE
        ${NAMESPACE}Plugins::${NAMESPACE}Plugins)


install(TARGETS ${MODULE_NAME} 
    DESTINATION lib/wpeframework/plugins)

write_config(${PLUGIN_NAME})
