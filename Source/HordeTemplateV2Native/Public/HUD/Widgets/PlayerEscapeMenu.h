

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerEscapeMenu.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerEscapeMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Escape Menu")
		void DisconnectFromServer();

	UFUNCTION(BlueprintCallable, Category = "Escape Menu")
		void CloseEscapeMenu();
};
