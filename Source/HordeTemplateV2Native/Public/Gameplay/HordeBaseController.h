

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Gameplay/LobbyStructures.h"
#include "Sound/SoundCue.h"
#include "HordeBaseController.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeBaseController : public APlayerController
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReveived, const FHordeChatMessage&, Message);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusGameChat);

public:

	UPROPERTY()
		FOnMessageReveived OnMessageReceivedDelegate;

	UPROPERTY()
		FOnMessageReveived OnLobbyMessageReceivedDelegate;

	UPROPERTY()
		FOnFocusGameChat OnFocusGameChat;

	AHordeBaseController();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "UI")
		void ClientCloseTraderUI();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "UI")
		void ClientOpenTraderUI();

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Game")
		void ClientPlay2DSound(USoundCue* Sound);

	UFUNCTION()
		void OpenEscapeMenu();

	UFUNCTION()
		void ToggleScoreboard();

	virtual void SetupInputComponent() override;

	void DisconnectFromServer();

	UFUNCTION()
	void ToggleChat();

	UFUNCTION()
		void CloseChat();
};
