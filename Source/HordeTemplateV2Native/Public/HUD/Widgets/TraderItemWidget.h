

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Misc/HordeTrader.h"
#include "TraderItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UTraderItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn = "true"))
		FTraderSellItem TraderItem;

	UFUNCTION(BlueprintCallable, Category="Trading")
		void BuyItem();

	UFUNCTION(BlueprintPure, Category = "Trading")
		FText GetPriceText();

	UFUNCTION(BlueprintPure, Category = "Trading")
		bool HasEnoughMoney();
};
