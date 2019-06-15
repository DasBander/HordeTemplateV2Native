

#include "HordeGameMode.h"
#include "HordeTemplateV2Native.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeBaseController.h"
#include "Gameplay/HordeGameState.h"
#include "AI/AISpawnPoint.h"
#include "Gameplay/HordePlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Character/BaseSpectator.h"
#include "HUD/HordeBaseHUD.h"
#include "Gameplay/HordeWorldSettings.h"
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

void AHordeGameMode::GetAISpawner(TArray<AActor*>& Spawner, int32& FreePoints)
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
	AHordeWorldSettings* WS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings());
	if (WS)
	{
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS && WS->MatchMode == EMatchMode::EMatchModeNonLinear)
		{
			if (ZedsLeftToSpawn == 0 && (GS->GameRound >= WS->MaxRounds))
			{
				GS->EndGame(false);
			}
			else 
			{
				if (GS->CountAliveZeds() == 0 && ZedsLeftToSpawn == 0)
				{
					GS->EndGameRound();
				}
			}

		}
	}
}



void AHordeGameMode::SpawnSpectator(APlayerController* PC)
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetSpectatorSpawnLocation());
	FActorSpawnParameters SpawnParam;
	ABaseSpectator* Spectator = GetWorld()->SpawnActor<ABaseSpectator>(ABaseSpectator::StaticClass(), SpawnTransform, SpawnParam);
	if (Spectator && PC)
	{
		PC->Possess(Spectator);
	}
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->AllPlayerDeadCheck();
	}
}

FVector AHordeGameMode::GetSpectatorSpawnLocation()
{
	TArray<FVector> LocalLocations;
	for (TActorIterator<AHordeBaseCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{

		AHordeBaseCharacter* AliveCharacter = *ActorItr;
		if (AliveCharacter && !AliveCharacter->GetIsDead())
		{
			LocalLocations.Add(AliveCharacter->GetActorLocation() + FVector(0.f, 0.f, 50.f));
		}
	}
	if (LocalLocations.Num() > 0)
	{
		return LocalLocations[FMath::RandRange(0, LocalLocations.Num() - 1)];
	}
	else
	{
		return GetRandomPlayerSpawn().GetLocation();
	}
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
			UDataTable* DTCharacters = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, CHARACTER_DATATABLE_PATH));
			if (DTCharacters) {
				FPlayableCharacter * Char = DTCharacters->FindRow<FPlayableCharacter>(PLY.SelectedCharacter, TEXT("GM Character DataTable"), true);
				if (Char && Char->CharacterID != "None")
				{
					ACharacter* Character = GetWorld()->SpawnActorDeferred<ACharacter>(Char->CharacterClass, FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
					if (Character)
					{
						Character->FinishSpawning(GetRandomPlayerSpawn(), false, nullptr);
					}
					PC->Possess(Character);
				}
				else {
					//If Character not found, Spawn Default Pawn.
					ACharacter* Character = GetWorld()->SpawnActorDeferred<ACharacter>(AHordeBaseCharacter::StaticClass(), FTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
					if (Character)
					{
						Character->FinishSpawning(GetRandomPlayerSpawn(), false, nullptr);
					}
					PC->Possess(Character);
				}
			}

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
				AAISpawnPoint* Spawner = Cast<AAISpawnPoint>(FreeSpawnPoints[FMath::RandRange(0, FreeSpawnPoints.Num())]);
				if (Spawner)
				{
					NewZed->PatrolTag = Spawner->PatrolTag;
					ZedsLeftToSpawn--;
					NewZed->FinishSpawning(Spawner->GetActorTransform(), false, nullptr);
				}
				else {
					NewZed->Destroy();
				}

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

