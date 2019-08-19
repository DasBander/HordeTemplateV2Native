

#include "PlayerKickMenu.h"
#include "Gameplay/HordePlayerState.h"

void UPlayerKickMenu::KickPlayer()
{
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		PS->RequestPlayerKick(PlyInfo);
	}
}
