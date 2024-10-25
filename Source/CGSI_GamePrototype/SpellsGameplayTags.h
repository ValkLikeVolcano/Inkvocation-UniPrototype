// Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace SpellTags
{
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shape_NoMatch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shape_NotEnoughPoints);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shape_Circle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shape_Square);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shape_Triangle);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shape_Star);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shape_Heart);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpellState_Drawing);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Disabled);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Walk);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Fall);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Slide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MovementState_Wallrun);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Draw);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Type_Shape);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Jump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Jump_DoubleJump);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Crouch);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Crouch_Slide);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Movement_Wallrun);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AttributeEffect_InkReduction);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_CommitShape);
	
}


