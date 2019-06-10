

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Widgets/PlayerHUDWidget.h"
#include "Widgets/PlayerLobbyWidget.h"
#include "Gameplay/LobbyStructures.h"
#include "HordeBaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeBaseHUD : public AHUD
{

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStatusChanged, uint8, GS);

	GENERATED_BODY()



protected:
	UPROPERTY()
		UPlayerHUDWidget* PlayerHUDWidget;

	UPROPERTY()
		UPlayerLobbyWidget * PlayerLobbyWidget;


	UPROPERTY()
		TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
		TSubclassOf<class UPlayerLobbyWidget> PlayerLobbyWidgetClass;

	UFUNCTION()
		void GameStatusChanged(uint8 GameStatus);

	UPROPERTY()
		bool FirstTimeGameStatusChange = false;

	UPROPERTY()
	class UTexture2D* CrosshairTex;
public:

	AHordeBaseHUD();


	UPROPERTY()
		FOnGameStatusChanged OnGameStatusChanged;

	UFUNCTION(BlueprintPure, Category = "HUD")
		UPlayerHUDWidget* GetHUDWidget();

	UFUNCTION(BlueprintPure, Category = "HUD")
		UPlayerLobbyWidget* GetLobbyWidget();


	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;
};
