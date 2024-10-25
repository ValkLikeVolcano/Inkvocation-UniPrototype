// Copyright (c) 2024 Nic. All Rights Reserved.


#include "SpellsAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"

USpellsAttributeSet::USpellsAttributeSet()
{
}

void USpellsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USpellsAttributeSet, Ink, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USpellsAttributeSet, InkMax, COND_None, REPNOTIFY_Always);
}

void USpellsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetInkAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetInkMax());
	}
	if(Attribute == GetInkMaxAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.f);
	}
}

void USpellsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if(Data.EvaluatedData.Attribute == GetInkAttribute())
	{
		SetInk(FMath::Clamp(GetInk(), 0.f, GetInkMax()));
	}
	if(Data.EvaluatedData.Attribute == GetInkMaxAttribute())
	{
		SetInkMax(FMath::Max(GetInkMax(), 1.f));
	}
}

void USpellsAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
}

void USpellsAttributeSet::OnRep_Ink(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpellsAttributeSet, Ink, OldValue);
}

void USpellsAttributeSet::OnRep_InkMax(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USpellsAttributeSet, InkMax, OldValue);
}
