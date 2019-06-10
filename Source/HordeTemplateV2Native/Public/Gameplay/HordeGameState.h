

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyStructures.h"
#include "HordeGameState.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeGameState : public AGameState
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby")
		FLobbyInfo LobbyInformation;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby")
		EGameStatus GameStatus;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby")
		bool GameStarting = false;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby")
		bool BlockDisconnect = false;

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
		TArray<FPlayerInfo> LobbyPlayers;

	virtual void BeginPlay() override;
	
	UFUNCTION()
	void TakePlayer(FPlayerInfo Player);

	UFUNCTION()
	void StartLobbyTimer();

	UFUNCTION()
		void ProcessLobbyTime();

	UFUNCTION()
		void ResetLobbyTime();

	UFUNCTION()
		void UnreadyAllPlayers();

	UFUNCTION()
		void StartGame();

	UFUNCTION()
		void FreeupUnassignedCharacters();

	UFUNCTION()
	FString GetUsernameBySteamID(FString ID, bool& FoundPlayer);

	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	FTimerHandle LobbyTimer;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Lobby")
		float LobbyTime = 0.f;

	UFUNCTION()
	void UpdatePlayerLobby();

	UFUNCTION()
	bool CheckPlayersReady();

	UFUNCTION()
	void PopMessage(FChatMessage Message);

	UFUNCTION()
	FName GetFreeCharacter();
};
