

#include "PlayerKickMenu.h"
#include "Gameplay/HordePlayerState.h"

/**
 * Kicks the Player with Owning Player Info.
 *
 * @param
 * @return void
 */
void UPlayerKickMenu::KickPlayer()
{
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		PS->RequestPlayerKick(PlyInfo);
	}
}
