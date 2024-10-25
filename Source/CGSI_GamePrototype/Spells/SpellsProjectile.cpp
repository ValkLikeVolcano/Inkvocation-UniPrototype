// Copyright (c) 2024 Nic. All Rights Reserved.


#include "SpellsProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"


ASpellsProjectile::ASpellsProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->ProjectileGravityScale = 0.05f;
}

void ASpellsProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}
