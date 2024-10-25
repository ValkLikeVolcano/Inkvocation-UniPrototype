// // Copyright (c) 2024 Nic. All Rights Reserved.


#include "SpellsCharacter.h"

#include "EnhancedInputComponent.h"
#include "SpellsPlayerState.h"
#include "AbilitySystem/SpellsAbilitySystemComponent.h"
#include "AbilitySystem/Data/ShapeAbilitySet.h"
#include "Camera/CameraComponent.h"
#include "CGSI_GamePrototype/SpellsGameplayTags.h"
#include "Component/StateMachineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Data/MovementStats.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#pragma region Definitions

#define WALK		SpellTags::MovementState_Walk
#define DISABLED	SpellTags::MovementState_Disabled
#define FALL		SpellTags::MovementState_Fall
#define JUMP		SpellTags::MovementState_Jump
#define CROUCH		SpellTags::MovementState_Crouch
#define SLIDE		SpellTags::MovementState_Slide
#define WALLRUN		SpellTags::MovementState_Wallrun

static constexpr float DOWN_SLOPE_ANGLE = 70.f;
static constexpr float SLIDE_SLOPE_THRESHOLD = 0.15f;
static constexpr float MOVEMENT_BOOST_MULTIPLIER = 100.f;
static constexpr float SLIDE_GROUND_FRICTION = 0.f;
static constexpr float FALL_MAX_FRICTION_SPEED = 4000.f;
static constexpr float FALL_MAX_FRICTION = 5.f;
static constexpr float SLIDE_MIN_ENTER_VELOCITY = 150.f;
static constexpr float SLIDE_TO_CROUCH_ENTRY_BUFFER = 250.f;
static constexpr float SLIDE_STEER_FORCE = 85000.f;
static constexpr float SLIDE_MAX_MIN_ENTRY_VELOCITY = 1400.f;
static constexpr float WALL_ENTRY_HEIGHT_ADDITION = 50.f;

#define IS_IN_STATE_MANY(...) \
	([&]() -> bool \
	{ \
		std::initializer_list<FGameplayTag> tags{__VA_ARGS__}; \
		for (const auto& tag : tags) \
		{ \
			if (StateMachine->StateTag == tag) \
			{ \
			return true; \
			} \
		} \
		return false; \
	}())

TArray<FGameplayTag> ToArray (TArray<FGameplayTag> Array)
{
	return Array;
}

#define HAS_TAG(InputTag) AbilitySystemComponent->HasMatchingGameplayTag(InputTag)

#pragma endregion

class ASpellsPlayerState;

ASpellsCharacter::ASpellsCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UStateMachineComponent>("StateMachine");
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocation(FVector(0.f, 0.f, Stats == nullptr ? 60.f : Stats->DefaultCamZOffset));
	CameraBoom->TargetArmLength = 0.f;
	CameraBoom->CameraLagSpeed = 50.f;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
}

void ASpellsCharacter::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(Stats, TEXT("Stats Data Asset is not set on %s"), *GetName());
	ensureMsgf(StateMachine, TEXT("StateMachine Component is not set on %s"), *GetName());
	
	StateMachine->StateInitDelegate.BindUObject(this, &ThisClass::OnStateInit);
	StateMachine->StateTickDelegate.BindUObject(this, &ThisClass::OnStateTick);
	StateMachine->StateExitDelegate.BindUObject(this, &ThisClass::OnStateExit);
	
	PC = Cast<APlayerController>(GetController());

	CurrentCamZOffset = Stats->DefaultCamZOffset;
	
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->AirControl = Stats->AirControl;
	Movement->JumpZVelocity = Stats->JumpZVelocity;
	Movement->GravityScale = Stats->DefaultGravityScale;
	Movement->MaxWalkSpeed = Stats->WalkForwardSpeed;
	Movement->BrakingDecelerationWalking = Stats->WalkBrakingDeceleration;
	Movement->MaxAcceleration = Stats->WalkAcceleration;
	Movement->SetWalkableFloorAngle(Stats->MaxWalkableFloorAngle);

	GetCapsuleComponent()->SetCapsuleHalfHeight(Stats->DefaultCapsuleHalfHeight);

	FOnTimelineFloat CrouchUpdate;
	CrouchUpdate.BindUFunction(this, FName("CrouchUpdate"));
	FOnTimelineEvent CrouchFinished;
	CrouchFinished.BindUFunction(this, FName("CrouchFinished"));

	CrouchTimeline.AddInterpFloat(CrouchCurve, CrouchUpdate);
	CrouchTimeline.SetTimelineFinishedFunc(CrouchFinished);

	FOnTimelineFloat FOVUpdate;
	FOVUpdate.BindUFunction(this, FName("FOVUpdate"));
	FOnTimelineEvent FOVFinished;
	FOVFinished.BindUFunction(this, FName("FOVFinished"));

	FOVTimeline.AddInterpFloat(FOVCurve, FOVUpdate);
	FOVTimeline.SetTimelineFinishedFunc(FOVFinished);

	FOnTimelineFloat WallRunUpdate;
	WallRunUpdate.BindUFunction(this, FName("WallRunUpdate"));
	FOnTimelineEvent WallRunFinished;
	WallRunFinished.BindUFunction(this, FName("WallRunFinished"));

	WallRunTimeline.AddInterpFloat(WallRunGravityScale, WallRunUpdate);
	WallRunTimeline.SetTimelineFinishedFunc(WallRunFinished);

	//Detach Spring arm for crouching, Mantle and other advanced Camera movements
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		false);
	CameraBoom->DetachFromComponent(DetachRules);
}

void ASpellsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// Timeline Component Tick
	FOVTimeline.TickTimeline(DeltaSeconds);
	if(HAS_TAG(SpellTags::Ability_Movement_Crouch)) CrouchTimeline.TickTimeline(DeltaSeconds);
	if(HAS_TAG(SpellTags::Ability_Movement_Wallrun)) WallRunTimeline.TickTimeline(DeltaSeconds);
	
	UpdateCamera(DeltaSeconds);

	if(bCrouchRequested && HAS_TAG(SpellTags::Ability_Movement_Crouch) && IsInState(WALK))
	{
		if(HAS_TAG(SpellTags::MovementState_Crouch))
		{
			StateMachine->SwitchState(CROUCH);
		}
		else
		{
			StateMachine->SwitchState(GetVelocity().Length() > 2.f ? SLIDE : CROUCH);
		}
	}
	else if(!bCrouchRequested && (IsInState(CROUCH) || IsInState(SLIDE)) && CanStandUp())
	{
		StateMachine->SwitchState(WALK);
	}
}

void ASpellsCharacter::UpdateCamera(float DeltaTime) const
{
	FVector TargetLocation = GetActorLocation() + FVector(0.f, 0.f, CurrentCamZOffset - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	CameraBoom->SetWorldLocation(TargetLocation);

	if(!Stats->bUseTilt) return; // improve to handle case where cam is tilted when the bool is set to false
	
	float TargetRoll = 0.f;

	//if grounded (No Slide) or wall running tilt the camera else go towards 0
	if(IS_IN_STATE_MANY(WALK, CROUCH))
	{
		TargetRoll = UKismetMathLibrary::MapRangeClamped(InputAxis.X, -1, 1, -Stats->CameraTiltAngle, Stats->CameraTiltAngle);
	}
	else if(IsInState(WALLRUN))
	{
		float TiltAngle = UKismetMathLibrary::Lerp(Stats->CameraWallTiltAngle, Stats->CameraWallTiltAngle * .5f, TimeOnWallScaled);
		TargetRoll = UKismetMathLibrary::MapRangeClamped(WallCamAxis, -1, 1, -TiltAngle, TiltAngle);
	}
	
	FRotator ControlRot = PC->GetControlRotation();
	FRotator CamRoll = FRotator(ControlRot.Pitch, ControlRot.Yaw, TargetRoll);
	if(CamRoll.Roll != ControlRot.Roll)
	{
		PC->SetControlRotation(UKismetMathLibrary::RInterpTo(ControlRot, CamRoll, DeltaTime, Stats->CameraTiltSpeed));
	}
}

#pragma region Input

void ASpellsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(Input);

	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_RequestJump);
	Input->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::Input_RequestCrouch);
	Input->BindAction(CancelShapeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_CancelShape);
	Input->BindAction(CommitShapeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_CommitShape);
}

void ASpellsCharacter::Input_Look(const FInputActionValue& Value)
{
	if(AbilitySystemComponent->HasMatchingGameplayTag(SpellTags::SpellState_Drawing))
	{
		return;
	}

	FVector2D LookValue = Value.Get<FVector2D>();
	if(!PC || LookValue.IsZero()) return;

	PC->AddYawInput(LookValue.X * Stats->CameraSensitivityX);
	PC->AddPitchInput(LookValue.Y * Stats->CameraSensitivityY * (Stats->bInvertY ? -1 : 1));
}

void ASpellsCharacter::Input_Move(const FInputActionValue& Value)
{
	InputAxis = Value.Get<FVector2D>();
	
	if(!PC || InputAxis.IsZero()) return;

	if(IS_IN_STATE_MANY(WALK, JUMP, FALL, CROUCH))
	{
		FVector Forward = GetActorForwardVector();
		FVector Right = GetActorRightVector();

		AddMovementInput(Forward, InputAxis.Y);
		
		float Multiplier = 0.f;
		if(IsInState(WALK)) Multiplier = Stats->WalkRightMultiplier;
		else if(IsInState(JUMP)) Multiplier = Stats->WalkRightMultiplier;
		else if(IsInState(FALL)) Multiplier = Stats->FallRightMultiplier;
		else if(IsInState(CROUCH)) Multiplier = Stats->CrouchRightMultiplier;

		AddMovementInput(Right, InputAxis.X * Multiplier);
	}

	else if(IsInState(SLIDE))
	{
		// Calculate the force to apply for character movement based on player input and slope direction.
		FVector InputDirection = GetActorForwardVector() * InputAxis.Y + GetActorRightVector() * InputAxis.X;
		FVector SlopeNormal = GetCharacterMovement()->CurrentFloor.HitResult.ImpactNormal;
		FVector ProjectedInputDirection = InputDirection - (FVector::DotProduct(InputDirection, SlopeNormal) * SlopeNormal);
		FVector FinalInputDirection = ProjectedInputDirection.GetSafeNormal() * InputAxis.Size();
		FVector SlopeRightVector;
		if(UKismetMathLibrary::EqualEqual_VectorVector(SlopeNormal, FVector::UpVector, 0.1f)) SlopeRightVector = GetActorRightVector(); // if on flat ground take the actor right vector
		else SlopeRightVector = FVector::CrossProduct(SlopeNormal, FVector::UpVector);
		float InputMagnitudeInSlopeRightDirection = FVector::DotProduct(FinalInputDirection, SlopeRightVector.GetSafeNormal());
		FVector ForceToApply = SlopeRightVector * InputMagnitudeInSlopeRightDirection;

		GetCharacterMovement()->AddForce(ForceToApply * SLIDE_STEER_FORCE);
		
	}
}

void ASpellsCharacter::Input_RequestJump()
{
	JumpMaxCount = HAS_TAG(SpellTags::Ability_Movement_Jump_DoubleJump) ? 2 : 1;

	if(!HAS_TAG(SpellTags::Ability_Movement_Jump)) return;
	
	if(IsInState(WALK))
	{
		StateMachine->SwitchState(JUMP);
	}
	else if(IsInState(WALLRUN))
	{
		JumpCurrentCount = -1;
		StateMachine->SwitchState(JUMP);
	}
	else if(IS_IN_STATE_MANY(CROUCH, SLIDE) && CanStandUp())
	{
		StateMachine->SwitchState(JUMP);
	}

	if(!HAS_TAG(SpellTags::Ability_Movement_Jump_DoubleJump)) return;
	
	if(IsInState(FALL)) StateMachine->SwitchState(JUMP);

	else if(IsInState(JUMP))
	{
		// Add force and jump manually
		GetCharacterMovement()->JumpZVelocity = Stats->DoubleJumpZVelocity;
		GetCharacterMovement()->AirControl = Stats->DoubleJumpAirControl;
		Jump();
		float Boost = 0.f;
		if(GetVelocity().Size() <= Stats->JumpMaxBoostVelocity) Boost = Stats->DoubleJumpBoost;
		AddMovementImpulse(Boost);
	}
}

void ASpellsCharacter::Input_CancelShape()
{
	const FGameplayTagContainer ShapeAbilities{ SpellTags::Ability_Type_Shape };
	AbilitySystemComponent->CancelAbilities(&ShapeAbilities);
}

void ASpellsCharacter::Input_CommitShape()
{
	FGameplayEventData Payload;
	Payload.EventTag = SpellTags::Event_CommitShape;
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void ASpellsCharacter::Input_RequestCrouch(const FInputActionValue& Value)
{
	bCrouchRequested = Value.Get<bool>();
}

#pragma endregion

#pragma region State Machine

void ASpellsCharacter::OnStateInit(const FGameplayTag InitStateTag)
{
	if(IsInState(WALK))
	{
		GetCharacterMovement()->MaxWalkSpeed = Stats->WalkForwardSpeed;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	else if(IsInState(JUMP)) InitJump();
	else if(IsInState(FALL))
	{
		GetCharacterMovement()->GravityScale = Stats->FallGravityScale;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
	else if(IsInState(CROUCH))
	{
		GetCharacterMovement()->MaxWalkSpeed = Stats->CrouchForwardSpeed;
	
		if(GetLastState() == SLIDE) return;
		
		CrouchTimeline.SetPlayRate(GetLastState()== FALL ? Stats->CrouchFastDownSpeed : Stats->CrouchDownSpeed);
		CrouchTimeline.Play();
	}
	else if(IsInState(SLIDE))
	{
		if(GetLastState()!= CROUCH)
		{
			CrouchTimeline.SetPlayRate(GetLastState()== FALL ? Stats->CrouchFastDownSpeed : Stats->CrouchDownSpeed);
			CrouchTimeline.Play();
		}
		InitSlide();
	}
	else if(IsInState(WALLRUN))
	{
		InitWallRun();
	}
	else if(IsInState(DISABLED))
	{
		DisableInput(PC);
	}
}

void ASpellsCharacter::InitJump()
{
	GetCharacterMovement()->bNotifyApex = true; 
	GetCharacterMovement()->JumpZVelocity = JumpCurrentCount == 0 ? Stats->JumpZVelocity : Stats->DoubleJumpZVelocity;
	GetCharacterMovement()->AirControl = JumpCurrentCount == 0 ? Stats->AirControl : Stats->DoubleJumpAirControl;
	GetCharacterMovement()->GravityScale = Stats->DefaultGravityScale;
	
	FGameplayTag LastState = GetLastState();
	if(LastState == WALLRUN)
	{
		FVector OffImpulse = LastWallNormal * Stats->WallJumpForce;
		GetCharacterMovement()->AddImpulse(OffImpulse, true);
		FVector UpImpulse = FVector::UpVector * Stats->WallJumpUpForce;
		GetCharacterMovement()->AddImpulse(UpImpulse, true);
		GetCharacterMovement()->GravityScale = Stats->FallGravityScale;
		return;
	}
	
	Jump();
	
	float Boost = 0.f;
	if(GetVelocity().Size() <= Stats->JumpMaxBoostVelocity)
	{
		if(LastState == WALK) Boost = Stats->JumpWalkBoost;
		else if(LastState == CROUCH) Boost = Stats->JumpCrouchBoost;
		else if(LastState == SLIDE) Boost = Stats->JumpSlideBoost;
		else if(LastState == FALL) Boost = Stats->DoubleJumpBoost;
	}
	AddMovementImpulse(Boost);
}

void ASpellsCharacter::InitSlide(const bool bEntryBoost)
{
	GetCharacterMovement()->GroundFriction = SLIDE_GROUND_FRICTION;
	GetCharacterMovement()->BrakingDecelerationWalking = Stats->SlideBrakingDeceleration;

	FVector VelocityFlat = GetVelocity();
	VelocityFlat.Z = 0.f;

	if(bEntryBoost && VelocityFlat.Size() <= Stats->SlideMaxEntryBoostVelocity)
	{
		
		float VelocityDiff = Stats->SlideMaxEntryBoostVelocity - VelocityFlat.Size();

		if(VelocityDiff > 0)
		{
			//TODO: Add Variation depending on slope angle
			FVector NewVelocity = VelocityFlat.GetSafeNormal() * (VelocityFlat.Size() + Stats->SlideEntryBoostAddedVelocity);
			float Alpha = GetSlopeAngle() / Stats->MaxWalkableFloorAngle;
			float MinSlideSpeed = UKismetMathLibrary::Lerp(Stats->CrouchForwardSpeed * Stats->CrouchRightMultiplier, SLIDE_MAX_MIN_ENTRY_VELOCITY, Alpha);
			GetCharacterMovement()->Velocity = NewVelocity.GetClampedToSize(MinSlideSpeed, Stats->SlideMaxEntryBoostVelocity);

		}
	}
}

void ASpellsCharacter::InitWallRun()
{
	JumpCurrentCountPreJump = 0;
	JumpCurrentCount = 0;
	TimeOnWallScaled = 0.f;
	GetCharacterMovement()->Velocity = FVector(GetVelocity().X, GetVelocity().Y, 0.f);

	const FVector Impulse = GetActorForwardVector() * Stats->WallConnectImpulse; 
	GetCharacterMovement()->AddImpulse(Impulse);

	ChangeFOV(Stats->CameraExtendedFOV, Stats->WallEnterFOVSpeed);

	WallRunTimeline.SetPlayRate(1.f / Stats->WallRunDuration);
	WallRunTimeline.PlayFromStart();
}

void ASpellsCharacter::OnStateTick(const FGameplayTag TickStateTag, float DeltaTime)
{
	if(IS_IN_STATE_MANY(JUMP, FALL))
	{
		// TODO: Add Attachment checks for Ledge

		FVector VelocityFlat = GetVelocity();
		VelocityFlat.Z = 0.f;
		float Alpha = (VelocityFlat.Size() - Stats->FallMaxFrictionlessSpeed) / (FALL_MAX_FRICTION_SPEED - Stats->FallMaxFrictionlessSpeed);
		GetCharacterMovement()->FallingLateralFriction = UKismetMathLibrary::Lerp(0.f, FALL_MAX_FRICTION, UKismetMathLibrary::Max(Alpha, 0.f));
		
		if(GetCharacterMovement()->IsMovingOnGround())
		{
			HandleLandedState();
		}

		if(HAS_TAG(SpellTags::Ability_Movement_Wallrun)) CheckWallRunPossibility();
	}
	
	if(IS_IN_STATE_MANY(WALK, CROUCH, SLIDE))
	{
		if(IS_IN_STATE_MANY(CROUCH, SLIDE) && !bCrouchRequested && CanStandUp())
		{
			StateMachine->SwitchState(WALK);
		}
		if(GetCharacterMovement()->IsFalling())
		{
			StateMachine->SwitchState(FALL);
		}
	}

	if(IsInState(CROUCH))
	{
		 if(MovesDownSlope() && HAS_TAG(SpellTags::Ability_Movement_Crouch_Slide))
		 {
			 StateMachine->SwitchState(SLIDE);
		 }
	}

	else if(IsInState(SLIDE))
	{
		FVector SlopeInfluence = GetFloorSlopeInfluence();
		FVector Forward = GetActorForwardVector();
		FVector SlideForce = FVector::Zero();
		
		float Dot = FVector::DotProduct(SlopeInfluence, Forward);

		if(Dot > SLIDE_SLOPE_THRESHOLD && GetVelocity().Length() < Stats->SlideMaxSpeed) // Moving Downhill and under Max Speed
		{
			SlideForce = SlopeInfluence * Stats->SlideSlopeAcceleration;
		}
		else if(Dot < -SLIDE_SLOPE_THRESHOLD) // Moving Uphill
		{
			SlideForce = SlopeInfluence * Stats->SlideSlopeDeceleration;
		}
		
		GetCharacterMovement()->AddForce(SlideForce);
		
		const float CrouchThreshold = Stats->CrouchForwardSpeed - SLIDE_TO_CROUCH_ENTRY_BUFFER; //IDFK how to name this shit
		
		if(GetVelocity().Length() < CrouchThreshold && !MovesDownSlope())
		{
			StateMachine->SwitchState(CROUCH);
		}

		float Alpha = (GetVelocity().Length() - Stats->CrouchForwardSpeed) / (Stats->SlideMaxSpeed - Stats->CrouchForwardSpeed);
		float FOV = UKismetMathLibrary::Lerp(Stats->CameraDefaultFOV, Stats->CameraExtendedFOV, Alpha);
		Camera->FieldOfView = UKismetMathLibrary::FInterpTo(Camera->FieldOfView, FOV, DeltaTime, 10.f);
	}

	else if (IsInState(WALLRUN))
	{
		WallTick(DeltaTime);
	}
}

void ASpellsCharacter::WallTick(const float DeltaTime)
{
	if(bCrouchRequested)
	{
		GetCharacterMovement()->AddImpulse(LastWallNormal * Stats->WallExitCrouchRepelForce);
		StateMachine->SwitchState(FALL);
	}

	GetCharacterMovement()->Velocity = UKismetMathLibrary::ClampVectorSize(GetVelocity(), 0.f, Stats->WallMaxVelocity);

	FHitResult WallHit;
	if(TryGetWallHit(WallHit))
	{
		LastWallNormal = WallHit.ImpactNormal;
		WallCamAxis = UKismetMathLibrary::Dot_VectorVector(GetActorRightVector(), LastWallNormal);

		GetCharacterMovement()->AddForce(-LastWallNormal * Stats->WallAttractionForce);
		GetCharacterMovement()->AddForce(GetActorForwardVector() * Stats->WallMoveSpeed); //TODO: Project Input axis on wall

		FVector WallDown = FVector::CrossProduct(LastWallNormal, FVector::UpVector);
		WallDown = FVector::CrossProduct(WallDown, LastWallNormal);
		if(DistanceToGround(WallDown) < Stats->WallMinDistanceToGround)
		{
			StateMachine->SwitchState(FALL);
		}
	}
	else
	{
		StateMachine->SwitchState(FALL);
	}
}

bool ASpellsCharacter::OnStateExit(const FGameplayTag ExitStateTag, const FGameplayTag NewStateTag)
{
		if(ExitStateTag == JUMP)
	{
		GetCharacterMovement()->GravityScale = Stats->FallGravityScale;
	}
	else if(ExitStateTag == FALL)
	{
		GetCharacterMovement()->GravityScale = Stats->DefaultGravityScale;
	}
	else if(ExitStateTag == CROUCH)
	{
		// Returns false if the character can't stand up and the new state is not Slide
		if(!CanStandUp() && NewStateTag != SLIDE) return false;

		// Uncrouch if not transitioning to Slide
		if(NewStateTag != SLIDE)
		{
			CrouchTimeline.SetPlayRate(Stats->CrouchUpSpeed);
			CrouchTimeline.Reverse();
		}
	}
	else if(ExitStateTag == SLIDE)
	{
		// Returns false if the character can't stand up and the new state is not Crouch
		if(!CanStandUp() && NewStateTag != CROUCH) return false;
		
		ChangeFOV(Stats->CameraDefaultFOV, Stats->SlideExitFOVSpeed);
		GetCharacterMovement()->GroundFriction = Stats->DefaultGroundFriction;
		GetCharacterMovement()->BrakingDecelerationWalking = Stats->WalkBrakingDeceleration;

		// Uncrouch if not transitioning to Crouch
		if(NewStateTag != CROUCH)
		{
			CrouchTimeline.SetPlayRate(Stats->CrouchUpSpeed);
			CrouchTimeline.Reverse();
		}
	}
	else if(ExitStateTag == DISABLED)
	{
		EnableInput(PC);
	}
	else if(ExitStateTag == WALLRUN)
	{
		ChangeFOV(Stats->CameraDefaultFOV, 3.f);
		WallRunTimeline.Stop();
		GetWorld()->GetTimerManager().SetTimer(WallResetTimerHandle,
			this,
			&ThisClass::ResetWallRun,
			Stats->WallBanResetTime,
			false);
	}
	return true;
}

#pragma endregion

void ASpellsCharacter::SetStateDisabled(const bool bEnable) const
{
	if(bEnable && IsInState(DISABLED))
	{
		GetCharacterMovement()->IsMovingOnGround() ? StateMachine->SwitchState(WALK) : StateMachine->SwitchState(FALL);
	}
	else if(!bEnable && !IsInState(DISABLED))
	{
		StateMachine->SwitchState(DISABLED);
	}
}

void ASpellsCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();

	// if Jumping Transition to fall
	if(IsInState(JUMP))
	{
		StateMachine->SwitchState(FALL);
	}
}

void ASpellsCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	JumpCurrentCount = 0;
	HandleLandedState();
}

void ASpellsCharacter::TeleportCharacter(const FVector& Location)
{
	StateMachine->SwitchState(DISABLED);
	TeleportTo(Location, GetActorRotation());
	StateMachine->SwitchState(FALL);
	ResetWallRun();
}

void ASpellsCharacter::HandleLandedState() const
{
	if(IS_IN_STATE_MANY(JUMP, FALL, WALLRUN)) //Add Mantle and Rail
	{
		if(bCrouchRequested && HAS_TAG(SpellTags::Ability_Movement_Crouch))
		{
			if(HAS_TAG(SpellTags::Ability_Movement_Crouch_Slide))
			{
				FVector Velocity = GetVelocity();
				Velocity.Z = 0.f;
				StateMachine->SwitchState(Velocity.Length() > SLIDE_MIN_ENTER_VELOCITY ? SLIDE : CROUCH);
				return;
			}
			
			StateMachine->SwitchState(CROUCH);
			return;
		}
		
		StateMachine->SwitchState(WALK);
	}
}

void ASpellsCharacter::AddMovementImpulse(const float Boost) const
{
	// Get the Vector of the Input axis relative to the actors forward
	FVector Impulse = (GetActorForwardVector() * InputAxis.Y + GetActorRightVector() * InputAxis.X).GetSafeNormal();
	Impulse *= MOVEMENT_BOOST_MULTIPLIER * Boost;
	GetCharacterMovement()->AddImpulse(Impulse);
}

float ASpellsCharacter::DistanceToGround(const FVector& Direction) const
{
	FHitResult Hit;

	if(GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() + Direction * 100000.f, ECC_Visibility))
	{
		return Hit.Distance;
	}
	return 9999999.f;
}

bool ASpellsCharacter::CanStandUp() const
{
	float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float CapsuleRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
	
	FVector Start = GetActorLocation() - FVector(0.f, 0.f, HalfHeight - CapsuleRadius);
	FVector End = Start + FVector(0.f, 0.f, Stats->DefaultCapsuleHalfHeight * 2 - CapsuleRadius * 2);
	FHitResult Hit;
	
	return !UKismetSystemLibrary::SphereTraceSingle(
		this,
		Start,
		End,
		CapsuleRadius,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		{},
		EDrawDebugTrace::None,
		Hit,
		true);
}

FVector ASpellsCharacter::GetFloorSlopeInfluence() const
{
	FVector FloorNormal = GetCharacterMovement()->CurrentFloor.HitResult.ImpactNormal;
	
	if(UKismetMathLibrary::EqualEqual_VectorVector(FVector::UpVector, FloorNormal, 0.01f))
	{
		return FVector::ZeroVector;
	}

	FVector SlopeRight = FVector::CrossProduct(UKismetMathLibrary::ProjectVectorOnToPlane(FloorNormal, FVector::UpVector), FVector::UpVector);
	FVector SlopeInfluence = FVector::CrossProduct(FloorNormal, SlopeRight).GetSafeNormal();
	
	return SlopeInfluence;
}

bool ASpellsCharacter::MovesDownSlope() const
{
	FVector SlopeInfluence = GetFloorSlopeInfluence();
	FVector ProjectedVelocity = UKismetMathLibrary::ProjectVectorOnToPlane(GetVelocity(), FVector::UpVector).GetSafeNormal();
	float Dot = FVector::DotProduct(SlopeInfluence, ProjectedVelocity);
	
	return 	UKismetMathLibrary::DegAcos(Dot) < DOWN_SLOPE_ANGLE;
}

float ASpellsCharacter::GetSlopeAngle() const
{
	FHitResult Hit;

	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() - FVector::UpVector * 100, ECC_Visibility);
	if (!Hit.bBlockingHit) return 90.f;
	
	FVector SlopeNormal = Hit.ImpactNormal;
	float Angle = UKismetMathLibrary::DegAcos(FVector::DotProduct(SlopeNormal, FVector::UpVector));
	return Angle;
}

void ASpellsCharacter::CheckWallRunPossibility() const
{
	if(DistanceToGround(FVector::DownVector) < Stats->WallMinDistanceToGround + WALL_ENTRY_HEIGHT_ADDITION) return;
	
	FHitResult RightUpperHit;
	if(CheckWallHit(GetActorRightVector(), RightUpperHit) && UKismetMathLibrary::NotEqual_VectorVector(RightUpperHit.ImpactNormal, LastWallNormal, 0.2f))
	{
		if(!IsInState(WALLRUN))
		{
			StateMachine->SwitchState(WALLRUN);
		}
		return;
	}
	
	FHitResult LeftUpperHit;
	if(CheckWallHit(-GetActorRightVector(), LeftUpperHit) && UKismetMathLibrary::NotEqual_VectorVector(LeftUpperHit.ImpactNormal, LastWallNormal, 0.2f))
	{
		if(!IsInState(WALLRUN))
		{
			StateMachine->SwitchState(WALLRUN);
		}
	}
}

bool ASpellsCharacter::TryGetWallHit(FHitResult& HitResult) const
{
	FHitResult RightHit;
	FHitResult LeftHit;
	bool RightWall = CheckWallHit(GetActorRightVector(), RightHit);
	bool LeftWall = CheckWallHit(-GetActorRightVector(), LeftHit);

	// If both walls are hit, choose the closest one
	if(RightWall && LeftWall)
	{
		RightHit.Distance < LeftHit.Distance ? HitResult = RightHit : HitResult = LeftHit;
		return true;
	}
	if(RightWall && !LeftWall)
	{
		HitResult = RightHit;
		return true;
	}
	if(!RightWall && LeftWall)
	{
		HitResult = LeftHit;
		return true;
	}

	return false;
}

bool ASpellsCharacter::CheckWallHit(const FVector& Direction, FHitResult& UpperHit) const
{
	const FVector HeightVar = FVector(0.f, 0.f, Stats->WallDetectionHeightVariation);
	
	FHitResult LowerHit;
	if(
		WallTrace(UpperHit, GetActorLocation() + HeightVar, Direction) &&
		WallTrace(LowerHit, GetActorLocation() - HeightVar, Direction))
		return true;
	
	return false;
}

bool ASpellsCharacter::WallTrace(FHitResult& HitResult, const FVector& Start, const FVector& Direction) const
{
	FVector End = Start + Direction * Stats->WallTraceDistance + GetActorForwardVector() * Stats->WallDetectionAngleOffset;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility
		);
	
	if(HitResult.bBlockingHit && ValidWallAngle(HitResult.ImpactNormal))
	{
		return true;
	}

	return false;
}

bool ASpellsCharacter::ValidWallAngle(const FVector& Normal) const
{
	float WallZ = Normal.Z;
	float MinAngle = UKismetMathLibrary::DegreesToRadians(Stats->WallMinTiltAngle);
	float MaxAngle = UKismetMathLibrary::DegreesToRadians(Stats->WallMaxTiltAngle);
	return UKismetMathLibrary::InRange_FloatFloat(WallZ, MinAngle, MaxAngle, true, true);
}

#pragma region Timelines

void ASpellsCharacter::CrouchUpdate(const float Alpha)
{
	if(CrouchTimeline.IsReversing() && !CanStandUp())
	{
		CrouchTimeline.Stop();
		StateMachine->SwitchState(CROUCH);
		return;
	}
	
	CurrentCamZOffset = UKismetMathLibrary::Lerp(Stats->DefaultCamZOffset, Stats->CrouchCamZOffset, Alpha);
	GetCapsuleComponent()->SetCapsuleHalfHeight(UKismetMathLibrary::Lerp(Stats->DefaultCapsuleHalfHeight, Stats->CrouchCapsuleHalfHeight, Alpha));
}

void ASpellsCharacter::CrouchFinished()
{
}

void ASpellsCharacter::ChangeFOV(const float NewFOV, const float Speed)
{
	FOVTimeline.SetPlayRate(Speed);
	TargetFOV = NewFOV;
	FOVTimeline.PlayFromStart();
}

void ASpellsCharacter::FOVUpdate(const float Alpha)
{
	float FOV = UKismetMathLibrary::Lerp(Camera->FieldOfView, TargetFOV, Alpha);
	Camera->SetFieldOfView(FOV);
}

void ASpellsCharacter::FOVFinished()
{
}

void ASpellsCharacter::WallRunUpdate(const float Alpha)
{
	TimeOnWallScaled = WallRunTimeline.GetPlaybackPosition() / WallRunTimeline.GetTimelineLength();
	float NewGravityScale = UKismetMathLibrary::Lerp(Stats->WallMinGravityScale, Stats->FallGravityScale, Alpha);
	GetCharacterMovement()->GravityScale = NewGravityScale;
}

void ASpellsCharacter::WallRunFinished()
{
	if(IsInState(WALLRUN)) StateMachine->SwitchState(FALL);
}

#pragma endregion

#pragma region MovementHelpers

bool ASpellsCharacter::IsInState(const FGameplayTag& State) const
{
	return StateMachine->StateTag == State;
}

FGameplayTag ASpellsCharacter::GetCurrentState() const
{
	return StateMachine->StateTag;
}

FGameplayTag ASpellsCharacter::GetLastState() const
{
	return StateMachine->StateHistory.Num() > 0 ? StateMachine->StateHistory.Last() : FGameplayTag();
}

#pragma endregion


UAbilitySystemComponent* ASpellsCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASpellsCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void ASpellsCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

FVector ASpellsCharacter::GetSpawnLocation()
{
	return Camera->GetComponentLocation() + Camera->GetForwardVector() * 60.f;
}

FRotator ASpellsCharacter::GetSpawnRotation()
{
	return PC->GetControlRotation();
}

void ASpellsCharacter::InitAbilityActorInfo()
{
	ASpellsPlayerState* PS = GetPlayerState<ASpellsPlayerState>();
	check(PS);

	AbilitySystemComponent = CastChecked<USpellsAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();
}

void ASpellsCharacter::AddCharacterAbilities()
{
	if(HasAuthority() && IsValid(AbilitySystemComponent) && IsValid(ShapeAbilitySet))
	{
		ShapeAbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr, this);
	}
}
