// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrainWidget.generated.h"

class ASpellsPlayerController;
/**
 * 
 */
UCLASS()
class CGSI_GAMEPROTOTYPE_API UTrainWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* AddButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ClearButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* NameTextBox;

protected:
	UFUNCTION()
	void AddNewTemplate();

	UFUNCTION()
	void ClearPoints();

protected:
	void NativeConstruct() override;
	
private:
	TObjectPtr<ASpellsPlayerController> SpellsPC;
};
