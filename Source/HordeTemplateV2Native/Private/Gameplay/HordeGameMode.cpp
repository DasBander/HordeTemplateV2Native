

#include "HordeGameMode.h"
#include "HordeTemplateV2Native.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeBaseController.h"
#include "Gameplay/HordeGameState.h"
#include "AI/AISpawnPoint.h"
#include "Gameplay/HordePlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "HordeGameSession.h"
#include "Character/BaseSpectator.h"
#include "HUD/HordeBaseHUD.h"
#include "Gameplay/HordeWorldSettings.h"
#include "AI/ZedPawn.h"
#include "Runtime/Engine/Public/EngineUtils.h"

/*
	FUNCTION: Constructor for AHordeGameMode
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for AHordeGameMode
*/
AHordeGameMode::AHordeGameMode()
{
	GameStateClass = AHordeGameState::StaticClass();
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AHordeBaseController::StaticClass();
	PlayerStateClass = AHordePlayerState::StaticClass();
	HUDClass = AHordeBaseHUD::StaticClass();
	GameSessionClass = AHordeGameSession::StaticClass();

	bUseSeamlessTravel = true;
	bStartPlayersAsSpectators = 0;
}

/*
	FUNCTION: Get AI Spawner
	PARAM: None
	RETURN: void, TArray - AActor ( Spawner ), int32 ( Free Points )
	DESC:
	Returns Free AI Spawner Actors and the amount of Free Points.
*/
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
	FreePoints = FreeSpawnPoints;
}


/*
	FUNCTION: Check Game Over
	PARAM: None
	RETURN: void
	DESC:
	Checks if all Zombies are dead. If yes it should end the game or end the current game round.
*/
void AHordeGameMode::CheckGameOver()
{
	AHordeWorldSettings* WS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings());
	if (WS)
	{
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS && WS->MatchMode == EMatchMode::EMatchModeNonLinear)
		{
			if (GS->ZedsLeft == 0 && (GS->GameRound >= WS->MaxRounds) && ZedsLeftToSpawn == 0)
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


/*
	FUNCTION: Spawn Spectator
	PARAM: APlayerController ( Player Controller )
	RETURN: void
	DESC:
	Spawns an Spectator Pawn and lets the Player Controller possess with.
*/
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

/*
	FUNCTION: Get Spectator Spawn Location
	PARAM: None
	RETURN: FVector ( Random Spawn Location )
	DESC:
	Returns a Random Spawn Location depending on players that are still alive.
*/
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

/*
	FUNCTION: Get Controller By ID
	PARAM: FString ( Player ID )
	RETURN: APlayerController ( Player Controller Owned by Player ID )
	DESC:
	Returns Player Controller Object owned by the given Player ID.
*/
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

/*
	FUNCTION: Get Random Player Spawn
	PARAM: None
	RETURN: FTransform - Player Start Location
	DESC:
	Returns Random Location of a Spawn Point placed inside the world.
*/
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

/*
	FUNCTION: Logout
	PARAM: AController ( Exiting Player Controller )
	RETURN: void
	DESC:
	Updates Player Lobby when a Player Exits the game.
*/
void AHordeGameMode::Logout(AController* Exiting)
{
	FTimerHandle DelayedRemove;

	GetWorld()->GetTimerManager().SetTimer(DelayedRemove, this, &AHordeGameMode::UpdatePlayerLobby, 1.f, false);

}

/*
	FUNCTION: Update Player Lobby
	PARAM: None
	RETURN: void
	DESC:
	Runs Update Player Lobby inside Game State.
*/
void AHordeGameMode::UpdatePlayerLobby()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->UpdatePlayerLobby();
	}
}

/*
	FUNCTION: Game Start
	PARAM: TArray - FPlayerInfo ( Lobby Players )
	RETURN: void
	DESC:
	Spawns all Players inside World and possesses them with given Player Controller.
*/
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

/*
	FUNCTION: Initiate Zombie Spawning
	PARAM: int32 ( Amount )
	RETURN: void
	DESC:
	Starts Zombie Spawning with given amount. If no spawn point left we try to spawn until one is left.
*/
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
				AAISpawnPoint* Spawner = Cast<AAISpawnPoint>(FreeSpawnPoints[FMath::RandRange(0, FreeSpawnPoints.Num() -1)]);
				if (Spawner)
				{
					NewZed->PatrolTag = Spawner->PatrolTag;
					ZedsLeftToSpawn--;
					FTransform SpawnTrans;
					SpawnTrans.SetLocation(Spawner->GetActorLocation());
					NewZed->FinishSpawning(SpawnTrans, false, nullptr);
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

