

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
	}
}

void AHordeGameState::TakePlayer(FPlayerInfo Player)
{

}

void AHordeGameState::UpdatePlayerLobby()
{

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
