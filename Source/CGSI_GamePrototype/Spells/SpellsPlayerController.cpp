// // Copyright (c) 2024 Nic. All Rights Reserved.


#include "SpellsPlayerController.h"

#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/SpellsPaintWidget.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SpellsCharacter.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Paint/UnistrokeDataTable.h"
#include "AbilitySystemGlobals.h" 
#include "VectorTypes.h"
#include "AbilitySystem/SpellsAbilitySystemComponent.h"
#include "AbilitySystem/SpellsAttributeSet.h"
#include "CGSI_GamePrototype/SpellsGameplayTags.h"

USpellsAbilitySystemComponent* ASpellsPlayerController::GetAbilitySystemComponent() const
{
	return Cast<USpellsAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()));
}

USpellsAttributeSet* ASpellsPlayerController::GetAttributeSet() const
{
	ASpellsCharacter* SpellChar = CastChecked<ASpellsCharacter>(GetPawn());
	return SpellChar->GetAttributeSet();
}

ASpellsPlayerController::ASpellsPlayerController()
{
	Recognizer = new FUnistrokeRecognizer();
}

void ASpellsPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	check(UnistrokeTemplatesTable);
	LoadTemplates();

	PaintWidget = CreateWidget<USpellsPaintWidget>(this, PaintWidgetClass);
	if(IsValid(PaintWidget))
	{
		PaintWidget->AddToViewport();
		PaintWidget->SetVisibility(ESlateVisibility::Visible);
	}
	
	// Create Train Widget allow train only in editor
	TrainWidget = CreateWidget<UUserWidget>(this, TrainWidgetClass);
	if (TrainWidget != nullptr)
	{
		TrainWidget->AddToViewport();
		HideTrainWidget();
	}
	
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	IsTraining = false;

	CurrentAction = EPaintAction::Idle;

	//Broadcast Initials
	OnInkChanged.Broadcast(GetAttributeSet()->GetInk());
	OnMaxInkChanged.Broadcast(GetAttributeSet()->GetInkMax());
	
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetInkAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnInkChanged.Broadcast(Data.NewValue);
		});
	GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(GetAttributeSet()->GetInkMaxAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxInkChanged.Broadcast(Data.NewValue);
		});
}

void ASpellsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(IMC, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
	check(Input);

	Input->BindAction(PaintAction, ETriggerEvent::Triggered, this, &ASpellsPlayerController::Input_Paint);
}

void ASpellsPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentAction == EPaintAction::Paint)
	{
		// Get ink attribute from player / if its empty
		ASpellsCharacter* SpellChar = CastChecked<ASpellsCharacter>(GetPawn());
		if (SpellChar->GetAttributeSet()->GetInk() <= 0)
		{
			Input_Paint(false);
			return;
		}
		
		const TArray<FVector2D> Points = PaintWidget->GetPoints();
		const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
		const FVector2D LastPoint = Points.Num() > 0 ? Points.Last() : MousePosition;
		const bool IsNewPoint = !LastPoint.Equals(MousePosition, 1.0f);

		if (Points.Num() == 0 || (Points.Num() > 0 && IsNewPoint))
		{
			PaintWidget->AddPoint(MousePosition);

			float Distance = FVector2D::Distance(LastPoint, MousePosition);
			TotalDistanceDrawn += Distance;

			UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
			
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle InkReductionGE = ASC->MakeOutgoingSpec(InkReductionEffect, 1.0f, EffectContext);
			InkReductionGE.Data.Get()->SetSetByCallerMagnitude(SpellTags::AttributeEffect_InkReduction, -Distance * 0.01f);
			ASC->ApplyGameplayEffectSpecToSelf(*InkReductionGE.Data.Get());
		}
	}
}

void ASpellsPlayerController::Input_Paint(const FInputActionValue& Value)
{
	bool bPressed = Value.Get<bool>();

	if(bPressed)
	{
		TotalDistanceDrawn = 0.0f;
		
		if ( !UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn())->HasMatchingGameplayTag(SpellTags::Ability_Draw))
		{
			return;
		}
		
		if(CurrentAction != EPaintAction::Train)
		{
			CurrentAction = EPaintAction::Paint;
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn())->SetLooseGameplayTagCount(SpellTags::SpellState_Drawing, 1);
			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			int32 ViewportSizeX, ViewportSizeY;
			GetViewportSize(ViewportSizeX, ViewportSizeY);
			// Calculate center coordinates
			float CenterX = ViewportSizeX / 2.0f;
			float CenterY = ViewportSizeY / 4.0f;

			SetMouseLocation(CenterX, CenterY);
			bShowMouseCursor = true;
		}
	}
	else
	{
		if(CurrentAction == EPaintAction::Paint)
		{
			CurrentAction = IsTraining ? EPaintAction::Train : EPaintAction::Recognize;
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn())->SetLooseGameplayTagCount(SpellTags::SpellState_Drawing, 0);
			
			if(!IsTraining)
			{
				FInputModeGameOnly InputMode;
				SetInputMode(InputMode);
				bShowMouseCursor = false;
			}
		}

		switch (CurrentAction)
		{
		case EPaintAction::Recognize:
			Spell();
			break;
		case EPaintAction::Train:
			if(PaintWidget->GetPoints().Num() > 2)
			{
				ShowTrainWidget();
			}
			break;
		default:
			break;
		}
	}
}

void ASpellsPlayerController::Spell()
{
	
	TArray<FVector2D>CurrentPoints = PaintWidget->GetPoints();
	FUnistrokeResult Result = Recognizer->Recognize(CurrentPoints, false);

	if (Result.Score > 0.8f && TotalDistanceDrawn >= MinDrawDistance)
	{
		USpellsAbilitySystemComponent* ASC =  Cast<USpellsAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()));
		ASC->ShapeRecognized(Result);
	}

	PaintWidget->AnimateShape(Result.ShapeTag);
	CurrentAction = EPaintAction::Idle;
}


void ASpellsPlayerController::LoadTemplates()
{
	if (UnistrokeTemplatesTable != nullptr)
	{
		const FString ContextString = "Templates";
		TArray<FUnistrokeDataTable*> Rows;

		UnistrokeTemplatesTable->GetAllRows<FUnistrokeDataTable>(ContextString, Rows);

		for (int i = 0; i < Rows.Num(); i++)
		{
			Recognizer->AddTemplate((*Rows[i]).ShapeTag, (*Rows[i]).Points);
		}
	}
}

/// Train Mode
void ASpellsPlayerController::TrainMode(const bool Is)
{
	IsTraining = Is;

	if (IsTraining)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Train mode is ON");
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Train mode is OFF");
	}
}

void ASpellsPlayerController::ShowTrainWidget()
{
	if (TrainWidget != nullptr)
	{
		TrainWidget->SetVisibility(ESlateVisibility::Visible);
		CurrentAction = EPaintAction::Train;
	}
}

void ASpellsPlayerController::HideTrainWidget()
{
	if (TrainWidget != nullptr)
	{		
		PaintWidget->RemoveAllPoints();
		TrainWidget->SetVisibility(ESlateVisibility::Hidden);
		CurrentAction = EPaintAction::Idle;
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;
}

void ASpellsPlayerController::AddTemplateToDataTable(const FGameplayTag& ShapeTag)
{
	FUnistrokeDataTable DataRow;
	
	DataRow.ShapeTag = ShapeTag;
	DataRow.Points = PaintWidget->GetPoints();
	
	const FString ContextString = "Templates";
	TArray<FUnistrokeDataTable*> Rows;
	
	UnistrokeTemplatesTable->GetAllRows<FUnistrokeDataTable>(ContextString, Rows);
	
	int RowNum = Rows.Num() + 1;
	
	UnistrokeTemplatesTable->AddRow(FName(*FString::FromInt(RowNum)), DataRow);
	
	Recognizer->AddTemplate(ShapeTag, PaintWidget->GetPoints());		
	
	HideTrainWidget();

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "New Gesture was added");
}
