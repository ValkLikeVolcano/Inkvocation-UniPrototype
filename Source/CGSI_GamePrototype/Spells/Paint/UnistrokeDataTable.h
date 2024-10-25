// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "UnistrokeDataTable.generated.h"


USTRUCT()
struct CGSI_GAMEPROTOTYPE_API FUnistrokeDataTable : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag ShapeTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector2D> Points;

};
