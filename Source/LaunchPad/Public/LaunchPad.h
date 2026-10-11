// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
class FUICommandList;
class FLaunchPadModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	void RegisterMenus();
	void PluginButtonClicked();
	bool CanExecutePlayAction() const;
	FText GetActionLabel() const;
	FText GetActionToolTip() const;
	TSharedPtr<FUICommandList> PluginCommands;
};
