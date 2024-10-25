// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "StateMachineComponent.generated.h"

DECLARE_DELEGATE_OneParam(FStateChangedSignature, const FGameplayTag NewStateTag);
DECLARE_DELEGATE_OneParam(FStateInitSignature, const FGameplayTag InitStateTag);
DECLARE_DELEGATE_RetVal_TwoParams(bool, FStateExitSignature, const FGameplayTag, const FGameplayTag);
DECLARE_DELEGATE_TwoParams(FStateTickSignature, const FGameplayTag TickStateTag, float DeltaTime);

UCLASS( ClassGroup=(StateMachine), meta=(BlueprintSpawnableComponent), Blueprintable)
class CGSI_GAMEPROTOTYPE_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UStateMachineComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/* State Delegates */
	FStateChangedSignature StateChangedDelegate;
	FStateInitSignature StateInitDelegate;
	FStateTickSignature StateTickDelegate;
	FStateExitSignature StateExitDelegate;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTag StateTag;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FGameplayTag InitialStateTag;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool bDebug = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<FGameplayTag> StateHistory;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 StateHistoryLength = 5;

	UFUNCTION(BlueprintCallable)
	void SwitchState(FGameplayTag NewStateTag);
	
protected:

	virtual void BeginPlay() override;

private:

	bool bCanTickState = false;
	void InitState();
	void TickState(float DeltaTime);
	bool ExitState(const FGameplayTag NewStateTag);

};
