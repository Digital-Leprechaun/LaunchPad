// Copyright Epic Games, Inc. All Rights Reserved.
#include "LaunchPad.h"
#include "LaunchPadCommands.h"
#include "LaunchPadLayout.h"
#include "LaunchPadStyle.h"
#include "Editor.h"
#include "Editor/UnrealEdEngine.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/UICommandList.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/MessageDialog.h"
#include "PlayInEditorDataTypes.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "ToolMenus.h"
#include "UObject/UObjectGlobals.h"
#include "UnrealEdGlobals.h"
#include "Widgets/SWindow.h"

#define LOCTEXT_NAMESPACE "FLaunchPadModule"

namespace
{
	FIntRect ToPixelRect(const FSlateRect& Rect)
	{
		return FIntRect(FMath::CeilToInt(Rect.Left), FMath::CeilToInt(Rect.Top),
			FMath::FloorToInt(Rect.Right), FMath::FloorToInt(Rect.Bottom));
	}
}

void FLaunchPadModule::StartupModule()
{
	FLaunchPadStyle::Initialize();
	FLaunchPadCommands::Register();
	PluginCommands = MakeShared<FUICommandList>();
	PluginCommands->MapAction(FLaunchPadCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FLaunchPadModule::PluginButtonClicked),
		FCanExecuteAction::CreateRaw(this, &FLaunchPadModule::CanExecutePlayAction));
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLaunchPadModule::RegisterMenus));
}

void FLaunchPadModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	PluginCommands.Reset();
	FLaunchPadCommands::Unregister();
	FLaunchPadStyle::Shutdown();
}

bool FLaunchPadModule::CanExecutePlayAction() const
{
	return GEditor && GUnrealEd && FSlateApplication::IsInitialized() &&
		!GEditor->IsPlaySessionRequestQueued();
}

FText FLaunchPadModule::GetActionLabel() const
{
	return GEditor && GEditor->PlayWorld
		? LOCTEXT("StopLabel", "Stop") : LOCTEXT("LaunchLabel", "LaunchPad");
}

FText FLaunchPadModule::GetActionToolTip() const
{
	return GEditor && GEditor->PlayWorld
		? LOCTEXT("StopToolTip", "Stop the current play session.")
		: LOCTEXT("LaunchToolTip", "Play in separate windows arranged by LaunchPad.");
}

void FLaunchPadModule::PluginButtonClicked()
{
	if (!CanExecutePlayAction())
	{
		return;
	}
	if (GEditor->PlayWorld)
	{
		GUnrealEd->RequestEndPlayMap();
		return;
	}
	FSlateApplication& SlateApp = FSlateApplication::Get();
	const TSharedPtr<SWindow> EditorWindow = SlateApp.GetActiveTopLevelRegularWindow();
	FSlateRect WorkArea = SlateApp.GetPreferredWorkArea();
	FSlateRect LayoutArea = WorkArea;
	if (EditorWindow.IsValid())
	{
		const FVector2D Position = EditorWindow->GetPositionInScreen();
		const FVector2D Size = EditorWindow->GetSizeInScreen();
		const FSlateRect WindowRect(Position.X, Position.Y, Position.X + Size.X, Position.Y + Size.Y);
		WorkArea = SlateApp.GetWorkArea(WindowRect);
		LayoutArea = FSlateRect(FMath::Max(WindowRect.Left, WorkArea.Left),
			FMath::Max(WindowRect.Top, WorkArea.Top), FMath::Min(WindowRect.Right, WorkArea.Right),
			FMath::Min(WindowRect.Bottom, WorkArea.Bottom));
		if (LayoutArea.Right <= LayoutArea.Left || LayoutArea.Bottom <= LayoutArea.Top)
		{
			LayoutArea = WorkArea;
		}
	}
	// Match Unreal's regular PIE window style and query decorations at this monitor's DPI.
	const FVector2D Center((LayoutArea.Left + LayoutArea.Right) * 0.5,
		(LayoutArea.Top + LayoutArea.Bottom) * 0.5);
	const float DPIScale = FPlatformApplicationMisc::GetDPIScaleFactorAtPoint(Center.X, Center.Y);
	const TSharedRef<SWindow> SizingWindow = SNew(SWindow)
		.UseOSWindowBorder(false).AdjustInitialSizeAndPositionForDPIScale(false);
	const FVector2D OuterSize = SizingWindow->GetWindowSizeFromClientSize(FVector2D::ZeroVector, DPIScale);
	const FIntPoint Decorations(FMath::CeilToInt(OuterSize.X), FMath::CeilToInt(OuterSize.Y));

	ULevelEditorPlaySettings* CustomPlaySettings = DuplicateObject<ULevelEditorPlaySettings>(
		GetDefault<ULevelEditorPlaySettings>(), GetTransientPackage());
	const FIntPoint DesiredSize = LaunchPad::ResolveDesiredSize(
		FIntPoint(CustomPlaySettings->NewWindowWidth, CustomPlaySettings->NewWindowHeight),
		ToPixelRect(WorkArea).Size());
	int32 ClientCount = 1;
	CustomPlaySettings->GetPlayNumberOfClients(ClientCount);
	ClientCount = FMath::Max(1, ClientCount);
	// Leave a small margin around the grid, preserving its screen-space origin.
	const float InsetX = (LayoutArea.Right - LayoutArea.Left) * 0.025f;
	const float InsetY = (LayoutArea.Bottom - LayoutArea.Top) * 0.025f;
	const FSlateRect SafeArea(LayoutArea.Left + InsetX, LayoutArea.Top + InsetY,
		LayoutArea.Right - InsetX, LayoutArea.Bottom - InsetY);
	FIntPoint ClientSize;
	TArray<FIntPoint> Positions;
	if (!LaunchPad::CalculateLayout(ToPixelRect(SafeArea), DesiredSize, Decorations, ClientCount, ClientSize, Positions)
		&& !LaunchPad::CalculateLayout(ToPixelRect(WorkArea), DesiredSize, Decorations, ClientCount, ClientSize, Positions))
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("LayoutFailed",
			"LaunchPad could not fit the requested play windows. Reduce the number of players and try again."));
		return;
	}
	CustomPlaySettings->MultipleInstancePositions = MoveTemp(Positions);
	CustomPlaySettings->CenterNewWindow = false;
	CustomPlaySettings->NewWindowWidth = ClientSize.X;
	CustomPlaySettings->NewWindowHeight = ClientSize.Y;
	CustomPlaySettings->SetClientWindowSize(ClientSize);
	CustomPlaySettings->NewWindowPosition = CustomPlaySettings->MultipleInstancePositions[0];
	FRequestPlaySessionParams SessionParams;
	SessionParams.EditorPlaySettings = CustomPlaySettings;
	// RequestPlaySession duplicates and retains these settings before returning.
	GUnrealEd->RequestPlaySession(SessionParams);
}

void FLaunchPadModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	const TAttribute<FText> Label = TAttribute<FText>::Create(
		TAttribute<FText>::FGetter::CreateRaw(this, &FLaunchPadModule::GetActionLabel));
	const TAttribute<FText> ToolTip = TAttribute<FText>::Create(
		TAttribute<FText>::FGetter::CreateRaw(this, &FLaunchPadModule::GetActionToolTip));
	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
	Menu->FindOrAddSection("WindowLayout").AddMenuEntryWithCommandList(
		FLaunchPadCommands::Get().PluginAction, PluginCommands, Label, ToolTip);
	UToolMenu* Toolbar = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
	FToolMenuEntry& Entry = Toolbar->FindOrAddSection("Play").AddEntry(
		FToolMenuEntry::InitToolBarButton(FLaunchPadCommands::Get().PluginAction, Label, ToolTip));
	Entry.SetCommandList(PluginCommands);
}

#undef LOCTEXT_NAMESPACE
IMPLEMENT_MODULE(FLaunchPadModule, LaunchPad)
