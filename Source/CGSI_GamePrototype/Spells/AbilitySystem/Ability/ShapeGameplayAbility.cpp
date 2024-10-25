// Copyright (c) 2024 Nic. All Rights Reserved.


#include "ShapeGameplayAbility.h"

#include "CGSI_GamePrototype/SpellsGameplayTags.h"
#include "CGSI_GamePrototype/Spells/SpellsProjectile.h"
#include "CGSI_GamePrototype/Spells/AbilitySystem/SpellsAbilitySystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


UShapeGameplayAbility::UShapeGameplayAbility()
{
	AbilityTags.AddTag(SpellTags::Ability_Type_Shape);
}

void UShapeGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UShapeGameplayAbility::SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Projectile"));

	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());

	ASpellsProjectile* Projectile = GetWorld()->SpawnActorDeferred<ASpellsProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetAvatarActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);


	Projectile->FinishSpawning(SpawnTransform);
}
