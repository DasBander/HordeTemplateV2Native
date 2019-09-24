

#include "PlayerTraderWidget.h"
#include "Gameplay/HordePlayerState.h"

/** ( Virtual; Overridden )
 * Sets widget bIsFocusable to true.
 *
 * @param
 * @return
 */
void UPlayerTraderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;
}

/**
 * Returns Players Money as Text.
 *
 * @param
 * @return Players Money as Text.
 */
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
