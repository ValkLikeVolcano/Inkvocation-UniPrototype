// // Copyright (c) 2024 Nic. All Rights Reserved.

#include "UnistrokeTemplate.h"

FUnistrokeTemplate::FUnistrokeTemplate()
{
	ShapeTag = FGameplayTag();
	Vector = TArray<float>();
	Points = TArray<FUnistrokePoint>();
}

FUnistrokeTemplate::FUnistrokeTemplate(const FGameplayTag& ShapeTag, const TArray<FUnistrokePoint> &Points)
{
	this->ShapeTag = ShapeTag;
	this->Points = Points;

	FUnistrokePoint::Resample(this->Points, NumPoints);

	float Radians = FUnistrokePoint::IndicativeAngle(Points);

	FUnistrokePoint::RotateBy(this->Points, -Radians);

	FUnistrokePoint::ScaleTo(this->Points, SquareSize);

	FUnistrokePoint::TranslateTo(this->Points, FUnistrokePoint());

	this->Vector = FUnistrokePoint::Vectorize(this->Points);
}

FUnistrokeTemplate::~FUnistrokeTemplate()
{
}
