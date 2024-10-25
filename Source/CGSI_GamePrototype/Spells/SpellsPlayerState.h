// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SpellsPlayerState.generated.h"

class USpellsAttributeSet;
class USpellsAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class CGSI_GAMEPROTOTYPE_API ASpellsPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ASpellsPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	USpellsAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpellsAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpellsAttributeSet> AttributeSet;
};

