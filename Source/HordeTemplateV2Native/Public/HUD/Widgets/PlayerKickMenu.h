

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/LobbyStructures.h"
#include "PlayerKickMenu.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerKickMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn = true))
		FPlayerInfo PlyInfo;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
		void KickPlayer();
};
