# Contributing

If you would like to contribute code to this project you can do so through GitHub by forking the repository and sending a pull request. Before RDK accepts your code into the project you must sign the RDK Contributor License Agreement (CLA).

## API Documentation Maintenance

Changes to AVOutput JSON-RPC API surface must update documentation in the same pull request.

- API reference: `docs/api/AVOutputPlugin.md`
- Maintenance workflow and checklist: `docs/api/AVOutputDocMaintenance.md`

This applies to:

- Method registration changes (`registerMethod(...)` in `plugin/AVOutputTV.cpp`)
- Notification emission changes (`sendNotify(...)` in `plugin/AVOutputTV.cpp` and `plugin/AVOutputTVHelper.cpp`)
