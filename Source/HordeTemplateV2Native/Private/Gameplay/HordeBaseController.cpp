

#include "HordeBaseController.h"
#include "Kismet/GameplayStatics.h"
#include "HordePlayerState.h"
#include "Character/HordeBaseCharacter.h"
#include "HUD/HordeBaseHUD.h"

void AHordeBaseController::ClientCloseTraderUI_Implementation()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->CloseTraderUI();
	}
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

void AHordeBaseController::ClientOpenTraderUI_Implementation()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->OpenTraderUI();
	}
}

void AHordeBaseController::OpenEscapeMenu()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->OpenEscapeMenu();
	}
}

void AHordeBaseController::ToggleScoreboard()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->ToggleScoreboard();
	}
}

void AHordeBaseController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent)
	{
		InputComponent->BindAction("Toggle Chat", IE_Pressed,this, &AHordeBaseController::ToggleChat);
		InputComponent->BindAction("EscapeMenu", IE_Pressed, this, &AHordeBaseController::OpenEscapeMenu);

		InputComponent->BindAction("Toggle Scoreboard", IE_Pressed, this, &AHordeBaseController::ToggleScoreboard);
		InputComponent->BindAction("Toggle Scoreboard", IE_Released, this, &AHordeBaseController::ToggleScoreboard);
	}
}

void AHordeBaseController::DisconnectFromServer()
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetPawn());
	if (PLY && PLY->GetCurrentFirearm())
	{
		PLY->Inventory->ServerDropItem(PLY->GetCurrentFirearm());
		
	}
	AHordePlayerState* PS = Cast<AHordePlayerState>(PlayerState);
	if (PS)
	{
		PS->GettingKicked();
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
