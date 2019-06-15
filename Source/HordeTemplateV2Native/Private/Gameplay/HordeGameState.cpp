

#include "HordeGameState.h"
#include "HordeTemplateV2Native.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Gameplay/HordeWorldSettings.h"
#include "Gameplay/HordeBaseController.h"
#include "Character/HordeBaseCharacter.h"
#include "AI/ZedPawn.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Misc/HordeTrader.h"

void AHordeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHordeGameState, LobbyInformation);
	DOREPLIFETIME(AHordeGameState, GameStatus);
	DOREPLIFETIME(AHordeGameState, GameStarting);
	DOREPLIFETIME(AHordeGameState, LobbyTime);
	DOREPLIFETIME(AHordeGameState, BlockDisconnect);
	DOREPLIFETIME(AHordeGameState, TradeProgress);
	DOREPLIFETIME(AHordeGameState, IsTradeInProgress);
	DOREPLIFETIME(AHordeGameState, RoundTime);
	DOREPLIFETIME(AHordeGameState, IsRoundPaused);
	DOREPLIFETIME(AHordeGameState, GameRound);
	DOREPLIFETIME(AHordeGameState, ZedsLeft);
	DOREPLIFETIME(AHordeGameState, Score_MVP);
	DOREPLIFETIME(AHordeGameState, Score_MostHeadshots);
	DOREPLIFETIME(AHordeGameState, Score_MostKills);
	DOREPLIFETIME(AHordeGameState, NextLevel);
	DOREPLIFETIME(AHordeGameState, EndTime);
}

void AHordeGameState::BeginPlay()
{
	if (HasAuthority())
	{
		GameStatus = EGameStatus::ELOBBY;
		
		AHordeWorldSettings* WorldSettings = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings());
		if (WorldSettings)
		{
			for (auto &Char : WorldSettings->PlayerCharacters)
			{
				FLobbyAvailableCharacters AChar;
				AChar.CharacterID = Char;
				LobbyInformation.AvailableCharacters.Add(AChar);
			}

			if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()) && !PlayerArray.IsValidIndex(0))
			{
				UDataTable* DTMaps = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, MAPS_DATATABLE_PATH));
				TArray<FPlayableLevel*> PlayableLevels;
				if (DTMaps) {
					DTMaps->GetAllRows<FPlayableLevel>(TEXT("Game State Init"), PlayableLevels);
				}
				for (auto PLevel : PlayableLevels)
				{
					LobbyInformation.LobbyMapRotation.Add(PLevel->RawLevelName);
					LobbyInformation.OwnerID = "Dedicated Server";
					LobbyInformation.LobbyName = "Horde Game - Lobby";
				}	
			}
			else
			{

			}
		}
		
		LobbyTime = LobbyInformation.DefaultLobbyTime;
	}
}

void AHordeGameState::TakePlayer(FPlayerInfo Player)
{
	if (!IsCharacterTaken(Player.SelectedCharacter))
	{
		TArray<FLobbyAvailableCharacters> Characters = LobbyInformation.AvailableCharacters;
		int32 CharacterIndex = -1;
		for (auto PChar : Characters)
		{
			CharacterIndex++;
			if (PChar.CharacterID == Player.SelectedCharacter)
			{
				break;
			}
		}
		if (Characters.IsValidIndex(CharacterIndex))
		{
			Characters[CharacterIndex].CharacterTaken = true;
			Characters[CharacterIndex].PlayerID = Player.PlayerID;
			Characters[CharacterIndex].PlayerUsername = Player.UserName;
			LobbyInformation.AvailableCharacters = Characters;
			UpdatePlayerLobby();
		}
	}
}

void AHordeGameState::KickPlayer(const FString& PlayerID)
{
	AHordePlayerState* PS = GetPlayerStateByID(PlayerID);
	if (PS)
	{
		PopMessage(FChatMessage(PS->GetPlayerInfo().UserName + " got kicked."));
		PS->GettingKicked();
	}
}

void AHordeGameState::StartRoundBasedGame()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(PauseTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(PauseTimer, this, &AHordeGameState::ProcessPauseTime, 1.f, true);
		IsRoundPaused = true;
		RoundTime = 0.f;
	}
}

void AHordeGameState::ProcessPauseTime()
{
	AHordeWorldSettings* WS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings(false, true));
	if (WS)
	{
		if (WS->PauseTime >= PauseTime)
		{
			PauseTime = 0.f;
			GameRound++;
			GetWorld()->GetTimerManager().ClearTimer(PauseTimer);
			IsRoundPaused = false;

			for (TActorIterator<AHordeBaseController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				AHordeBaseController* CTRL = *ActorItr;
				if (CTRL)
				{
					USoundCue* NewRoundSound = ObjectFromPath<USoundCue>(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_RoundWarmup_Cue.A_RoundWarmup_Cue'"));
					if (NewRoundSound)
					{
						CTRL->ClientPlay2DSound(NewRoundSound);
					}
				}

			}

			StartGameRound();
		}
		else 
		{
			PauseTime++;
		}
	}
}

void AHordeGameState::StartGameRound()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(RoundTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(RoundTimer, this, &AHordeGameState::ProcessRoundTime, 1.f, true);
	}

	AHordeGameMode* GMO = Cast<AHordeGameMode>(GetWorld()->GetAuthGameMode());
	if (GMO)
	{
		AHordeWorldSettings* HWS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings(false, true));
		if (HWS)
		{
			GMO->InitiateZombieSpawning(GameRound * HWS->ZedMultiplier);
		}
	}
	for (auto& PLY : PlayerArray)
	{
		AHordeBaseController* PC = Cast<AHordeBaseController>(PLY->GetOwner());
		if (PC)
		{
			PC->ClientCloseTraderUI();
		}
	}

	for (TActorIterator<AHordeTrader> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AHordeTrader* Trader = *ActorItr;
		if (Trader)
		{
			Trader->PlayGoodBye();
		}
		
	}
}

void AHordeGameState::ProcessRoundTime()
{
	AHordeWorldSettings* HWS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings(false, true));
	if (HWS)
	{
		if (HWS->RoundTime >= RoundTime)
		{
			EndGameRound();
		}
		else 
		{
			RoundTime++;
		}
	}
}

FName AHordeGameState::GetNextLevelInRotation(bool ResetLevel)
{
	FName NextLevel = *GetWorld()->GetMapName();
	if (!ResetLevel)
	{
		int32 CurLevelIndex = LobbyInformation.LobbyMapRotation.Find(NextLevel);
		if (CurLevelIndex >= (LobbyInformation.LobbyMapRotation.Num() - 1))
		{
			if (LobbyInformation.LobbyMapRotation.IsValidIndex(0))
			{
				NextLevel = LobbyInformation.LobbyMapRotation[0];
			}
		}
		else
		{
			if (LobbyInformation.LobbyMapRotation.IsValidIndex(CurLevelIndex + 1))
			{
				NextLevel = LobbyInformation.LobbyMapRotation[CurLevelIndex + 1];
			}
		}
	}
	return NextLevel;
}

void AHordeGameState::EndGameRound()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(RoundTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(RoundTimer);
	}
	StartGameRound();
}

void AHordeGameState::StartLobbyTimer()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(LobbyTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(LobbyTimer, this, &AHordeGameState::ProcessLobbyTime, 1.f, true);
	}
}

void AHordeGameState::ProcessLobbyTime()
{
	if (LobbyTime > 0.f)
	{
		LobbyTime--;
		if (FMath::RoundToInt(LobbyTime) <= 10 && FMath::RoundToInt(LobbyTime) > 0)
		{
			PopMessage(FChatMessage("Game starting in " + FString::FromInt(FMath::RoundToInt(LobbyTime)) + " seconds."));
			if (FMath::RoundToInt(LobbyTime) <= 5)
			{
				BlockDisconnect = true;
			}
		}
		else {
			if (FMath::RoundToInt(LobbyTime) == 0)
			{
				PopMessage(FChatMessage("Game starting...."));
			}
		}
	}
	else 
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(LobbyTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(LobbyTimer);
		}
		StartGame();
	}

}

void AHordeGameState::ResetLobbyTime()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(LobbyTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(LobbyTimer);
	}
	LobbyTime = LobbyInformation.DefaultLobbyTime;
	PopMessage(FChatMessage("Game Start was interrupted."));
	BlockDisconnect = false;
}

void AHordeGameState::UnreadyAllPlayers()
{
	for (auto& PS : PlayerArray)
	{
		AHordePlayerState* PlayerState = Cast<AHordePlayerState>(PS);
		if (PlayerState)
		{
			PlayerState->SwitchReady(false);
		}
	}
}

void AHordeGameState::StartGame()
{
	if (GameStatus != EGameStatus::EINGAME)
	{
		AbortLobbyTrade();
		GameStatus = EGameStatus::EINGAME;
		for (auto& Ply : PlayerArray)
		{
			AHordePlayerState* PS = Cast<AHordePlayerState>(Ply);
			if (PS)
			{
				PS->ClientUpdateGameStatus(GameStatus);
			}
		}
		AHordeGameMode* HGM = Cast<AHordeGameMode>(GetWorld()->GetAuthGameMode());
		if (HGM)
		{
			HGM->GameStart(LobbyPlayers);
		}

		AHordeWorldSettings* WS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings(true, true));
		if (WS)
		{
			if (WS->MatchMode == EMatchMode::EMatchModeNonLinear)
			{
				StartRoundBasedGame();
			}
		}
	}
}

bool AHordeGameState::IsCharacterTaken(FName CharacterID)
{
	bool RetTaken = false;
	for (auto Char : LobbyInformation.AvailableCharacters)
	{
		if (Char.CharacterID == CharacterID && Char.PlayerID != "")
		{
			RetTaken = true;
		}
	}
	return RetTaken;
}

FName AHordeGameState::GetCharacterByID(FString PlayerID, int32 &CharacterIndex)
{
	int32 TempIndex = -1;
	FName RetTemp = TEXT("None");
	for (auto PChar : LobbyInformation.AvailableCharacters)
	{
		TempIndex++;
		if (PChar.PlayerID == PlayerID)
		{
			RetTemp = PChar.CharacterID;
			break;
		}
	}
	CharacterIndex = TempIndex;
	return RetTemp;
}

AHordePlayerState* AHordeGameState::GetPlayerStateByID(FString PlayerID)
{
	AHordePlayerState* RetPS = nullptr;
	for (auto& PLY : PlayerArray)
	{
		AHordePlayerState* PS = Cast<AHordePlayerState>(PLY);
		if (PS)
		{
			if (PS->UniqueId->ToString() == PlayerID)
			{
				RetPS = PS;
				break;
			}
		}
	}
	return RetPS;
}

void AHordeGameState::FreeupUnassignedCharacters()
{
	TArray<FLobbyAvailableCharacters> LocalCharacters = LobbyInformation.AvailableCharacters;
	TArray<int32> PlayersToFreeup;
	
	int32 LocalCharCurrentIndex = -1;
	for (auto LChar : LocalCharacters)
	{
		LocalCharCurrentIndex += 1;
		bool PlayerFound = false;
		GetUsernameBySteamID(LChar.PlayerID, PlayerFound);
		if (!PlayerFound)
		{
			PlayersToFreeup.AddUnique(LocalCharCurrentIndex);
		}
	}
	for (int32 PlyID : PlayersToFreeup)
	{
		if (LocalCharacters[PlyID].PlayerID != "" && LocalCharacters[PlyID].PlayerUsername != "")
		{
			//Display Disconnect Message.
			PopMessage(FChatMessage(LocalCharacters[PlyID].PlayerUsername + " has disconnected"));

			//Abort Character Trade if Player was involved.
			if (IsTradeInProgress && TradeProgress.Instigator == LocalCharacters[PlyID].PlayerID || TradeProgress.Target == LocalCharacters[PlyID].PlayerID)
			{
				AbortLobbyTrade();
			}

			//Reset Character and set it to not taken.
			LocalCharacters[PlyID].PlayerUsername = "";
			LocalCharacters[PlyID].PlayerID = "";
			LocalCharacters[PlyID].CharacterTaken = false;

		}
		else {
			LocalCharacters[PlyID].PlayerUsername = "";
			LocalCharacters[PlyID].PlayerID = "";
			LocalCharacters[PlyID].CharacterTaken = false;
		}
	}
	//Update Lobby Information with new Character List.
	LobbyInformation.AvailableCharacters = LocalCharacters;


}



FString AHordeGameState::GetUsernameBySteamID(FString ID, bool &FoundPlayer)
{
	FString RetUserName = "None";
	bool TempFoundPlayer = false;
	for (auto& PS : PlayerArray)
	{
		AHordePlayerState* PlayerST = Cast<AHordePlayerState>(PS);
		if (PlayerST)
		{
			if (PlayerST->UniqueId->ToString() == ID)
			{
				RetUserName = PlayerST->GetPlayerName();
				TempFoundPlayer = true;
			}
		}
	}
	FoundPlayer = TempFoundPlayer;
	return RetUserName;
}

void AHordeGameState::UpdatePlayerLobby()
{
	if (HasAuthority())
	{
		LobbyPlayers.Empty();

		if (PlayerArray.Num() >= LobbyInformation.MinimumStartingPlayers)
		{
			StartLobbyTimer();
		}
		else {
			GameStarting = false;
			UnreadyAllPlayers();
			ResetLobbyTime();
		}
		for (auto& PS : PlayerArray)
		{
			AHordePlayerState* PlayerST = Cast<AHordePlayerState>(PS);
			if (PlayerST)
			{
				LobbyPlayers.Add(PlayerST->GetPlayerInfo());
			}
		}
		FreeupUnassignedCharacters();
		for (auto& PS : PlayerArray)
		{
			AHordePlayerState* PlayerST = Cast<AHordePlayerState>(PS);
			if (PlayerST)
			{
				PlayerST->UpdateLobbyPlayerList(LobbyPlayers);
			}
		}
		if (CheckPlayersReady())
		{
			GameStarting = true;
			LobbyTime = 10.f;
		}
	

	}
}

bool AHordeGameState::CheckPlayersReady()
{
	bool Ready = true;
	for (auto& PLY : LobbyPlayers)
	{
		if (!PLY.PlayerReady)
		{
			Ready = false;
			break;
		}
	}
	return Ready;
}

void AHordeGameState::PopMessage(FChatMessage Message)
{
	for (auto& PS : PlayerArray)
	{
		AHordePlayerState* PlayerST = Cast<AHordePlayerState>(PS);
		if (PlayerST)
		{
			PlayerST->OnMessageReceived(Message);
		}
	}
}

FName AHordeGameState::GetFreeCharacter()
{
	FName TempChar = "None";
	for (auto& Char : LobbyInformation.AvailableCharacters)
	{
		if (!Char.CharacterTaken)
		{
			TempChar = Char.CharacterID;
			break;
		}
	}
	return TempChar;
}

void AHordeGameState::AllPlayerDeadCheck()
{
	if (CountAlivePlayers() < 1)
	{
		EndGame(true);
	}
}

void AHordeGameState::EndGame(bool ResetLevel)
{
	if (ResetLevel)
	{
		GameStatus = EGameStatus::EGAMEOVER;
		CalcEndScore(Score_MostKills, Score_MostHeadshots, Score_MostKills);
		NextLevel = GetNextLevelInRotation(ResetLevel);

		for (auto& PS : PlayerArray)
		{
			AHordePlayerState* PLY = Cast<AHordePlayerState>(PS);
			if (PLY)
			{
				PLY->ClientUpdateGameStatus(GameStatus);
			}
		}

		for (TActorIterator<AHordeBaseCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AHordeBaseCharacter* PLYChar = *ActorItr;
			if (PLYChar)
			{
				PLYChar->GetCurrentFirearm()->Destroy();
				PLYChar->Destroy();
			}
		}

		if (!GetWorld()->GetTimerManager().IsTimerActive(EndGameTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(EndGameTimer, this, &AHordeGameState::ProcessEndTime, 1.f, true);
		}
	}
	else
	{
		AHordeWorldSettings* WS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings(false, true));
		if (WS)
		{
			if (ZedsLeft > 0 && WS->MatchMode != EMatchMode::EMatchModeNonLinear)
			{
				PopMessage(FChatMessage("All Zombies needs to be killed to end the Map."));
			}
		}
	
	}

}

void AHordeGameState::ProcessEndTime()
{
	if (EndTime <= 0.f)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(EndGameTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(EndGameTimer);
		}
		GameStatus = EGameStatus::ESERVERTRAVEL;
		for (auto& PS : PlayerArray)
		{
			AHordePlayerState* PLY = Cast<AHordePlayerState>(PS);
			if (PLY)
			{
				PLY->ClientUpdateGameStatus(GameStatus);
			}
		}
		GetWorld()->Exec(GetWorld(), *FString("servertravel " + NextLevel.ToString()));
		ResetLobby();
	}
	else 
	{
		EndTime--;
	}
}

void AHordeGameState::ResetLobby()
{
	AHordeWorldSettings* WS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings(false, true));
	if (WS)
	{
		LobbyTime = 300.f;
		GameStarting = false;
		BlockDisconnect = false;
		GameStatus = EGameStatus::ELOBBY;
	}
}

void AHordeGameState::CalcEndScore(FPlayerScore& MVP, FPlayerScore& HS, FPlayerScore& KS)
{
	FPlayerScore LocalMVP("MVP");
	FPlayerScore LocalHS("Most Headshots");
	FPlayerScore LocalKS("Most Kills");

	for (auto& PS : PlayerArray)
	{
		AHordePlayerState * PLY = Cast<AHordePlayerState>(PS);
		if (PLY)
		{
			if (PLY->Points > MVP.Score)
			{
				MVP.Score = PLY->Points;
				MVP.PlayerID = PLY->GetPlayerInfo().PlayerID;
				MVP.Character = PLY->GetPlayerInfo().SelectedCharacter;
			}
			if (PLY->HeadShots > HS.Score)
			{
				HS.Score = PLY->HeadShots;
				HS.PlayerID = PLY->GetPlayerInfo().PlayerID;
				HS.Character = PLY->GetPlayerInfo().SelectedCharacter;
			}
			if (PLY->ZedKills > KS.Score)
			{
				KS.Score = PLY->ZedKills;
				KS.PlayerID = PLY->GetPlayerInfo().PlayerID;
				KS.Character = PLY->GetPlayerInfo().SelectedCharacter;
			}
		}
	}

	MVP = LocalMVP;
	HS = LocalHS;
	KS = LocalKS;
	
}

int32 AHordeGameState::CountAlivePlayers()
{
	int32 TempAliveCount = 0;
	for (TActorIterator<AHordeBaseCharacter> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AHordeBaseCharacter* PLYChar = *ActorItr;
		if (PLYChar && !PLYChar->GetIsDead())
		{
			TempAliveCount++;
		}
	}
	return TempAliveCount;
}

int32 AHordeGameState::CountAliveZeds()
{
	int32 TempAliveCount = 0;
	for (TActorIterator<AZedPawn> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AZedPawn* Zed = *ActorItr;
		if (Zed && !Zed->GetIsDead())
		{
			TempAliveCount++;
		}

	}
	return TempAliveCount;
}

void AHordeGameState::UpdateAliveZeds()
{
	ZedsLeft = CountAliveZeds();
	AHordeGameMode* GMO = Cast<AHordeGameMode>(GetWorld()->GetAuthGameMode());
	if (GMO)
	{
		GMO->CheckGameOver();
	}
}

void AHordeGameState::StartCharacterTrade(FString InstigatorPlayer, FString TargetPlayer)
{
	if (!IsTradeInProgress && InstigatorPlayer != "" && TargetPlayer != "" && !GetWorld()->GetTimerManager().IsTimerActive(LobbyTradeTimer))
	{
		TradeProgress.Instigator = InstigatorPlayer;
		TradeProgress.Target = TargetPlayer;
		TradeProgress.TimeLeft = 20.f;
		GetWorld()->GetTimerManager().SetTimer(LobbyTradeTimer, this, &AHordeGameState::ProcessCharacterTrade, 1.f, true);
		IsTradeInProgress = true;
	}
}

void AHordeGameState::ProcessCharacterTrade()
{
	if (TradeProgress.TimeLeft > 0)
	{
		TradeProgress.TimeLeft--;
	}
	else {
		AbortLobbyTrade();
	}
}

void AHordeGameState::AcceptCharacterTrade()
{
	
	int32 TargetCharacterIndex = -1;
	int32 InstigatorCharacterIndex = -1;
	FName TargetCharacter = GetCharacterByID(TradeProgress.Target, TargetCharacterIndex);
	FName InstigatorCharacter = GetCharacterByID(TradeProgress.Instigator, InstigatorCharacterIndex);

	AHordePlayerState* TargetPlayerState = GetPlayerStateByID(TradeProgress.Target);
	if (TargetPlayerState)
	{
		LobbyInformation.AvailableCharacters[InstigatorCharacterIndex].PlayerID = TargetPlayerState->GetPlayerInfo().PlayerID;
		LobbyInformation.AvailableCharacters[InstigatorCharacterIndex].PlayerUsername = TargetPlayerState->GetPlayerInfo().UserName;
		TargetPlayerState->SwitchCharacter(InstigatorCharacter);
	}

	AHordePlayerState* InstigatorPlayerState = GetPlayerStateByID(TradeProgress.Instigator);
	if (InstigatorPlayerState)
	{
		LobbyInformation.AvailableCharacters[TargetCharacterIndex].PlayerID = InstigatorPlayerState->GetPlayerInfo().PlayerID;
		LobbyInformation.AvailableCharacters[TargetCharacterIndex].PlayerUsername = InstigatorPlayerState->GetPlayerInfo().UserName;
		InstigatorPlayerState->SwitchCharacter(TargetCharacter);
	}
	AbortLobbyTrade();
	UpdatePlayerLobby();

}

void AHordeGameState::AbortLobbyTrade()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(LobbyTradeTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(LobbyTradeTimer);
	}
	TradeProgress.Instigator = "";
	TradeProgress.Target = "";
	TradeProgress.TimeLeft = 20.f;
	IsTradeInProgress = false;
}


