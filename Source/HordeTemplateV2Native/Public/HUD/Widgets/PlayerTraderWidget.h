

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTraderWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerTraderWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintPure, Category = "Economy")
		FText GetPlayerMoney();
};
