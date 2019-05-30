

#include "PlayerHeadDisplay.h"

void UPlayerHeadDisplay::NativeConstruct()
{
	OnShowWidgetDelegate.AddDynamic(this, &UPlayerHeadDisplay::OnShowWidget);
	OnHideWidgetDelegate.AddDynamic(this, &UPlayerHeadDisplay::OnHideWidget);
}
