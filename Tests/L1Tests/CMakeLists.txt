cmake_minimum_required(VERSION 3.8)
set(PLUGIN_NAME L1TestsSU)
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(${NAMESPACE}Plugins REQUIRED)

set (TEST_SRC
    tests/test_UtilsFile.cpp
)

set (TEST_LIB
    ${NAMESPACE}Plugins::${NAMESPACE}Plugins
)

set (TEST_INC ../../helpers)

#########################################################################################
# Macro definitions remain unchanged
#########################################################################################

macro(add_plugin_test_ex plugin_opt plugin_test_sources_str plugin_includes_str plugin_libs_str)
    if(${plugin_opt})
        message(STATUS "${plugin_opt}=ON")

        string(REPLACE ";" ";" srclist "${plugin_test_sources_str}")
        string(REPLACE ";" ";" inclist "${plugin_includes_str}")
        string(REPLACE ";" ";" liblist "${plugin_libs_str}")

        foreach(item IN LISTS srclist)
            list(APPEND TEST_SRC ${item})
        endforeach()

        foreach(item IN LISTS inclist)
            list(APPEND TEST_INC ${item})
        endforeach()

        foreach(item IN LISTS liblist)
            list(APPEND TEST_LIB ${item})
        endforeach()
    else()
        message(STATUS "${plugin_opt}=OFF")
    endif()
endmacro()

macro(add_plugin_test plugin_name test_files)
    string(TOUPPER "${plugin_name}" plugin_option)
    set(plugin_opt "PLUGIN_${plugin_option}")

    add_plugin_test_ex(${plugin_opt} "${test_files}" "../../${plugin_name}" "${NAMESPACE}${plugin_name}")
endmacro()

#########################################################################################
# PLUGIN_AVOUTPUT  (Converted from FIRMWAREUPDATE)
#########################################################################################

set (AVOUTPUT_LIBS 
    ${NAMESPACE}AVOutput 
    ${NAMESPACE}AVOutputImplementation
)

set (AVOUTPUT_INC 
    ${CMAKE_SOURCE_DIR}/../entservices-avoutput/plugin
    ${CMAKE_SOURCE_DIR}/../entservices-avoutput/helpers
)

# Updated test file name to match AVOutput
add_plugin_test_ex(PLUGIN_AVOUTPUT 
    tests/test_AVOutput.cpp 
    "${AVOUTPUT_INC}" 
    "${AVOUTPUT_LIBS}"
)

#########################################################################################

add_library(${MODULE_NAME} SHARED ${TEST_SRC})

include_directories(${TEST_INC})

target_link_directories(${MODULE_NAME} PUBLIC 
    ${CMAKE_INSTALL_PREFIX}/lib 
    ${CMAKE_INSTALL_PREFIX}/lib/wpeframework/plugins
)

target_link_libraries(${MODULE_NAME} ${TEST_LIB})

target_include_directories(${MODULE_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        ${CMAKE_SOURCE_DIR}/../entservices-testframework/Tests/mocks
        ${CMAKE_SOURCE_DIR}/../entservices-testframework/Tests/mocks/devicesettings
        ${CMAKE_SOURCE_DIR}/../entservices-testframework/Tests/mocks/thunder
        ${CMAKE_SOURCE_DIR}/../Thunder/Source/plugins
        )

install(TARGETS ${MODULE_NAME} DESTINATION lib)
write_config(${PLUGIN_NAME})
