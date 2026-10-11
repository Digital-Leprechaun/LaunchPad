// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
namespace LaunchPad
{
	FIntPoint ResolveDesiredSize(FIntPoint RequestedSize, FIntPoint WorkAreaSize);
	// All dimensions and returned positions are desktop pixels.
	bool CalculateLayout(const FIntRect& Bounds, FIntPoint DesiredSize, FIntPoint Decorations,
		int32 ClientCount, FIntPoint& OutClientSize, TArray<FIntPoint>& OutPositions);
}
