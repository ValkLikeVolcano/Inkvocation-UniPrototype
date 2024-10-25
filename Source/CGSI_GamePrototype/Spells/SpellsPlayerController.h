// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Paint/UnistrokeRecognizer.h"
#include "SpellsPlayerController.generated.h"

class USpellsAbilitySystemComponent;
class UGameplayEffect;
class USpellsAttributeSet;
class UTrainWidget;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class USpellsPaintWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInkChangedSignature, float, newValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMaxInkChangedSignature, float, newValue);

UENUM()
enum EPaintAction : uint8
{
	Idle,
	Paint,
	Recognize,
	Train
};

UCLASS()
class CGSI_GAMEPROTOTYPE_API ASpellsPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(Exec)
	void TrainMode(const bool Is);
	void ShowTrainWidget();
	void HideTrainWidget();
	void AddTemplateToDataTable(const FGameplayTag& ShapeTag);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TObjectPtr<UInputAction> PaintAction;

	UFUNCTION(BlueprintCallable, Category="Spells")
	USpellsAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	UFUNCTION(BlueprintCallable, Category="Spells")
	USpellsAttributeSet* GetAttributeSet() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spells");
	TSubclassOf<UGameplayEffect> InkReductionEffect;

	UPROPERTY(BlueprintAssignable)
	FInkChangedSignature OnInkChanged;
	UPROPERTY(BlueprintAssignable)
	FMaxInkChangedSignature OnMaxInkChanged;

public:
	ASpellsPlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Paint")
	TObjectPtr<UDataTable> UnistrokeTemplatesTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Paint")
	TSubclassOf<USpellsPaintWidget> PaintWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Paint|Train")
	TSubclassOf<UUserWidget> TrainWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinDrawDistance = 1500.0f;

private:
	void Input_Paint(const FInputActionValue& Value);
	void LoadTemplates();
	void Spell();

private:
	UPROPERTY()
	TObjectPtr<USpellsPaintWidget> PaintWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> TrainWidget;
	
	FUnistrokeRecognizer* Recognizer;
	
	int CurrentAction;
	bool IsTraining;

	float TotalDistanceDrawn = 0.0f;
};
