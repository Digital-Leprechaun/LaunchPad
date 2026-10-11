# Changelog

## 26.10.10.1

- Calculate the layout once per launch with a single grid-fitting algorithm.
- Fix single-player fitting in narrow editor windows.
- Resolve automatic dimensions before calculating aspect ratios.
- Preserve client aspect ratios and keep window decorations separate from client sizes.
- Cap fitted windows at the requested resolution.
- Preserve monitor coordinates and constrain grids to the monitor work area.
- Query Slate decoration sizes at the chosen monitor's DPI.
- Validate window and editor state, and report layouts that cannot fit.
- Duplicate play settings locally instead of storing an untracked UObject pointer on the module.
- Show Stop text during PIE and prevent duplicate queued launch requests.
- Remove unused template code and compile without module PCHs or unity files.
- Add layout regression coverage, repository documentation and repeatable release packaging.

Validated with the local UE.AssetPack Unreal Engine 5.8.2 source build on Win64, Development and DebugGame. Automated layout checks passed; interactive PIE validation remains a manual check.
