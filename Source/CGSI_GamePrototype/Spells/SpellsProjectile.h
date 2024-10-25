// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellsProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class CGSI_GAMEPROTOTYPE_API ASpellsProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASpellsProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

protected:
	virtual void BeginPlay() override;

};
