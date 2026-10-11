// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;
public class LaunchPad : ModuleRules
{
	public LaunchPad(ReadOnlyTargetRules Target) : base(Target)
	{
		// This small module builds independently so missing includes cannot hide in a PCH or unity file.
		PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
		bUseUnity = false;
		PublicDependencyModuleNames.AddRange(new[] { "Core", "Slate", "SlateCore" });
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Projects", "InputCore", "UnrealEd", "ToolMenus", "CoreUObject", "Engine", "ApplicationCore"
		});
	}
}
