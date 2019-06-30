

#include "PlayerEscapeMenu.h"
#include "Gameplay/HordeBaseController.h"
#include "HUD/HordeBaseHUD.h"

void UPlayerEscapeMenu::DisconnectFromServer()
{
	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		PC->DisconnectFromServer();
	}
}

void UPlayerEscapeMenu::CloseEscapeMenu()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetOwningPlayer()->GetHUD());
	if (HUD)
	{
		HUD->CloseEscapeMenu();
	}
}
