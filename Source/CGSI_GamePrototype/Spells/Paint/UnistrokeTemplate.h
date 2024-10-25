// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnistrokePoint.h"
#include "UnistrokeTemplate.generated.h"

static const int NumPoints = 64;
static const float SquareSize = 250.0f;

USTRUCT()
struct CGSI_GAMEPROTOTYPE_API FUnistrokeTemplate
{
	GENERATED_USTRUCT_BODY()

public:
	FUnistrokeTemplate();
	FUnistrokeTemplate(const FGameplayTag& ShapeTag, const TArray<FUnistrokePoint> &Points);
	~FUnistrokeTemplate();

public:
	FGameplayTag ShapeTag;
	TArray<float> Vector;
	TArray<FUnistrokePoint> Points;
};
