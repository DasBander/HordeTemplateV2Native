

#include "PlayerTraderWidget.h"
#include "Gameplay/HordePlayerState.h"

void UPlayerTraderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
}

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
