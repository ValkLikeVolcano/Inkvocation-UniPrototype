// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpellsGameplayAbility.h"
#include "ShapeGameplayAbility.generated.h"

class ASpellsProjectile;
struct FUnistrokeResult;
/**
 * 
 */
UCLASS()
class CGSI_GAMEPROTOTYPE_API UShapeGameplayAbility : public USpellsGameplayAbility
{
	GENERATED_BODY()
protected:
	UShapeGameplayAbility();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category="GameplayAbility|Projectile")
	void SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ASpellsProjectile> ProjectileClass;
	
};
