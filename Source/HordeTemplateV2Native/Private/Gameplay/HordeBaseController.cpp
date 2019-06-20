

#include "HordeBaseController.h"
#include "Kismet/GameplayStatics.h"
#include "HUD/HordeBaseHUD.h"

void AHordeBaseController::ClientCloseTraderUI_Implementation()
{

}

void AHordeBaseController::ClientPlay2DSound_Implementation(USoundCue* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound);
	}
}

AHordeBaseController::AHordeBaseController()
{
	bAttachToPawn = true;

	
}

void AHordeBaseController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("Toggle Chat", IE_Pressed,this, &AHordeBaseController::ToggleChat);
	}
}

void AHordeBaseController::ToggleChat()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		if (!HUD->IsInChat)
		{
			HUD->IsInChat = true;
			bShowMouseCursor = true;
			OnFocusGameChat.Broadcast();
		}
	}
}

void AHordeBaseController::CloseChat()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->IsInChat = false;
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
}
