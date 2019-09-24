

#include "PlayerEscapeMenu.h"
#include "Gameplay/HordeBaseController.h"
#include "HUD/HordeBaseHUD.h"

/**
 * Disconnects owning player from server.
 *
 * @param
 * @return void
 */
void UPlayerEscapeMenu::DisconnectFromServer()
{
	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		PC->DisconnectFromServer();
	}
}

/**
 * Closes Escape Menu.
 *
 * @param
 * @return void
 */
void UPlayerEscapeMenu::CloseEscapeMenu()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetOwningPlayer()->GetHUD());
	if (HUD)
	{
		HUD->CloseEscapeMenu();
	}
}
