

#include "PlayerScoreboardWidget.h"
#include "Gameplay/HordeGameState.h"

void UPlayerScoreboardWidget::UpdatePlayerList(const TArray<APlayerState*>& PlayerList)
{
	OnPlayerListUpdated(PlayerList);
}

FText UPlayerScoreboardWidget::GetLobbyName()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		return FText::FromString(GS->LobbyInformation.LobbyName);
	}
	else
	{
		return FText::FromString("Could not cast to GameState");
	}
}
