

#include "PlayerHeadDisplay.h"

void UPlayerHeadDisplay::NativeConstruct()
{
	Super::NativeConstruct();

	OnShowWidgetDelegate.AddDynamic(this, &UPlayerHeadDisplay::OnShowWidget);
	OnHideWidgetDelegate.AddDynamic(this, &UPlayerHeadDisplay::OnHideWidget);
}
