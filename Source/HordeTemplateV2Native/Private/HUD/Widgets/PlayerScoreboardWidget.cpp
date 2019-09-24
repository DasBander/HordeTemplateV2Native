

#include "PlayerScoreboardWidget.h"
#include "Gameplay/HordeGameState.h"

/**
 * Updates the current Player List by given playerlist.
 *
 * @param Array of Player States.
 * @return void
 */
void UPlayerScoreboardWidget::UpdatePlayerList(const TArray<APlayerState*>& PlayerList)
{
	OnPlayerListUpdated(PlayerList);
}

/**
 * Returns the current lobby name as text.
 *
 * @param
 * @return Lobby Name as Text.
 */
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
