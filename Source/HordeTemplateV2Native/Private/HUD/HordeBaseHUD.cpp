

#include "HordeBaseHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Engine/Texture2D.h"
#include "Gameplay/HordeGameState.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/HordeBaseCharacter.h"

/**
 * Swaps widgets out and switches input mode depending on the game status. 
 *
 * @param Current Game Status.
 * @return void
 */
void AHordeBaseHUD::GameStatusChanged(uint8 GameStatus)
{
	EGameStatus GS = (EGameStatus)GameStatus;
	CurrentGameStatus = GS;
	FirstTimeGameStatusChange = true;
	//Remove All Widgets from Viewport.
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient)
	{
		ViewportClient->RemoveAllViewportWidgets();
	}
	
	//Add Widgets to Viewport depending on Game Status.
	switch (GS) {
		case EGameStatus::EINGAME:
			if (PlayerHUDWidget) {
				PlayerHUDWidget->AddToViewport();
				GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
				GetOwningPlayerController()->bShowMouseCursor = false;
			} 
			break;
		case EGameStatus::ELOBBY:
			if (PlayerLobbyWidget)
			{
				PlayerLobbyWidget->AddToViewport();
				FInputModeGameAndUI * PlyInput = new FInputModeGameAndUI();
				PlyInput->SetWidgetToFocus(PlayerLobbyWidget->TakeWidget());
				GetOwningPlayerController()->SetInputMode(*PlyInput);
				GetOwningPlayerController()->bShowMouseCursor = true;
			} 
			break;
		case EGameStatus::EGAMEOVER:
			if (PlayerEndScreenWidget)
			{
				PlayerEndScreenWidget->AddToViewport();
				GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
				GetOwningPlayerController()->bShowMouseCursor = false;
			}
			break;

		case EGameStatus::ESERVERTRAVEL:
			PlayerTravelWidget->AddToViewport();
			GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
			GetOwningPlayerController()->bShowMouseCursor = false;
			break;

		default:
			break;
	}
}

/**
 * On Player Points Received Binding.
 *
 * @param The Ponts Type and Points.
 * @return void
 */
void AHordeBaseHUD::OnPlayerPointsReceived(EPointType PointType, int32 Points)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->OnPointsReceived(PointType, Points);
	}
}

/**
 * Constructor for AHordeBaseHUD
 *
 * @param
 * @return
 */
AHordeBaseHUD::AHordeBaseHUD()
{
	const ConstructorHelpers::FClassFinder<UPlayerHUDWidget> PlayerHUDAsset(WIDGET_HUD_MAIN_UI_PATH);
	if (PlayerHUDAsset.Class)
	{
		PlayerHUDWidgetClass = PlayerHUDAsset.Class;
	}

	const ConstructorHelpers::FClassFinder<UPlayerLobbyWidget> PlayerLobbyAsset(WIDGET_LOBBY_UI_PATH);
	if (PlayerLobbyAsset.Class)
	{
		PlayerLobbyWidgetClass = PlayerLobbyAsset.Class;
	}

	const ConstructorHelpers::FClassFinder<UPlayerTraderWidget> PlayerTraderAsset(WIDGET_TRADER_UI_PATH);
	if (PlayerTraderAsset.Class)
	{
		PlayerTraderWidgetClass = PlayerTraderAsset.Class;
	}

	const ConstructorHelpers::FClassFinder<UPlayerEndScreen> PlayerEndScreenAsset(WIDGET_ENDSCREEN_UI_PATH);
	if (PlayerEndScreenAsset.Class)
	{
		PlayerEndScreenClass = PlayerEndScreenAsset.Class;
	}

	const ConstructorHelpers::FClassFinder<UPlayerTravelWidget> PlayerTravelWidgetAsset(WIDGET_SERVERTRAVEL_UI_PATH);
	if (PlayerTravelWidgetAsset.Class)
	{
		PlayerTravelWidgetClass = PlayerTravelWidgetAsset.Class;
	}

	const ConstructorHelpers::FClassFinder<UPlayerEscapeMenu> PlayerEscapeWidgetAsset(WIDGET_ESCAPEMENU_UI_PATH);
	if (PlayerEscapeWidgetAsset.Class)
	{
		PlayerEscapeWidgetClass = PlayerEscapeWidgetAsset.Class;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexAsset(CROSSHAIR_TEXTURE_PATH);
	if (CrosshairTexAsset.Succeeded())
	{
		CrosshairTex = CrosshairTexAsset.Object;
	}

	static ConstructorHelpers::FClassFinder<UPlayerScoreboardWidget> PlayerScoreboardAsset(WIDGET_SCOREBOARD_PATH);
	if (PlayerScoreboardAsset.Succeeded())
	{
		PlayerScoreboardWidgetClass = PlayerScoreboardAsset.Class;
	}

	OnGameStatusChanged.AddDynamic(this, &AHordeBaseHUD::GameStatusChanged);
	OnPlayerPointsReceivedDelegate.AddDynamic(this, &AHordeBaseHUD::OnPlayerPointsReceived);
}

/**
 * Returns HUD Widget Object
 *
 * @param
 * @return HUDWidget Object
 */
UPlayerHUDWidget* AHordeBaseHUD::GetHUDWidget()
{
	return (PlayerHUDWidget) ? PlayerHUDWidget : nullptr;
}

/**
 * Returns Lobby Widget Object.
 *
 * @param
 * @return Lobby Widget Object.
 */
UPlayerLobbyWidget* AHordeBaseHUD::GetLobbyWidget()
{
	return (PlayerLobbyWidget) ? PlayerLobbyWidget : nullptr;
}

/**
 * Closes Escape Menu and Resets input mode to game only.
 *
 * @param
 * @return void
 */
void AHordeBaseHUD::CloseEscapeMenu()
{
	if (PlayerEscapeWidget)
	{
		PlayerEscapeWidget->RemoveFromParent();
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
		GetOwningPlayerController()->bShowMouseCursor = false;
	}
}

/**
 * Opens or closes Scoreboard.
 *
 * @param
 * @return void
 */
void AHordeBaseHUD::ToggleScoreboard()
{
	if (!IsInChat && !bIsTraderUIOpen && CurrentGameStatus == EGameStatus::EINGAME)
	{
		if (!bIsScoreboardOpen)
		{
			AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
			if(GS)
			{
				PlayerScoreboardWidget->UpdatePlayerList(GS->PlayerArray);
			}	
			bIsScoreboardOpen = true;
			PlayerScoreboardWidget->AddToViewport(9999);
		}
		else {
			PlayerScoreboardWidget->RemoveFromParent();
			bIsScoreboardOpen = false;
		}
	}
}

/**
 * Opens Escape Menu or closes chat or trader ui.
 *
 * @param
 * @return void
 */
void AHordeBaseHUD::OpenEscapeMenu()
{
	if (!bIsScoreboardOpen && CurrentGameStatus != EGameStatus::ELOBBY)
	{
		if (IsInChat)
		{
			GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
			GetOwningPlayerController()->bShowMouseCursor = false;
			IsInChat = false;
		}

		if (bIsTraderUIOpen)
		{
			CloseTraderUI();
		}

		PlayerEscapeWidget->AddToViewport(9999);
		FInputModeUIOnly* InputMode = new FInputModeUIOnly();
		InputMode->SetWidgetToFocus(PlayerEscapeWidget->TakeWidget());
		GetOwningPlayerController()->SetInputMode(*InputMode);
		GetOwningPlayerController()->bShowMouseCursor = true;

	}
}

/**
 * Opens Trader UI
 *
 * @param
 * @return void
 */
void AHordeBaseHUD::OpenTraderUI()
{
	if (!IsInChat && !bIsScoreboardOpen && CurrentGameStatus == EGameStatus::EINGAME && !bIsTraderUIOpen)
	{
		PlayerTraderWidget->AddToViewport(9999);
		FInputModeUIOnly * InputMode = new FInputModeUIOnly();
		InputMode->SetWidgetToFocus(PlayerTraderWidget->TakeWidget());
		GetOwningPlayerController()->SetInputMode(*InputMode);
		GetOwningPlayerController()->bShowMouseCursor = true;
		bIsTraderUIOpen = true;
	}
}

/**
 * Closes Trader UI
 *
 * @param
 * @return void
 */
void AHordeBaseHUD::CloseTraderUI()
{
	if (bIsTraderUIOpen)
	{
		PlayerTraderWidget->RemoveFromViewport();
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
		GetOwningPlayerController()->bShowMouseCursor = false;
		bIsTraderUIOpen = false;
	}
}

/** ( Virtual; Overridden )
 * Tick
 *
 * @param
 * @return
 */
void AHordeBaseHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

/** ( Virtual; Overridden )
 * Creates all Widgets on Begin Play and makes Objects Valid.
 *
 * @param
 * @return void
 */
void AHordeBaseHUD::BeginPlay()
{
	Super::BeginPlay();


	PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetOwningPlayerController(), PlayerHUDWidgetClass);

	PlayerLobbyWidget = CreateWidget<UPlayerLobbyWidget>(GetOwningPlayerController(), PlayerLobbyWidgetClass);

	PlayerTraderWidget = CreateWidget<UPlayerTraderWidget>(GetOwningPlayerController(), PlayerTraderWidgetClass);

	PlayerEndScreenWidget =CreateWidget<UPlayerEndScreen>(GetOwningPlayerController(), PlayerEndScreenClass);
	
	PlayerTravelWidget = CreateWidget<UPlayerTravelWidget>(GetOwningPlayerController(), PlayerTravelWidgetClass);
	
	PlayerEscapeWidget = CreateWidget<UPlayerEscapeMenu>(GetOwningPlayerController(), PlayerEscapeWidgetClass);

	PlayerScoreboardWidget = CreateWidget<UPlayerScoreboardWidget>(GetOwningPlayerController(), PlayerScoreboardWidgetClass);

}

/** ( Virtual; Overridden )
 * Draws Waiting for Server if Player state is not valid or simply client isn't ready.
 * Draws Center Dot.
 * @param
 * @return void
 */
void AHordeBaseHUD::DrawHUD()
{
	Super::DrawHUD();

	// Find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// Offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
		(Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)));

	// Draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	//Display Waiting for Server if player isn't ready yet.
	if (!GetOwningPlayerController()->PlayerState || !FirstTimeGameStatusChange) {
		//Draw Message if player is not Ready.
		DrawText("Waiting for Server...", FLinearColor(FVector4(1.f, 1.f, 1.f, 1.f)), 10.f, 10.f, nullptr, 3.f, false);
	}
}

/** ( Virtual; Overridden )
 * Releases Slate Resources on HUDClass getting destroyed.
 *
 * @param End Play Reason
 * @return void
 */
void AHordeBaseHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PlayerHUDWidget->ReleaseSlateResources(true);
	PlayerLobbyWidget->ReleaseSlateResources(true);
	PlayerTraderWidget->ReleaseSlateResources(true);
	PlayerEndScreenWidget->ReleaseSlateResources(true);
	PlayerTravelWidget->ReleaseSlateResources(true);
	PlayerScoreboardWidget->ReleaseSlateResources(true);
	PlayerScoreboardWidget = nullptr;
	PlayerHUDWidget = nullptr;
	PlayerLobbyWidget = nullptr;
	PlayerTraderWidget = nullptr;
	PlayerEndScreenWidget = nullptr;
	PlayerTravelWidget = nullptr;

	Super::EndPlay(EndPlayReason);
}

