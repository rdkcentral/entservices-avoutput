---
applyTo: "CMakeLists.txt"
---

## Requirement

### Coverity Scan Inclusion and Test Workflow Updates for New Plugins

When adding a new plugin in `CMakeLists.txt`, you **must** also update the following files to ensure the plugin is included in all required test and Coverity analysis workflows:

- **CI Workflow Files:**
  - `L1-tests.yml`
  - `L2-tests.yml`
- **Coverity Build Script:**
  - `cov_build.sh`

### Where Plugin Flags Are Defined

Use a dedicated CMake option named `-DPLUGIN_<PLUGIN_NAME>=ON` for each plugin.

- Define the plugin option in the repository `CMakeLists.txt` and wire it to `add_subdirectory(<PluginDir>)`.
- Pass the same `-DPLUGIN_<PLUGIN_NAME>=ON` flag in:
  - `.github/workflows/L1-tests.yml`
  - `.github/workflows/L2-tests.yml`
  - `cov_build.sh`

This ensures the plugin is consistently built in CI, local test workflows, and Coverity scans.

### Example

**Example:**

1. **CMake Plugin Registration Example**

```cmake
if (PLUGIN_AVOUTPUT)
    add_subdirectory(plugin)
endif()
if (PLUGIN_MY_NEW_PLUGIN)
    add_subdirectory(MyNewPlugin)
endif()
```

2. **Update Coverity Build Script**

```bash
cmake \
    -DPLUGIN_AVOUTPUT=ON \
    -DPLUGIN_MY_NEW_PLUGIN=ON \
    .
```

This ensures Coverity runs on your new plugin.

3. **Update Test Workflow YAMLs**

Ensure each test workflow references your new plugin using the **DPLUGIN_<PLUGINNAME>** CMake flag in their build or test step. For example, in `L1-tests.yml`:

```yaml
jobs:
  build-test:
    runs-on: ubuntu-22.04
    steps:
      - name: Configure with new plugin
        run: |
          cmake \
            -DPLUGIN_AVOUTPUT=ON \
            -DPLUGIN_MY_NEW_PLUGIN=ON \
            .
      - name: Run tests
        run: |
          ctest
```

Repeat similar additions in `L2-tests.yml`.

**Summary:**
Whenever a new plugin is registered via `CMakeLists.txt`, always update:
- `cov_build.sh` to add the plugin flag to the Coverity scan build step
- All test CI workflows in this repository, currently `L1-tests.yml` and `L2-tests.yml`, to include your plugin flag so the plugin is built and validated in CI
