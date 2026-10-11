// Copyright Epic Games, Inc. All Rights Reserved.

#include "LaunchPadCommands.h"

#define LOCTEXT_NAMESPACE "FLaunchPadModule"

void FLaunchPadCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "LaunchPad", "Play in separate windows arranged by LaunchPad.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
