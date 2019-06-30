

#include "PlayerScoreboardWidget.h"

void UPlayerScoreboardWidget::UpdatePlayerList(const TArray<APlayerState*>& PlayerList)
{
	OnPlayerListUpdated(PlayerList);
}
