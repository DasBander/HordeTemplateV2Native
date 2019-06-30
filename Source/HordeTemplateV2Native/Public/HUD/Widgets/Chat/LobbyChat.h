

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/HordeBaseController.h"
#include "LobbyChat.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API ULobbyChat : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
		void OnMessageReceived(const FHordeChatMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Chat")
		void SubmitChatMessage(const FText& Message);

	virtual void NativeDestruct() override;
};

