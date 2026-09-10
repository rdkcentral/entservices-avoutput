---
applyTo: "**/*.config,**/*.conf.in"
---

### Plugin Configuration

### Requirement

- Each plugin must define a <PluginName>.conf.in file with the plugin's base Thunder configuration. When the repository also ships a matching <PluginName>.config file, product-specific values such as autostart may be overridden there.

	- **autostart**: Indicates whether the plugin should start automatically when the framework boots. Set this in the configuration files to match the intended product behavior.

	- **callsign**: A unique identifier used to reference the plugin within the framework. Every callsign must start with the prefix `org.rdk`, followed by the AVOutput service name written in PascalCase. For example: `org.rdk.AVOutput`.

	- **Custom properties**: Any additional configuration parameters required by the plugin. These are passed during activation via PluginHost::IShell::ConfigLine(). The following structural configuration elements are commonly defined when the plugin needs them:
		- startuporder - Specifies the order in which plugins are started, relative to others.
		- precondition - If these preconditions aren't met, the plugin stays in the Preconditions state and activates automatically once they are satisfied. It is recommended to define the preconditions if the plugin depends on other subsystems being active.

### Example

AVOutput.conf.in:

```ini
precondition = ["Platform"]
callsign = "org.rdk.AVOutput"
autostart = "false"
startuporder = "@PLUGIN_AVOUTPUT_STARTUPORDER@"
```

AVOutput.config:

```cmake
set (autostart true)
set (callsign "org.rdk.AVOutput")
```
