

#include "PlayerHeadDisplay.h"

void UPlayerHeadDisplay::NativeConstruct()
{
	Super::NativeConstruct();

	OnShowWidgetDelegate.AddDynamic(this, &UPlayerHeadDisplay::OnShowWidget);
	OnHideWidgetDelegate.AddDynamic(this, &UPlayerHeadDisplay::OnHideWidget);
}

FText UPlayerHeadDisplay::GetPlayerName()
{
	return FText::FromString(PlayerName);
}

float UPlayerHeadDisplay::GetPlayerHealth()
{
	InterpHealth = FMath::FInterpTo(InterpHealth, Health, GetWorld()->GetDeltaSeconds(), 2.f);
	return InterpHealth / 100.f;
}
