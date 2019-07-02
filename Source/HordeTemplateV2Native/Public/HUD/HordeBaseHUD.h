

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Widgets/PlayerHUDWidget.h"
#include "Widgets/PlayerLobbyWidget.h"
#include "Widgets/PlayerTraderWidget.h"
#include "Widgets/PlayerEndScreen.h"
#include "Widgets/PlayerTravelWidget.h"
#include "Widgets/PlayerScoreboardWidget.h"
#include "Widgets/PlayerEscapeMenu.h"
#include "Gameplay/LobbyStructures.h"
#include "HordeTemplateV2Native.h"
#include "HordeBaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeBaseHUD : public AHUD
{

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStatusChanged, uint8, GS);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerPointsReceived, EPointType, PointType, int32, Points);

	GENERATED_BODY()

public:
	UPROPERTY()
		FOnPlayerPointsReceived OnPlayerPointsReceivedDelegate;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UPlayerHUDWidget* PlayerHUDWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UPlayerLobbyWidget* PlayerLobbyWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UPlayerTraderWidget* PlayerTraderWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UPlayerEndScreen* PlayerEndScreenWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UPlayerTravelWidget* PlayerTravelWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UPlayerEscapeMenu* PlayerEscapeWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widget")
		class UPlayerScoreboardWidget* PlayerScoreboardWidget;
protected:

	UPROPERTY()
		TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY()
		TSubclassOf<class UPlayerLobbyWidget> PlayerLobbyWidgetClass;

	UPROPERTY()
		TSubclassOf<class UPlayerTraderWidget> PlayerTraderWidgetClass;

	UPROPERTY()
		TSubclassOf<class UPlayerEscapeMenu> PlayerEscapeWidgetClass;


	UPROPERTY()
		TSubclassOf<class UPlayerEndScreen> PlayerEndScreenClass;

	UPROPERTY()
		TSubclassOf<class UPlayerTravelWidget> PlayerTravelWidgetClass;

	UPROPERTY()
		TSubclassOf<class UPlayerScoreboardWidget> PlayerScoreboardWidgetClass;

	UFUNCTION()
		void GameStatusChanged(uint8 GameStatus);

	UFUNCTION()
		void OnPlayerPointsReceived(EPointType PointType, int32 Points);

	UPROPERTY()
		bool FirstTimeGameStatusChange = false;

	UPROPERTY()
	class UTexture2D* CrosshairTex;
public:

	AHordeBaseHUD();

	EGameStatus CurrentGameStatus = EGameStatus::ELOBBY;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
		bool IsInChat = false;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
		bool bIsScoreboardOpen = false;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
		bool bIsTraderUIOpen = false;


	UPROPERTY()
		FOnGameStatusChanged OnGameStatusChanged;

	UFUNCTION(BlueprintPure, Category = "HUD")
		UPlayerHUDWidget* GetHUDWidget();

	UFUNCTION(BlueprintPure, Category = "HUD")
		UPlayerLobbyWidget* GetLobbyWidget();

	UFUNCTION(BlueprintCallable, Category="HUD")
		void CloseEscapeMenu();

	UFUNCTION(BlueprintCallable, Category = "HUD")
		void ToggleScoreboard();

	UFUNCTION(BlueprintCallable, Category = "HUD")
		void OpenEscapeMenu();

	UFUNCTION(BlueprintCallable, Category="HUD")
		void OpenTraderUI();

	UFUNCTION(BlueprintCallable, Category = "HUD")
		void CloseTraderUI();


	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
