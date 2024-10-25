// // Copyright (c) 2024 Nic. All Rights Reserved.


#include "SpellsPaintWidget.h"

#include "CGSI_GamePrototype/SpellsGameplayTags.h"
#include "CGSI_GamePrototype/Spells/Paint/UnistrokePoint.h"
#include "CGSI_GamePrototype/Spells/Paint/UnistrokeRectangle.h"


int32 USpellsPaintWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	for (int32 i = 0; i < LineSegments.Num(); ++i)
	{
		const FLineSegment& Segment = LineSegments[i];
		const float TimeElapsed = CurrentTime - Segment.TimeDrawn;

		// Calculate gradient color and alpha
		const float Alpha = 1.0f - FMath::Clamp(TimeElapsed / LineDuration, 0.0f, 1.0f);
		FLinearColor LineColor = FLinearColor::LerpUsingHSV(StartColor, EndColor, FMath::Clamp(TimeElapsed / LineDuration, 0.0f, 1.0f));
		LineColor.A = bFadeOut ? Alpha : 1.0f;
		
		const float Thickness = FMath::Max(EndThickness, StartThickness - TimeElapsed / LineDuration * (StartThickness - EndThickness));
		
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			{ Segment.StartPoint, Segment.EndPoint },
			ESlateDrawEffect::None,
			LineColor,
			true,
			Thickness
		);
	}

	return LayerId + 1;
}

void USpellsPaintWidget::AddPoint(const FVector2D& Point)
{
	if (Points.Num() > 0) 
	{
		FLineSegment Segment;
		Segment.StartPoint = Points.Last();
		Segment.EndPoint = Point;
		Segment.TimeDrawn = GetWorld()->GetTimeSeconds();
		LineSegments.Add(Segment);
	}
	Points.Add(Point);
}

void USpellsPaintWidget::RemoveAllPoints()
{
	Points.Empty();
	LineSegments.Empty();
}

void USpellsPaintWidget::AnimateShape(const FGameplayTag& ShapeTag)
{

	if(ShapeTag.MatchesTagExact(SpellTags::Shape_NoMatch) || ShapeTag.MatchesTagExact(SpellTags::Shape_NotEnoughPoints))
	{
		// Play failed Animation
	}
	else if(ShapeTag.MatchesTagExact(SpellTags::Shape_Circle))
	{
	}
	else if(ShapeTag.MatchesTagExact(SpellTags::Shape_Square))
	{
	}

	RemoveAllPoints();
}
