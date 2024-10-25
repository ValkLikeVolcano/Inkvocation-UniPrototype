// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SpellsAbilitySystemComponent.generated.h"

struct FUnistrokeResult;
/**
 * 
 */
UCLASS()
class CGSI_GAMEPROTOTYPE_API USpellsAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:

	void ShapeRecognized(const FUnistrokeResult& Result);
	
	bool bAllowMultipleShapes = true;
};
