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

if (AVOUTPUT_TV)
    if (TV_HAL_INCLUDE_DIR)
        include_directories(BEFORE ${TV_HAL_INCLUDE_DIR})
    endif()
    find_package(Boost REQUIRED COMPONENTS filesystem)
    if (NOT RDK_SERVICES_L1_TEST)
        find_library(IARMBUS_LIB NAMES IARMBus PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu PATH_SUFFIXES lib lib64)
        find_library(TVSETTINGS_HAL_LIB NAMES tvsettings-hal PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu PATH_SUFFIXES lib lib64)
        find_library(TR181API_LIB NAMES tr181api PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu PATH_SUFFIXES lib lib64)
        find_library(DS_LIB NAMES ds PATHS ${CMAKE_PREFIX_PATH} /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu PATH_SUFFIXES lib lib64)
        foreach(REQUIRED_TV_LIBRARY IARMBUS_LIB TVSETTINGS_HAL_LIB TR181API_LIB DS_LIB)
            if (NOT ${REQUIRED_TV_LIBRARY})
                message(FATAL_ERROR "Required TV library ${REQUIRED_TV_LIBRARY} was not found")
            endif()
        endforeach()
    endif()
endif()

if (AVOUTPUT_TV)
    add_library(${MODULE_NAME} SHARED
        AVOutputBase.cpp
        AVOutputTV.cpp
        AVOutputTVHelper.cpp
        AVOutput.cpp
        Module.cpp)
    if (RDK_SERVICES_L1_TEST)
        target_sources(${MODULE_NAME} PRIVATE ../Tests/L1Tests/tvSettingsUnsupported.cpp)
    endif()
    target_compile_definitions(${MODULE_NAME}
        PRIVATE
            AVOUTPUT_TV=1
            DEVICE_TYPE=AVOutputTV)
    target_link_libraries(${MODULE_NAME}
        PRIVATE
            Boost::filesystem
            glib-2.0
            pthread
    )
    if (NOT RDK_SERVICES_L1_TEST)
        target_link_libraries(${MODULE_NAME} PRIVATE ${IARMBUS_LIB})
        target_link_libraries(${MODULE_NAME} PRIVATE ${TVSETTINGS_HAL_LIB})
        target_link_libraries(${MODULE_NAME} PRIVATE ${TR181API_LIB})
        target_link_libraries(${MODULE_NAME} PRIVATE ${DS_LIB})
    endif()
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

find_package(DS)

target_include_directories(${MODULE_NAME} PRIVATE ${DS_INCLUDE_DIRS})
target_include_directories(${MODULE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../helpers)
target_link_libraries(${MODULE_NAME} 
    PRIVATE
        ${NAMESPACE}Plugins::${NAMESPACE}Plugins)


install(TARGETS ${MODULE_NAME} 
    DESTINATION lib/wpeframework/plugins)

write_config(${PLUGIN_NAME})
