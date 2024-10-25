// // Copyright (c) 2024 Nic. All Rights Reserved.


#include "TrainWidget.h"

#include "GameplayTagsManager.h"
#include "CGSI_GamePrototype/Spells/SpellsPlayerController.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UTrainWidget::AddNewTemplate()
{
	if (!ensure(SpellsPC != nullptr)) return;
	const FString Name = NameTextBox->GetText().ToString();
	// Attempt to convert the name string into a gameplay tag
	FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*Name), true);

	if (Tag.IsValid()) 
	{
		SpellsPC->AddTemplateToDataTable(Tag); 
	} 
}

void UTrainWidget::ClearPoints()
{
	if (!ensure(SpellsPC != nullptr)) return;
	SpellsPC->HideTrainWidget();
}

void UTrainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController != nullptr)
	{
		SpellsPC = Cast<ASpellsPlayerController>(PlayerController);
	}
	
	AddButton->OnClicked.AddUniqueDynamic(this, &UTrainWidget::AddNewTemplate);
	ClearButton->OnClicked.AddUniqueDynamic(this, &UTrainWidget::ClearPoints);
}
