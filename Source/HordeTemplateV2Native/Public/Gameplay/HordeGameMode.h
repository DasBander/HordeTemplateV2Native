

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyStructures.h"
#include "HordeGameMode.generated.h"


UENUM(BlueprintType)
enum class EMatchMode : uint8
{
	EMatchModeLinear UMETA(DisplayName = "Linear"),
	EMatchModeNonLinear UMETA(DisplayName = "Nonlinear")
};


/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	int32 ZedsLeftToSpawn = 0;


public:
	AHordeGameMode();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zeds")
	void GetAISpawner(TArray<AActor*>& Spawner, int32 &FreePoints);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Zeds")
		AActor* GetFreeAISpawnPoint();

	UFUNCTION()
		void CheckGameOver();

	APlayerController* GetControllerByID(FString PlayerID);

	FTransform GetRandomPlayerSpawn();

	virtual void Logout(AController* Exiting) override;

	void GameStart(const TArray<FPlayerInfo>& LobbyPlayers);

	void InitiateZombieSpawning(int32 Amount);
};
