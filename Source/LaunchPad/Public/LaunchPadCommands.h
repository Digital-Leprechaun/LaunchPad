// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "LaunchPadStyle.h"

class FLaunchPadCommands : public TCommands<FLaunchPadCommands>
{
public:

	FLaunchPadCommands()
		: TCommands<FLaunchPadCommands>(TEXT("LaunchPad"), NSLOCTEXT("Contexts", "LaunchPad", "LaunchPad Plugin"), NAME_None, FLaunchPadStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
