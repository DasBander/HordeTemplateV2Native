

#include "HordeBaseHUD.h"
#include "Character/HordeBaseCharacter.h"

AHordeBaseHUD::AHordeBaseHUD()
{
	const ConstructorHelpers::FClassFinder<UPlayerHUDWidget> PlayerHUDAsset(TEXT("WidgetBlueprint'/Game/HordeTemplateBP/Blueprint/Widgets/WBP_HUDCpp.WBP_HUDCpp_C'"));
	if (PlayerHUDAsset.Class)
	{
		PlayerHUDWidgetClass = PlayerHUDAsset.Class;
	}
}

UPlayerHUDWidget* AHordeBaseHUD::GetHUDWidget()
{
	return (PlayerHUDWidget) ? PlayerHUDWidget : nullptr;
}

void AHordeBaseHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void AHordeBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetOwningPlayerController(), PlayerHUDWidgetClass);
	PlayerHUDWidget->AddToViewport();
}


