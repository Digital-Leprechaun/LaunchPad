// Copyright Epic Games, Inc. All Rights Reserved.

#include "LaunchPadStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FLaunchPadStyle::StyleInstance = nullptr;

void FLaunchPadStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FLaunchPadStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FLaunchPadStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("LaunchPadStyle"));
	return StyleSetName;
}


const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef< FSlateStyleSet > FLaunchPadStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("LaunchPadStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("LaunchPad")->GetBaseDir() / TEXT("Resources"));

	Style->Set("LaunchPad.PluginAction", new IMAGE_BRUSH_SVG(TEXT("ButtonIcon"), Icon40x40));
	return Style;
}

const ISlateStyle& FLaunchPadStyle::Get()
{
	return *StyleInstance;
}
