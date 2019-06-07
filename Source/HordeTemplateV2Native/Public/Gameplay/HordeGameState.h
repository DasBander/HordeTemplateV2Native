

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

	virtual void BeginPlay() override;
	
	void TakePlayer(FPlayerInfo Player);

	void UpdatePlayerLobby();

	void PopMessage(FChatMessage Message);

	FName GetFreeCharacter();
};
