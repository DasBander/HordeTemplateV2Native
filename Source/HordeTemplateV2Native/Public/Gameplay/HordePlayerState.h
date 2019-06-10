

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyStructures.h"
#include "HordePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	FORCEINLINE FPlayerInfo GetPlayerInfo() {
		return Player;
	}

	FORCEINLINE void SwitchReady(bool State)
	{
		Player.PlayerReady = State;
	}

	UFUNCTION(Client, Reliable)
		void OnMessageReceived(FChatMessage Msg);

	UFUNCTION(Client, Reliable)
		void UpdateLobbyPlayerList(const TArray<FPlayerInfo>& Players);

protected:

	UFUNCTION(Client, Reliable)
		void ClientUpdateGameStatus(EGameStatus GameStatus);



	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Player Info")
		FPlayerInfo Player;

	virtual void BeginPlay() override;

};
