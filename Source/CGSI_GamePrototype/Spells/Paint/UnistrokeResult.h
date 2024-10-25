// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UnistrokeRectangle.h"
#include "UnistrokeResult.generated.h"

USTRUCT(BlueprintType)
struct CGSI_GAMEPROTOTYPE_API FUnistrokeResult
{
	GENERATED_BODY()

public:
	FUnistrokeResult();
	FUnistrokeResult(const FGameplayTag& ShapeTag, const FVector2D& Center, const FUnistrokeRectangle& BoundingBox, const float &Score, const float &Time);
	~FUnistrokeResult();

public:
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ShapeTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly)
	FVector2D Center = FVector2d();
	UPROPERTY(BlueprintReadOnly)
	FUnistrokeRectangle BoundingBox = FUnistrokeRectangle();
	UPROPERTY(BlueprintReadOnly)
	float Score;
	UPROPERTY(BlueprintReadOnly)
	float Time;
};
