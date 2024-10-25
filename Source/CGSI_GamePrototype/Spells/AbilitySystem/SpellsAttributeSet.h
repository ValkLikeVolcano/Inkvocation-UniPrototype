// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "SpellsAttributeSet.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInkChanged, float, NewValue);

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class CGSI_GAMEPROTOTYPE_API USpellsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	USpellsAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Ink, Category = "Primary Attributes")
	FGameplayAttributeData Ink;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, Ink);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_InkMax, Category = "Primary Attributes")
	FGameplayAttributeData InkMax;
	ATTRIBUTE_ACCESSORS(USpellsAttributeSet, InkMax);
	
protected:
	UFUNCTION()
	void OnRep_Ink(const FGameplayAttributeData& OldValue) const;

	UFUNCTION()
	void OnRep_InkMax(const FGameplayAttributeData& OldValue) const;

};
