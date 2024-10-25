// Copyright (c) 2024 Nic. All Rights Reserved.


#include "SpellsAbilitySystemComponent.h"

#include "Ability/ShapeGameplayAbility.h"
#include "CGSI_GamePrototype/SpellsGameplayTags.h"
#include "CGSI_GamePrototype/Spells/Paint/UnistrokeResult.h"

void USpellsAbilitySystemComponent::ShapeRecognized(const FUnistrokeResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Detected Shape: %s"), *Result.ShapeTag.ToString());

	FGameplayTagContainer InvalidTags;
	InvalidTags.AddTag(SpellTags::Shape_NoMatch);
	InvalidTags.AddTag(SpellTags::Shape_NotEnoughPoints);

	if (!Result.ShapeTag.IsValid() || Result.ShapeTag.MatchesAnyExact(InvalidTags))
	{
		return;
	}

	ABILITYLIST_SCOPE_LOCK(); // Prevent changes in the Ability Array while iterating over it
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(Result.ShapeTag) && !AbilitySpec.
			IsActive())
		{
			if (!bAllowMultipleShapes)
			{
				const FGameplayTagContainer ShapeAbilities{SpellTags::Ability_Type_Shape};
				CancelAbilities(&ShapeAbilities, nullptr, AbilitySpec.Ability);
			}
			
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}
