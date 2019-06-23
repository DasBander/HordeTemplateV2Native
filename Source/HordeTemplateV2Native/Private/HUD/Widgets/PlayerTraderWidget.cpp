

#include "PlayerTraderWidget.h"
#include "Gameplay/HordePlayerState.h"
FText UPlayerTraderWidget::GetPlayerMoney()
{
	FText PlayerMoney;
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		PlayerMoney = FText::FromString(FString::FromInt(PS->PlayerMoney) + " $");
	}
	return PlayerMoney;
}
