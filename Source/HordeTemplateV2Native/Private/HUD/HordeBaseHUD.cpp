

#include "HordeBaseHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Engine/Texture2D.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Character/HordeBaseCharacter.h"

void AHordeBaseHUD::GameStatusChanged(uint8 GameStatus)
{
	EGameStatus GS = (EGameStatus)GameStatus;
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
				FInputModeGameAndUI PlyInput = FInputModeGameAndUI();
				PlyInput.SetWidgetToFocus(PlayerLobbyWidget->GetCachedWidget());
				GetOwningPlayerController()->SetInputMode(PlyInput);
				GetOwningPlayerController()->bShowMouseCursor = true;
			} 

			break;

		default:
			break;
	}
}

AHordeBaseHUD::AHordeBaseHUD()
{
	const ConstructorHelpers::FClassFinder<UPlayerHUDWidget> PlayerHUDAsset(TEXT("WidgetBlueprint'/Game/HordeTemplateBP/Blueprint/Widgets/WBP_HUDCpp.WBP_HUDCpp_C'"));
	if (PlayerHUDAsset.Class)
	{
		PlayerHUDWidgetClass = PlayerHUDAsset.Class;
	}

	const ConstructorHelpers::FClassFinder<UPlayerLobbyWidget> PlayerLobbyAsset(TEXT("WidgetBlueprint'/Game/HordeTemplateBP/Blueprint/Widgets/Lobby/WBP_Lobby_Main.WBP_Lobby_Main_C'"));
	if (PlayerLobbyAsset.Class)
	{
		PlayerLobbyWidgetClass = PlayerLobbyAsset.Class;
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexAsset(TEXT("Texture2D'/Game/HordeTemplateBP/Assets/Textures/Hud/center_dot.center_dot'"));
	if (CrosshairTexAsset.Succeeded())
	{
		CrosshairTex = CrosshairTexAsset.Object;
	}

	OnGameStatusChanged.AddDynamic(this, &AHordeBaseHUD::GameStatusChanged);

}

UPlayerHUDWidget* AHordeBaseHUD::GetHUDWidget()
{
	return (PlayerHUDWidget) ? PlayerHUDWidget : nullptr;
}

UPlayerLobbyWidget* AHordeBaseHUD::GetLobbyWidget()
{
	return (PlayerLobbyWidget) ? PlayerLobbyWidget : nullptr;
}

void AHordeBaseHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
}

void AHordeBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetOwningPlayerController(), PlayerHUDWidgetClass);
	PlayerLobbyWidget = CreateWidget<UPlayerLobbyWidget>(GetOwningPlayerController(), PlayerLobbyWidgetClass);

}

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

