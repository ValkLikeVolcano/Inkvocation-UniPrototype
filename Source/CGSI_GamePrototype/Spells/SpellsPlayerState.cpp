// Copyright (c) 2024 Nic. All Rights Reserved.


#include "SpellsPlayerState.h"

#include "AbilitySystem/SpellsAbilitySystemComponent.h"
#include "AbilitySystem/SpellsAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SpellsPlayerState)

ASpellsPlayerState::ASpellsPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<USpellsAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed); // Mixed for Players, Minimal for Other

	AttributeSet = CreateDefaultSubobject<USpellsAttributeSet>("AttributeSet");
	
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* ASpellsPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
