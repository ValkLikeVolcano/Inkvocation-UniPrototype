// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UnistrokeTemplate.h"
#include "UnistrokeResult.h"
#include "UnistrokeRecognizer.generated.h"


struct FGameplayTag;
static const int NumTemplates = 16;
static const float Diagonal = FMath::Sqrt(2 * FMath::Square(SquareSize));
static const float HalfDiagonal = 0.5 * Diagonal;
static const float AngleRange = FMath::DegreesToRadians(45.0);
static const float AnglePrecision = FMath::DegreesToRadians(2.0);


USTRUCT()
struct CGSI_GAMEPROTOTYPE_API FUnistrokeRecognizer
{
	GENERATED_BODY()

public:
	FUnistrokeRecognizer();
	~FUnistrokeRecognizer();
	FUnistrokeResult Recognize(const TArray<FVector2D> &VectorPoints, const bool &UseProtractor);
	void AddTemplate(const FGameplayTag& ShapeTag, const TArray<FVector2D> &VectorPoints);
	void Reset();

public:
	TArray<FUnistrokeTemplate> Templates;
};
