

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Widgets/PlayerHUDWidget.h"
#include "HordeBaseHUD.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeBaseHUD : public AHUD
{
	GENERATED_BODY()



protected:
	UPROPERTY()
		UPlayerHUDWidget* PlayerHUDWidget;

	UPROPERTY()
		TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

public:

	AHordeBaseHUD();

	UFUNCTION(BlueprintPure, Category = "HUD")
		UPlayerHUDWidget* GetHUDWidget();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
};
