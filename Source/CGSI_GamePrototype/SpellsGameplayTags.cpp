// Copyright (c) 2024 Nic. All Rights Reserved.

#include "SpellsGameplayTags.h"

namespace SpellTags
{
	
	UE_DEFINE_GAMEPLAY_TAG(Shape_NoMatch, "Shape.NoMatch");
	UE_DEFINE_GAMEPLAY_TAG(Shape_NotEnoughPoints, "Shape.NotEnoughPoints");

	UE_DEFINE_GAMEPLAY_TAG(Shape_Circle, "Shape.Circle");
	UE_DEFINE_GAMEPLAY_TAG(Shape_Square, "Shape.Square");
	UE_DEFINE_GAMEPLAY_TAG(Shape_Triangle, "Shape.Triangle");
	UE_DEFINE_GAMEPLAY_TAG(Shape_Star, "Shape.Star");
	UE_DEFINE_GAMEPLAY_TAG(Shape_Heart, "Shape.Heart");

	UE_DEFINE_GAMEPLAY_TAG(SpellState_Drawing, "SpellState.Drawing");
	

	UE_DEFINE_GAMEPLAY_TAG(MovementState_Disabled, "MovementState.Disabled");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Walk, "MovementState.Walk");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Fall, "MovementState.Fall");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Jump, "MovementState.Jump");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Crouch, "MovementState.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Slide, "MovementState.Slide");
	UE_DEFINE_GAMEPLAY_TAG(MovementState_Wallrun, "MovementState.Wallrun");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Draw, "Ability.Draw");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Type_Shape, "Ability.Type.Shape");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Jump, "Ability.Movement.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Jump_DoubleJump, "Ability.Movement.Jump.DoubleJump");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Crouch, "Ability.Movement.Crouch");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Crouch_Slide, "Ability.Movement.Crouch.Slide");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Movement_Wallrun, "Ability.Movement.Wallrun");

	UE_DEFINE_GAMEPLAY_TAG(AttributeEffect_InkReduction, "AttributeEffect.InkReduction");

	UE_DEFINE_GAMEPLAY_TAG(Event_CommitShape, "Event.CommitShape");
	
}
