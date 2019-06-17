

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Sound/SoundCue.h"
#include "HordeBaseController.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeBaseController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AHordeBaseController();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "UI")
		void ClientCloseTraderUI();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Game")
		void ClientPlay2DSound(USoundCue* Sound);
};
