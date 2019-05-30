

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/GameplayStructures.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryHelpers.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UInventoryHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UInventoryHelpers();


	UFUNCTION(BlueprintPure, Category = "Inventory|Helper")
		static FItem FindItemByID(FName ItemID);
};
