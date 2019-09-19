

#include "HordeBaseController.h"
#include "Kismet/GameplayStatics.h"
#include "HordePlayerState.h"
#include "Character/HordeBaseCharacter.h"
#include "HUD/HordeBaseHUD.h"

/*
	FUNCTION: Client Close Trader UI ( Client )
	PARAM: None
	RETURN: void
	DESC:
	Runs Close Trader UI in HUD Class
*/
void AHordeBaseController::ClientCloseTraderUI_Implementation()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->CloseTraderUI();
	}
}

/*
	FUNCTION: Client Play 2D Sound ( Client )
	PARAM: USoundCue ( Sound To Play )
	RETURN: void
	DESC:
	Plays sound on Owning Client.
*/
void AHordeBaseController::ClientPlay2DSound_Implementation(USoundCue* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound);
	}
}

/*
	FUNCTION: Constructor for AHordeBaseController
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for AHordeBaseController
*/
AHordeBaseController::AHordeBaseController()
{
	bAttachToPawn = true;
}

/*
	FUNCTION: Client Open Trader UI ( Client )
	PARAM: None
	RETURN: void
	DESC:
	Runs OpenTraderUI in HUD Class.
*/
void AHordeBaseController::ClientOpenTraderUI_Implementation()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->OpenTraderUI();
	}
}


/*
	FUNCTION: Open Escape Menu
	PARAM: None
	RETURN: void
	DESC:
	Runs OpenEscapeMenu in HUD Class.
*/
void AHordeBaseController::OpenEscapeMenu()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->OpenEscapeMenu();
	}
}

/*
	FUNCTION: Toggle Scoreboard
	PARAM: None
	RETURN: void
	DESC:
	Runs ToggleScoreboard in HUD Class.
*/
void AHordeBaseController::ToggleScoreboard()
{
	AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(GetHUD());
	if (HUD)
	{
		HUD->ToggleScoreboard();
	}
}

/*
	FUNCTION: SetupInputComponent
	PARAM: None
	RETURN: void
	DESC:
	Sets up the key bindings that should be permanent.
*/
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

/*
	FUNCTION: Disconnect From Server 
	PARAM: None
	RETURN: void
	DESC:
	Drops Current Firearm and Kicks Player.
*/
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

/*
	FUNCTION: Toggle Chat
	PARAM: None
	RETURN: void
	DESC:
	Toggles Chat in-game in HUD Class.
*/
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

/*
	FUNCTION: Close Chat
	PARAM: None
	RETURN: void
	DESC:
	Closes Chat and sets input to Game Only.
*/
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
