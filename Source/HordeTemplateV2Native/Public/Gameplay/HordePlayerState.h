

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

	FORCEINLINE void SwitchCharacter(FName Character)
	{
		Player.SelectedCharacter = Character;
	}

	UFUNCTION(Client, Reliable)
		void OnMessageReceived(FChatMessage Msg);

	UFUNCTION(Client, Reliable)
		void UpdateLobbyPlayerList(const TArray<FPlayerInfo>& Players);

protected:

	UFUNCTION(Client, Reliable)
		void ClientUpdateGameStatus(EGameStatus GameStatus);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Lobby")
		void ToggleReadyStatus();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Lobby|Trade")
		void RequestCharacterTrade(const FString& InstigatorPlayer, const FString& TargetPlayer);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Lobby|Trade")
		void AcceptCharacterTrade();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Lobby|Trade")
		void CancelCharacterTrade();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Player Info")
		FPlayerInfo Player;

	virtual void BeginPlay() override;

};
