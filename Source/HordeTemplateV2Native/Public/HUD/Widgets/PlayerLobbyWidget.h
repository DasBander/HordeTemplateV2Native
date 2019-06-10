

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/LobbyStructures.h"
#include "PlayerLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyPlayersUpdated, const TArray<FPlayerInfo>&, LobbyPlayers);

protected:
		virtual void NativeConstruct() override;

	

public:
	UPROPERTY()
		FOnLobbyPlayersUpdated OnLobbyPlayersUpdateDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
		FLobbyInfo LobbyInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby")
		FPlayableLevel MapInfo;

	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
		void OnLobbyPlayersUpdated(const TArray<FPlayerInfo>& LobbyPlayers);

	UFUNCTION(BlueprintPure, Category = "Lobby")
		FText GetLobbyTime();
	
};
