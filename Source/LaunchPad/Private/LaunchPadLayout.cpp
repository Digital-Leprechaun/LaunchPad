// Copyright Epic Games, Inc. All Rights Reserved.
#include "LaunchPadLayout.h"

FIntPoint LaunchPad::ResolveDesiredSize(FIntPoint RequestedSize, FIntPoint WorkAreaSize)
{
	if (RequestedSize.X <= 0 || RequestedSize.Y <= 0)
	{
		return FIntPoint(FMath::Max(1, FMath::RoundToInt(WorkAreaSize.X * 0.75)),
			FMath::Max(1, FMath::RoundToInt(WorkAreaSize.Y * 0.75)));
	}
	return RequestedSize;
}

bool LaunchPad::CalculateLayout(const FIntRect& Bounds, FIntPoint DesiredSize, FIntPoint Decorations,
	int32 ClientCount, FIntPoint& OutClientSize, TArray<FIntPoint>& OutPositions)
{
	OutClientSize = FIntPoint::ZeroValue;
	OutPositions.Reset();
	if (ClientCount <= 0 || DesiredSize.X <= 0 || DesiredSize.Y <= 0 ||
		Bounds.Width() <= 0 || Bounds.Height() <= 0 || Decorations.X < 0 || Decorations.Y < 0)
	{
		return false;
	}
	int64 BestArea = 0;
	int32 BestColumns = 0;
	int32 BestRows = 0;
	for (int32 Columns = 1; Columns <= ClientCount; ++Columns)
	{
		const int32 Rows = FMath::DivideAndRoundUp(ClientCount, Columns);
		const int32 AvailableWidth = Bounds.Width() / Columns - Decorations.X;
		const int32 AvailableHeight = Bounds.Height() / Rows - Decorations.Y;
		if (AvailableWidth <= 0 || AvailableHeight <= 0)
		{
			continue;
		}
		const double Scale = FMath::Min(1.0, FMath::Min(
			double(AvailableWidth) / DesiredSize.X, double(AvailableHeight) / DesiredSize.Y));
		const FIntPoint Size(FMath::FloorToInt(DesiredSize.X * Scale),
			FMath::FloorToInt(DesiredSize.Y * Scale));
		const int64 Area = int64(Size.X) * Size.Y;
		if (Size.X > 0 && Size.Y > 0 && Area > BestArea)
		{
			BestArea = Area;
			BestColumns = Columns;
			BestRows = Rows;
			OutClientSize = Size;
		}
	}
	if (BestColumns == 0)
	{
		return false;
	}
	const FIntPoint CellSize = OutClientSize + Decorations;
	const FIntPoint GridSize(CellSize.X * BestColumns, CellSize.Y * BestRows);
	const FIntPoint Origin = Bounds.Min + (Bounds.Size() - GridSize) / 2;
	OutPositions.Reserve(ClientCount);
	for (int32 Index = 0; Index < ClientCount; ++Index)
	{
		OutPositions.Add(Origin + FIntPoint((Index % BestColumns) * CellSize.X,
			(Index / BestColumns) * CellSize.Y));
	}
	return true;
}
