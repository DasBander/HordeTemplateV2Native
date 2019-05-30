

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHeadDisplay.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerHeadDisplay : public UUserWidget
{
	GENERATED_BODY()

		DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShowWidget);
		DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideWidget);

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
		void OnShowWidget();

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
		void OnHideWidget();

	UPROPERTY(BlueprintReadOnly, Category = "Player Head Display")
		FOnShowWidget OnShowWidgetDelegate;

	UPROPERTY(BlueprintReadOnly, Category = "Player Head Display")
		FOnShowWidget OnHideWidgetDelegate;

	UPROPERTY(BlueprintReadOnly, Category = "Player Head Display")
		FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Player Head Display")
		float Health;
	
};
