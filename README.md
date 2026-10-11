# LaunchPad

LaunchPad is an Unreal Engine editor plugin that starts Play In Editor (PIE) in separate windows and arranges them in a grid over the editor window. Click the same button during PIE to stop the session.

**Version: 26.10.10.1**

## Features

- Fits one or more player windows while preserving their requested aspect ratio, subject to whole-pixel rounding.
- Shrinks windows when needed and never enlarges them beyond the requested resolution.
- Resolves automatic window dimensions before calculating the grid.
- Accounts for Slate window decorations and monitor DPI.
- Preserves screen coordinates on secondary monitors, including negative coordinates.
- Uses the monitor work area if the editor area cannot fit the grid.
- Shows a **Stop** label and tooltip while PIE runs.

## Compatibility

Validated on Windows with the local **UE.AssetPack Unreal Engine 5.8.2 source build**, in Development and DebugGame editor configurations. Other engine versions and operating systems have not been tested.

The Windows binary archive is built for that specific engine build. Its DLLs are not guaranteed to load in an Epic Games Launcher installation or another custom build, even with the same engine version. Use the source archive and rebuild for your engine in that case.

LaunchPad is editor-only and does not add a runtime module to packaged games.

## Installation

1. Close the Unreal Editor.
2. Extract the archive's `LaunchPad` folder into your project's `Plugins` directory, so the descriptor is at `YourProject/Plugins/LaunchPad/LaunchPad.uplugin`.
3. For a source installation, generate project files and build your project's editor target with your engine. A Blueprint-only project may need a C++ target to build the plugin.
4. Open the project, enable **LaunchPad** in the Plugins dialog, and restart if prompted.

When upgrading, replace the existing plugin folder rather than merging old binaries into a source-only installation.

## Usage

Set **Number of Players** and the **New Editor Window** resolution in Unreal's Play settings, then click **LaunchPad** beside the Play controls. The command is also available from the **Window** menu.

Setting either resolution dimension to zero uses Unreal's automatic-resolution behavior: both dimensions resolve to 75% of the chosen monitor's work area before the grid is fitted. Additional player windows use the same fitted client size. Network mode and other play settings come from your existing editor settings.

If the grid cannot fit in the editor area, LaunchPad tries the monitor work area. If neither area can fit positive-sized windows, it reports the problem without starting PIE.

## Validation

Both Win64 editor configurations build with LaunchPad's precompiled headers and unity compilation disabled. `LaunchPad.Layout.Regression` passed in an isolated Unreal Editor host project. It covers automatic resolution, single-client fitting, size limits, invalid inputs, and 64 combinations of player counts, screen bounds and window decorations, checking bounds, aspect ratio and overlap.

To run it in your editor, open **Tools > Test Automation**, find `LaunchPad.Layout.Regression`, and run it. Menu placement may vary by engine version.

Useful manual checks include a narrow editor window, two players at 640x360 on a large display, three players at 1280x720, automatic dimensions, secondary monitors with different scaling, and the Stop button.

## Release preparation

Run `Scripts/Prepare-Release.ps1` in PowerShell to create a source archive, release notes and SHA-256 checksums under `Releases/26.10.10.1/`. Add `-EngineRoot <your-engine-directory>` to also package the local-engine Win64 binaries. Build binaries for the desired engine before packaging; the script packages existing binaries and does not compile them. Generated binaries and release archives are excluded from Git.

See [CHANGELOG.md](CHANGELOG.md) for this release's changes and [NOTICE](NOTICE) for attribution.

## License

MIT. See [LICENSE](LICENSE). Existing Epic template copyright notices are retained; see [NOTICE](NOTICE).
