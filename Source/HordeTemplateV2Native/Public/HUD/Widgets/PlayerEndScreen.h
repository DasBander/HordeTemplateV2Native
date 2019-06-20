

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerEndScreen.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerEndScreen : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintPure, Category = "End Screen")
		FText GetMVPName();

	UFUNCTION(BlueprintPure, Category = "End Screen")
		FText GetMHSName();

	UFUNCTION(BlueprintPure, Category = "End Screen")
		FText GetMKSName();

	UFUNCTION(BlueprintPure, Category = "End Screen")
		FText GetMVPPoints();

	UFUNCTION(BlueprintPure, Category = "End Screen")
		FText GetMHSPoints();

	UFUNCTION(BlueprintPure, Category = "End Screen")
		FText GetMKSPoints();

	UFUNCTION(BlueprintPure, Category = "End Screen")
		FText GetEndTime();

};
