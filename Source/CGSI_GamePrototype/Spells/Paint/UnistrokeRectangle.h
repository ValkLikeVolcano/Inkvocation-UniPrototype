// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnistrokeRectangle.generated.h"

USTRUCT(BlueprintType)
struct CGSI_GAMEPROTOTYPE_API FUnistrokeRectangle
{
	GENERATED_USTRUCT_BODY()

public:
	FUnistrokeRectangle();
	FUnistrokeRectangle(const float& X, const float& Y, const float& Width, const float& Height);
	~FUnistrokeRectangle();

public:
	UPROPERTY(BlueprintReadOnly)
	float X;
	UPROPERTY(BlueprintReadOnly)
	float Y;
	UPROPERTY(BlueprintReadOnly)
	float Width;
	UPROPERTY(BlueprintReadOnly)
	float Height;
};
