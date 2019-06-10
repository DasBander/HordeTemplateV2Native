

#include "HordeGameState.h"
#include "HordePlayerState.h"
#include "HordeTemplateV2Native.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Gameplay/HordeWorldSettings.h"

void AHordeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHordeGameState, LobbyInformation);
	DOREPLIFETIME(AHordeGameState, GameStatus);
	DOREPLIFETIME(AHordeGameState, GameStarting);
	DOREPLIFETIME(AHordeGameState, LobbyTime);
	DOREPLIFETIME(AHordeGameState, BlockDisconnect);
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
			PopMessage(FChatMessage("SERVER", FText::FromString("Game starting in " + FString::FromInt(FMath::RoundToInt(LobbyTime)) + " seconds.")));
			if (FMath::RoundToInt(LobbyTime) <= 5)
			{
				BlockDisconnect = true;
			}
		}
		else {
			if (FMath::RoundToInt(LobbyTime) == 0)
			{
				PopMessage(FChatMessage("SERVER", FText::FromString("Game starting....")));
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
	PopMessage(FChatMessage("SERVER", FText::FromString("Game Start was interrupted.")));
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
			PopMessage(FChatMessage("SERVER", FText::FromString(LocalCharacters[PlyID].PlayerUsername + " has disconnected")));
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
