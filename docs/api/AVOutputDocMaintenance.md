# AVOutput API Documentation Maintenance

This document defines how to keep AVOutput API documentation aligned with implementation in this repository.

## Purpose

- Keep published API behavior synchronized with code.
- Make API contract changes reviewable in pull requests.
- Support open source compliance by documenting exposed interfaces and compatibility differences.

## Source of Truth

Use code as the source of truth:

- JSON-RPC methods: `registerMethod(...)` in `plugin/AVOutputTV.cpp`
- Notifications/events: `sendNotify(...)` in `plugin/AVOutputTV.cpp` and `plugin/AVOutputTVHelper.cpp`

## Mandatory Update Rule

If a pull request changes method registration or event emission, it must update API documentation in the same pull request.

Examples of code changes that require doc updates:

- Added, removed, or renamed `registerMethod(...)` entries
- Added, removed, or renamed `sendNotify(...)` events
- Method parameter/result behavior changes visible to API consumers

## PR Checklist (Required)

- [ ] Updated `docs/api/AVOutputPlugin.md` when API surface changed
- [ ] Verified name compatibility notes for renamed methods/events

## Recommended Verification Commands

Run from repository root:

```bash
# Registered methods in code
rg -o --replace '$1' 'registerMethod\("([^"]+)"' plugin/AVOutputTV.cpp | sort -u

# Emitted notifications in code
rg -h -o --replace '$1' 'sendNotify\("([^"]+)"' plugin/AVOutputTV.cpp plugin/AVOutputTVHelper.cpp | sort -u

# Documented method/event section headings
rg -o --replace '$1' '^## \*([A-Za-z0-9]+)\*$' docs/api/AVOutputPlugin.md | sort -u
```

## Open Source Guidance

- Keep compatibility notes explicit for method/event name changes.
- Prefer additive documentation when behavior differs by platform/profile.
- Do not silently remove upstream API sections.
- Ensure references to legal files remain accurate: `LICENSE`, `COPYING`, `NOTICE`.

## Related Documents

- `docs/api/AVOutputPlugin.md`
- `CONTRIBUTING.md`
