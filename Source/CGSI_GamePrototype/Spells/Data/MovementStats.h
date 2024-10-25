// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MovementStats.generated.h"

/**
 * 
 */
UCLASS()
class CGSI_GAMEPROTOTYPE_API UMovementStats : public UDataAsset
{
	GENERATED_BODY()
public:
	
	/* Camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraSensitivityX = .7f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraSensitivityY = .7f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	bool bInvertY = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float DefaultCamZOffset = 60.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	bool bUseTilt = true;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta=(EditCondition="bUseTilt"))
	float CameraTiltAngle = 2.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta=(EditCondition="bUseTilt"))
	float CameraTiltSpeed = 6.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraDefaultFOV = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera", meta=(tooltip="Used for Sliding, Wall Running etc."))
	float CameraExtendedFOV = 115.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraWallTiltAngle = 25.f;

	/* Movement */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkForwardSpeed = 1400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkRightMultiplier = .6f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxWalkableFloorAngle = 36.9999f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkBrakingDeceleration = 6000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float WalkAcceleration = 6000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultCapsuleHalfHeight = 88.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultGravityScale = 2.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultGroundFriction = 8.f;

	/* Fall */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fall")
	float FallRightMultiplier = .7f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fall")
	float FallMaxFrictionlessSpeed = 2500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fall")
	float AirControl = .65f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fall")
	float FallGravityScale = 3.5f;
	

	/* Jump */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta=(EditCondition="bCanJump"))
	float JumpZVelocity = 1250.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta=(EditCondition="bCanJump"))
	float JumpMaxBoostVelocity = 2000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta=(EditContition="bCanJump"))
	float JumpWalkBoost = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta=(EditContition="bCanJump"))
	float JumpCrouchBoost = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta=(EditContition="bCanJump"))
	float JumpSlideBoost = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump", meta=(EditContition="bCanJump"))
	float WallJumpForce = 1700.f;

	/* Double Jump */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump|DoubleJump", meta=(EditCondition="bCanDoubleJump"))
	float DoubleJumpZVelocity = 1250.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump|DoubleJump", meta=(EditCondition="bCanDoubleJump"))
	float DoubleJumpBoost = 100.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jump|DoubleJump", meta=(EditCondition="bCanDoubleJump"))
	float DoubleJumpAirControl = .9f;
	
	/* Crouch */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta=(EditCondition="bCanCrouch"))
	float CrouchForwardSpeed = 850.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta=(EditCondition="bCanCrouch"))
	float CrouchRightMultiplier = .7f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta=(EditCondition="bCanCrouch"))
	float CrouchDownSpeed = 4.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta=(EditCondition="bCanCrouch"))
	float CrouchFastDownSpeed = 10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta=(EditCondition="bCanCrouch"))
	float CrouchUpSpeed = 6.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta=(EditCondition="bCanCrouch"))
	float CrouchCapsuleHalfHeight = 40.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crouch", meta=(EditCondition="bCanCrouch"))
	float CrouchCamZOffset = 30.f;

	/* Slide */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta=(EditCondition="bCanSlide"))
	float SlideMaxSpeed = 2500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta=(EditCondition="bCanSlide"))
	float SlideEntryBoostAddedVelocity = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta=(EditCondition="bCanSlide"))
	float SlideMaxEntryBoostVelocity = 1800.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta=(EditCondition="bCanSlide"))
	float SlideSlopeAcceleration = 3500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta=(EditCondition="bCanSlide"))
	float SlideSlopeDeceleration = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta=(EditCondition="bCanSlide", advancedDisplay=true))
	float SlideBrakingDeceleration = 1000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slide", meta=(EditCondition="bCanSlide", advancedDisplay=true))
	float SlideExitFOVSpeed = 1.5f;

	/* Wall Running */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallJumpUpForce = 1200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallMinDistanceToGround = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallTraceDistance = 150.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallDetectionAngleOffset = -65.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallMinTiltAngle = -20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallMaxTiltAngle = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallDetectionHeightVariation = 80.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallConnectImpulse = 40000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallEnterFOVSpeed = .8f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallRunDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallMinGravityScale = -.8f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallMaxVelocity = 2800.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallAttractionForce = 100000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallMoveSpeed = 100000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallBanResetTime = 3.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Running", meta=(EditCondition="bCanWallRun"))
	float WallExitCrouchRepelForce = 100000.f;
	
};
