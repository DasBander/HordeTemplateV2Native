

#include "HordeGameMode.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeBaseController.h"
#include "Gameplay/HordeGameState.h"
#include "AI/AISpawnPoint.h"
#include "Gameplay/HordePlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "HUD/HordeBaseHUD.h"
#include "AI/ZedPawn.h"
#include "Runtime/Engine/Public/EngineUtils.h"

AHordeGameMode::AHordeGameMode()
{
	GameStateClass = AHordeGameState::StaticClass();
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AHordeBaseController::StaticClass();
	PlayerStateClass = AHordePlayerState::StaticClass();
	HUDClass = AHordeBaseHUD::StaticClass();

	bUseSeamlessTravel = true;
	bStartPlayersAsSpectators = 0;
}

void AHordeGameMode::GetAISpawner(TArray<AActor*>& Spawner, int32 &FreePoints)
{
	int32 FreeSpawnPoints = 0;
	TArray<AActor*> TempSpawner;
	for (TActorIterator<AAISpawnPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
	
		AAISpawnPoint* SpawnPoint = *ActorItr;
		if (SpawnPoint && !SpawnPoint->SpawnNotFree)
		{
			FreeSpawnPoints++;
			TempSpawner.Add(SpawnPoint);
		}
	}
	Spawner = TempSpawner;
	FreePoints = FreePoints;
}

AActor* AHordeGameMode::GetFreeAISpawnPoint()
{
	return nullptr;
}

void AHordeGameMode::CheckGameOver()
{

}

APlayerController* AHordeGameMode::GetControllerByID(FString PlayerID)
{
	APlayerController* TempCTRL = nullptr;
	for (TActorIterator<APlayerController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APlayerController* APC = *ActorItr;
		if (APC)
		{
			if (APC->PlayerState->UniqueId->ToString() == PlayerID)
			{
				TempCTRL = APC;
			}
		}
	}
	return TempCTRL;
}

FTransform AHordeGameMode::GetRandomPlayerSpawn()
{
	TArray<FTransform> SpawnPoints;
	for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{

		APlayerStart* SpawnPoint = *ActorItr;
		if (SpawnPoint)
		{
			SpawnPoints.Add(SpawnPoint->GetActorTransform());
		}
	}
	return SpawnPoints[FMath::RandRange(0, (SpawnPoints.Num() - 1))];
}

void AHordeGameMode::Logout(AController* Exiting)
{
	FTimerHandle DelayedRemove;
	FTimerDelegate DelayedRemoveDel;
	DelayedRemoveDel.BindLambda([=] {
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			GS->UpdatePlayerLobby();
		}
	});
	GetWorld()->GetTimerManager().SetTimer(DelayedRemove, DelayedRemoveDel, 1.f, false);

}

void AHordeGameMode::GameStart(const TArray<FPlayerInfo>& LobbyPlayers)
{
	for (auto PLY : LobbyPlayers)
	{
		APlayerController* PC = GetControllerByID(PLY.PlayerID);
		if (PC)
		{
			AHordeBaseCharacter* Character = GetWorld()->SpawnActorDeferred<AHordeBaseCharacter>(AHordeBaseCharacter::StaticClass(), FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (Character)
			{
				Character->FinishSpawning(GetRandomPlayerSpawn(), false, nullptr);
			}
			PC->Possess(Character);
		}
	}
}

void AHordeGameMode::InitiateZombieSpawning(int32 Amount)
{
	ZedsLeftToSpawn = Amount;
	TArray<AActor*> FreeSpawnPoints;
	int32 AmountOfFreePoints = 0;
	GetAISpawner(FreeSpawnPoints, AmountOfFreePoints);
	if (ZedsLeftToSpawn > 0)
	{
		for (int32 i = 0; i < ((Amount > AmountOfFreePoints) ? AmountOfFreePoints : Amount); i++)
		{
			AZedPawn* NewZed = GetWorld()->SpawnActorDeferred<AZedPawn>(AZedPawn::StaticClass(), FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (NewZed)
			{
				ZedsLeftToSpawn--;
				NewZed->FinishSpawning(FreeSpawnPoints[FMath::RandRange(0, FreeSpawnPoints.Num())]->GetActorTransform(), false, nullptr);
			}
		}
		if (ZedsLeftToSpawn > 0)
		{
			FTimerHandle RestartSpawningHandle;
			FTimerDelegate RestartSpawningDelegate;
			RestartSpawningDelegate.BindLambda([=] {
				InitiateZombieSpawning(ZedsLeftToSpawn);
			});
			GetWorld()->GetTimerManager().SetTimer(RestartSpawningHandle, RestartSpawningDelegate, 3.f, false);
		}
	}
}

