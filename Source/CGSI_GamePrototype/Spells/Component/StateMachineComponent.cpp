// Copyright (c) 2024 Nic. All Rights Reserved.


#include "StateMachineComponent.h"

UStateMachineComponent::UStateMachineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	StateTag = FGameplayTag::EmptyTag;
}

void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SwitchState(InitialStateTag);
}

void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(bCanTickState)
	{
		TickState(DeltaTime);
	}

	if(bDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, FString::Printf(TEXT("Current State: %s"), *StateTag.ToString()));

		if(StateHistory.Num()>0)
		{
			for(int32 i = StateHistory.Num()-1; i >= 0; i--)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT("State History %d: %s"), i, *StateHistory[i].ToString()));
			}
		}
	}
}

void UStateMachineComponent::SwitchState(FGameplayTag NewStateTag)
{
	if(!NewStateTag.MatchesTagExact(StateTag))
	{
		bCanTickState = false;

		// If Exit fails, the old State will NOT be Reinitialized (No Side effects)
		if(!ExitState(NewStateTag))
		{
			bCanTickState = true;
			return;
		}

		StateTag = NewStateTag;
		
		InitState();
		bCanTickState = true;
		if(StateChangedDelegate.IsBound())
		{
			StateChangedDelegate.Execute(StateTag);
		}
	}
	else if(bDebug)
	{
		UE_LOG(LogTemp, Warning, TEXT("State %s is already active"), *NewStateTag.ToString())
	}
}

void UStateMachineComponent::InitState()
{
	if(StateInitDelegate.IsBound())
	{
		StateInitDelegate.Execute(StateTag);
	}
}

void UStateMachineComponent::TickState(float DeltaTime)
{
	if(StateTickDelegate.IsBound())
	{
		StateTickDelegate.Execute(StateTag, DeltaTime);
	}
}

bool UStateMachineComponent::ExitState(FGameplayTag NewStateTag)
{
	if(StateHistory.Num() >= StateHistoryLength)
	{
		StateHistory.RemoveAt(0);
	}
	StateHistory.Push(StateTag);

	if(StateTag == FGameplayTag::EmptyTag)
	{
		return true;
	}
	
	if(StateExitDelegate.IsBound())
	{
		return StateExitDelegate.Execute(StateTag, NewStateTag);
	}
	return false;
}

