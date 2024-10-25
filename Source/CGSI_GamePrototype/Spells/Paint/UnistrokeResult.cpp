// // Copyright (c) 2024 Nic. All Rights Reserved.


#include "UnistrokeResult.h"

#include "CGSI_GamePrototype/SpellsGameplayTags.h"

FUnistrokeResult::FUnistrokeResult()
{
	this->ShapeTag = SpellTags::Shape_NoMatch;
	this->Center = FVector2d();
	this->BoundingBox = FUnistrokeRectangle();
	this->Score = 0.0f;
	this->Time = 0.0f;
}

FUnistrokeResult::FUnistrokeResult(const FGameplayTag& ShapeTag, const FVector2D& Center, const FUnistrokeRectangle& BoundingBox, const float& Score, const float& Time)
{
	this->ShapeTag = ShapeTag;
	this->Center = Center;
	this->BoundingBox = BoundingBox;
	this->Score = Score;
	this->Time = Time;
}

FUnistrokeResult::~FUnistrokeResult()
{
}
