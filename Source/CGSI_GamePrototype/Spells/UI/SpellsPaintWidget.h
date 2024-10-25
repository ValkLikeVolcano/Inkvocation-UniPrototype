// // Copyright (c) 2024 Nic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "SpellsPaintWidget.generated.h"

struct FLineSegment
{
	FVector2D StartPoint;
	FVector2D EndPoint;
	float TimeDrawn; 
};

/**
 * 
 */
UCLASS()
class CGSI_GAMEPROTOTYPE_API USpellsPaintWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	TArray<FVector2D> GetPoints() const { return Points; }
	void AddPoint(const FVector2D &Point);
	void RemoveAllPoints();

	void AnimateShape(const FGameplayTag& ShapeTag);

protected:
	virtual int32 NativePaint(const FPaintArgs &Args, const FGeometry &AllottedGeometry, const FSlateRect &MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle &InWidgetStyle, bool bParentEnabled) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Appearance")
	FLinearColor StartColor = FLinearColor::Blue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Appearance")
	FLinearColor EndColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Appearance")
	bool bFadeOut = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Appearance")
	float StartThickness = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Appearance")
	float EndThickness = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line Appearance", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float LineDuration = 2.0f;
	
private:
	TArray<FVector2D> Points;

	TArray<FLineSegment> LineSegments;
};
