// Copyright (c) 2024 Nic. All Rights Reserved.


#include "UnistrokeRecognizer.h"

#include "CGSI_GamePrototype/SpellsGameplayTags.h"

FUnistrokeRecognizer::FUnistrokeRecognizer()
{
}

FUnistrokeRecognizer::~FUnistrokeRecognizer()
{
}

FUnistrokeResult FUnistrokeRecognizer::Recognize(const TArray<FVector2D>& VectorPoints, const bool& UseProtractor)
{
	TArray<FUnistrokePoint> Points = FUnistrokePoint::Convert(VectorPoints);

	if (Points.Num() < 2 || FUnistrokePoint::PathLength(Points) < 100.0f) return FUnistrokeResult(SpellTags::Shape_NotEnoughPoints, FVector2d(), FUnistrokeRectangle(), 0.0, 0.0);

	const FUnistrokeTemplate Candidate = FUnistrokeTemplate(FGameplayTag(), Points);

	int TemplateIndex = -1;
	float BestDistance = TNumericLimits<float>::Max();

	const FDateTime StartTime = FDateTime::Now();
	
	for (int i = 0; i < Templates.Num(); i++)
	{

		const float distance = UseProtractor ? FUnistrokePoint::OptimalCosineDistance(Templates[i].Vector, Candidate.Vector) :
			FUnistrokePoint::DistanceAtBestAngle(Candidate.Points, Templates[i].Points, -AngleRange, AngleRange, AnglePrecision);

		if (distance < BestDistance)
		{
			BestDistance = distance;
			TemplateIndex = i;
		}
	}

	const FDateTime EndTime = FDateTime::Now();
	const float ElapsedTime = (EndTime.GetMillisecond() - StartTime.GetMillisecond()) / 1000;
	const float Score = UseProtractor ? (1.0 - BestDistance) : (1.0 - BestDistance / HalfDiagonal);
	const FUnistrokePoint Center = FUnistrokePoint::Centroid(Points);
	const FVector2D Center2D(Center.X, Center.Y);
	const FUnistrokeRectangle Bounds = FUnistrokePoint::BoundingRect(Points);

	return (TemplateIndex == -1) ? FUnistrokeResult(SpellTags::Shape_NoMatch, FVector2d(), FUnistrokeRectangle(), 0.0, ElapsedTime) : FUnistrokeResult(Templates[TemplateIndex].ShapeTag, Center2D, Bounds, Score, ElapsedTime);
}

void FUnistrokeRecognizer::AddTemplate(const FGameplayTag& ShapeTag, const TArray<FVector2D>& VectorPoints)
{
	const TArray<FUnistrokePoint> Points = FUnistrokePoint::Convert(VectorPoints);

	Templates.Add(FUnistrokeTemplate(ShapeTag, Points));
}

void FUnistrokeRecognizer::Reset()
{
	Templates.SetNum(NumTemplates);
}
