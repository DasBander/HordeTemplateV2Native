

#include "TraderItemWidget.h"
#include "Gameplay/HordePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "HordeTemplateV2Native.h"

void UTraderItemWidget::BuyItem()
{
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		if ((PS->GetPlayerMoney() - TraderItem.ItemPrice) >= 0) 
		{
			PS->BuyItem(TraderItem.ItemID);
			USoundCue* BuySound = ObjectFromPath<USoundCue>(TRADER_BUY_SOUND);
			if (BuySound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), BuySound);
			}
			
		}
	}
}

FText UTraderItemWidget::GetPriceText()
{
	return FText::FromString(FString::FromInt(TraderItem.ItemPrice) + "$");
}

bool UTraderItemWidget::HasEnoughMoney()
{
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
	if (PS)
	{
		return (PS->GetPlayerMoney() - TraderItem.ItemPrice) >= 0;
	}
	else
	{
		return false;
	}
}
