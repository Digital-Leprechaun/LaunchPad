// Copyright Epic Games, Inc. All Rights Reserved.
#include "LaunchPadLayout.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLaunchPadLayoutTest, "LaunchPad.Layout.Regression",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FLaunchPadLayoutTest::RunTest(const FString& Parameters)
{
	TestEqual(TEXT("Automatic resolution"), LaunchPad::ResolveDesiredSize(FIntPoint(0, 0),
		FIntPoint(1920, 1080)), FIntPoint(1440, 810));
	TestEqual(TEXT("One automatic dimension resolves both, as Unreal does"),
		LaunchPad::ResolveDesiredSize(FIntPoint(1280, 0), FIntPoint(1920, 1080)), FIntPoint(1440, 810));
	FIntPoint Size;
	TArray<FIntPoint> Positions;
	TestTrue(TEXT("Single client fits a narrow editor window"), LaunchPad::CalculateLayout(
		FIntRect(20, 30, 780, 1170), FIntPoint(1280, 720), FIntPoint(16, 45), 1, Size, Positions));
	TestTrue(TEXT("Single client has a positive size"), Size.X > 0 && Size.Y > 0);
	TestTrue(TEXT("Large desktop fits requested size"), LaunchPad::CalculateLayout(
		FIntRect(96, 54, 3744, 2106), FIntPoint(640, 360), FIntPoint(16, 45), 2, Size, Positions));
	TestEqual(TEXT("Large desktop does not enlarge clients"), Size, FIntPoint(640, 360));

	// Exercise portrait bounds, nonzero/negative monitor origins, and scaled decorations.
	for (const FIntRect Bounds : { FIntRect(48, 27, 1872, 1053), FIntRect(-1900, 40, -1100, 1240) })
	{
		for (const FIntPoint Decorations : { FIntPoint(16, 45), FIntPoint(32, 90) })
		{
			for (int32 Count = 1; Count <= 16; ++Count)
			{
				if (!TestTrue(TEXT("Grid fits"), LaunchPad::CalculateLayout(Bounds,
					FIntPoint(1280, 720), Decorations, Count, Size, Positions)))
				{
					continue;
				}
				TestEqual(TEXT("Every client has a position"), Positions.Num(), Count);
				TestTrue(TEXT("Client size stays within request"), Size.X <= 1280 && Size.Y <= 720);
				TestTrue(TEXT("Aspect ratio differs by less than one pixel of rounding"),
					FMath::Abs(double(Size.X) / 1280 - double(Size.Y) / 720) < 1.0 / 720);
				const FIntPoint OuterSize = Size + Decorations;
				for (int32 Index = 0; Index < Positions.Num(); ++Index)
				{
					const FIntPoint P = Positions[Index];
					TestTrue(TEXT("Outer window stays in screen bounds"),
						P.X >= Bounds.Min.X && P.Y >= Bounds.Min.Y &&
						P.X + OuterSize.X <= Bounds.Max.X && P.Y + OuterSize.Y <= Bounds.Max.Y);
					for (int32 Other = 0; Other < Index; ++Other)
					{
						const FIntPoint Q = Positions[Other];
						TestTrue(TEXT("Outer windows do not overlap"),
							P.X >= Q.X + OuterSize.X || Q.X >= P.X + OuterSize.X ||
							P.Y >= Q.Y + OuterSize.Y || Q.Y >= P.Y + OuterSize.Y);
					}
				}
			}
		}
	}
	TestFalse(TEXT("Impossible bounds are rejected"), LaunchPad::CalculateLayout(
		FIntRect(100, 100, 110, 110), FIntPoint(640, 360), FIntPoint(16, 45), 1, Size, Positions));
	TestEqual(TEXT("Failed layout clears positions"), Positions.Num(), 0);
	TestEqual(TEXT("Failed layout clears size"), Size, FIntPoint::ZeroValue);
	TestFalse(TEXT("Unresolved zero resolution is rejected"), LaunchPad::CalculateLayout(
		FIntRect(0, 0, 1920, 1080), FIntPoint(0, 720), FIntPoint(16, 45), 1, Size, Positions));
	return true;
}
#endif
