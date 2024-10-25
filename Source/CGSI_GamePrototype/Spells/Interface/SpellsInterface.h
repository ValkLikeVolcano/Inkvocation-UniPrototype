// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CGSI_GamePrototype/Spells/Paint/UnistrokeResult.h"
#include "UObject/Interface.h"
#include "SpellsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class USpellsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CGSI_GAMEPROTOTYPE_API ISpellsInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual FVector GetSpawnLocation();
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual FRotator GetSpawnRotation();
};
