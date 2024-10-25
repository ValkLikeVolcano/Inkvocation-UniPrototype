// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "Interface/SpellsInterface.h"
#include "Paint/UnistrokeResult.h"
#include "SpellsCharacter.generated.h"

class UMovementStats;
class UStateMachineComponent;
class USpellsLocomotionComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
struct FInputActionValue;
class UShapeAbilitySet;
struct FGameplayTag;
class USpellsAbilitySystemComponent;
class USpellsAttributeSet;

UCLASS()
class CGSI_GAMEPROTOTYPE_API ASpellsCharacter : public ACharacter, public IAbilitySystemInterface, public ISpellsInterface
{
	GENERATED_BODY()

public:
	ASpellsCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult& Hit) override;

	UFUNCTION(BlueprintCallable)
	void TeleportCharacter(const FVector& Location);

	UFUNCTION(BlueprintCallable)
	void SetStateDisabled(const bool bEnable) const;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	USpellsAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	virtual FVector GetSpawnLocation() override;
	virtual FRotator GetSpawnRotation() override;

protected:
	virtual void BeginPlay() override;
	
	void InitAbilityActorInfo();
	void AddCharacterAbilities();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spells|Ability")
	TObjectPtr<UShapeAbilitySet> ShapeAbilitySet;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpellsAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpellsAttributeSet> AttributeSet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UStateMachineComponent> StateMachine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	TObjectPtr<UMovementStats> Stats;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> CancelShapeAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> CommitShapeAction;
	
	void Input_Look(const FInputActionValue& Value);
	void Input_Move(const FInputActionValue& Value);
	void Input_RequestCrouch(const FInputActionValue& Value);
	void Input_RequestJump();
	void Input_CancelShape();
	void Input_CommitShape();

	//Delegate functions
	UFUNCTION()
	void OnStateInit(const FGameplayTag InitStateTag);
	UFUNCTION()
	void OnStateTick(const FGameplayTag TickStateTag, float DeltaTime);
	UFUNCTION()
	bool OnStateExit(const FGameplayTag ExitStateTag, const FGameplayTag NewStateTag);

	//Camera
	void UpdateCamera(float DeltaTime) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	TObjectPtr<UCurveFloat> CrouchCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	TObjectPtr<UCurveFloat> FOVCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curves")
	TObjectPtr<UCurveFloat> WallRunGravityScale;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> PC;

	FVector2D InputAxis;

	void InitJump();
	void InitSlide(const bool bEntryBoost = true);
	void InitWallRun();
	void WallTick(const float DeltaTime);
	
	void AddMovementImpulse(const float Boost) const;
	float DistanceToGround(const FVector& Direction) const;

	bool bCrouchRequested;
	float CurrentCamZOffset;
	
	FTimeline CrouchTimeline;
	UFUNCTION()
	void CrouchUpdate(const float Alpha);
	UFUNCTION()
	void CrouchFinished();

	bool CanStandUp() const;
	
	void ChangeFOV(const float NewFOV, const float Speed);
	FTimeline FOVTimeline;
	UFUNCTION()
	void FOVUpdate(const float Alpha);
	UFUNCTION()
	void FOVFinished();
	float TargetFOV;

	// Used to determine the State to switch to when the character lands on stable ground
	// States: WALK, CROUCH, SLIDE
	void HandleLandedState() const;

	FVector GetFloorSlopeInfluence() const;
	bool MovesDownSlope() const;
	float GetSlopeAngle() const;

	inline bool IsInState(const FGameplayTag& State) const;
	inline FGameplayTag GetCurrentState() const;
	inline FGameplayTag GetLastState() const;

	void CheckWallRunPossibility() const;
	bool WallTrace(FHitResult& HitResult, const FVector& Start, const FVector& Direction) const;
	bool ValidWallAngle(const FVector& Normal) const;
	bool TryGetWallHit(FHitResult& HitResult) const;
	bool CheckWallHit(const FVector& Direction, FHitResult& UpperHit) const;
	FVector LastWallNormal;
	float TimeOnWallScaled = 0.f;
	float WallCamAxis = 0.f;

	FTimeline WallRunTimeline;
	UFUNCTION()
	void WallRunUpdate(const float Alpha);
	UFUNCTION()
	void WallRunFinished();

	FTimerHandle WallResetTimerHandle;
	void ResetWallRun() { LastWallNormal = FVector::ZeroVector; }
};
