LaunchPad 26.10.10.1

Built with the local UE.AssetPack engine (Unreal Engine 5.8.2), Win64 Development and DebugGame.

Installation: close the TwaZGame editor, copy this entire LaunchPad folder into
TwaZGame/Plugins/LaunchPad, enable LaunchPad in the Plugins dialog, and restart.

Suggested manual checks:
- One player with a narrow editor window: a positive-size window should fit.
- Two players at 640x360 on a large display: windows should remain 640x360.
- Three players at 1280x720: windows should fit without overlap and retain the aspect ratio.
- Set either new-window dimension to zero: automatic resolution should work.
- Move the editor to another monitor, including one with different display scaling.
- During PIE the button should read Stop and end the session when clicked.

Automated validation: LaunchPad.Layout.Regression passed in the isolated host project.
It checks automatic resolution, single-client fitting, size caps, 64 combinations
of client counts/screen bounds/decorations, non-overlap, and invalid inputs.
