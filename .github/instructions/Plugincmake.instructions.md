---
applyTo: "**/CMakeLists.txt"
---

### Namespace Usage

### Requirement

All CMake targets, install paths, export sets, and framework target references must use the ${NAMESPACE} variable instead of hardcoded framework names. If the repository still carries a compatibility `find_package(WPEFramework)` call, keep all target naming and link references namespace-based.

### Correct Example

```cmake
set(MODULE_NAME ${NAMESPACE}${PLUGIN_NAME})

find_package(WPEFramework)
find_package(${NAMESPACE}Plugins REQUIRED)

target_link_libraries(${MODULE_NAME}
    PRIVATE
    ${NAMESPACE}Plugins::${NAMESPACE}Plugins
)
```

### Incorrect Example

```cmake
set(MODULE_NAME WPEFramework${PLUGIN_NAME})

find_package(WPEFrameworkPlugins REQUIRED)

target_link_libraries(${MODULE_NAME}
    PRIVATE
    WPEFrameworkPlugins::WPEFrameworkPlugins
)
```
