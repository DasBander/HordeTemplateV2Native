

#include "LobbyPlayerWidget.h"
#include "Gameplay/HordePlayerState.h"
#include "Gameplay/HordeGameState.h"


FLinearColor ULobbyPlayerWidget::GetReadyColor()
{
	if (PlayerInfo.PlayerReady)
	{
		return FLinearColor(0.003907f, 0.628472f, 0.f, 0.8f);
	}
	else
	{
		return FLinearColor(0.f, 0.f, 0.f, 0.8);
	}
}

ESlateVisibility ULobbyPlayerWidget::GetCharacterAvailableVisibility()
{
	if (PlayerInfo.SelectedCharacter != NAME_None)
	{
		return ESlateVisibility::Visible;
	}
	else
	{
		return ESlateVisibility::Hidden;
	}
}

ESlateVisibility ULobbyPlayerWidget::GetTradingAvailableVisibility()
{
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			if (PlayerInfo.PlayerReady || PlayerInfo.PlayerID == PS->GetPlayerInfo().PlayerID || GS->IsTradeInProgress || PS->GetPlayerInfo().PlayerReady)
			{
				return ESlateVisibility::Hidden;
			}
			else {
				return ESlateVisibility::Visible;
			}
		}
		else {
			return ESlateVisibility::Hidden;
		}
	}
	else
	{
		return ESlateVisibility::Hidden;
	}
}

void ULobbyPlayerWidget::InitiateTrade()
{
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		PS->RequestCharacterTrade(PS->GetPlayerInfo().PlayerID, PlayerInfo.PlayerID);
	}
}

bool ULobbyPlayerWidget::IsAdmin()
{
	bool RetIs = false;
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			if (PS->GetPlayerInfo().PlayerID == GS->LobbyInformation.OwnerID)
			{
				 RetIs = true;
			}
		}
	}
	return RetIs;
}



bool ULobbyPlayerWidget::IsOwner()
{
	bool RetIs = false;
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			if (PS->GetPlayerInfo().PlayerID == PlayerInfo.PlayerID)
			{
				RetIs = true;
			}
		}
	}
	return RetIs;
}

void ULobbyPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Character = FromDatatable<FPlayableCharacter>(CHARACTER_DATATABLE_PATH, PlayerInfo.SelectedCharacter);
}
