

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerScoreboardWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	void UpdatePlayerList(const TArray<APlayerState*>& PlayerList);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player List")
		void OnPlayerListUpdated(const TArray<APlayerState*>& PlayerList);

	UFUNCTION(BlueprintPure, Category = "Player List")
		FText GetLobbyName();
};
