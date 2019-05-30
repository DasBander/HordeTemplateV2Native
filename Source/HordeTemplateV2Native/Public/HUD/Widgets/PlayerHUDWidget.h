

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetInteractionText, const FText&, InteractionText);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideInteractionText);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBindingVariables);

protected:

	UPROPERTY()
		UMaterialInstance* CircleMaterialInstance;

public:

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnSetInteractionText OnSetInteractionText;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnHideInteractionText OnHideInteractionText;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
		FOnBindingVariables OnBindingVariables;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool IsInteracting = false;

	UPROPERTY()
		float InteractionValueToInterpolate = 0.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
		void SetInteractionText(const FText &InteractionText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
		void HideInteractionText();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
		void VariablesBound();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
		void HideInteractionTxt();

	UFUNCTION(BlueprintPure, Category = "Visibility")
		ESlateVisibility IsOwningCharacter();

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		UMaterialInstanceDynamic* ProgressCircleDynamic;


	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
};
