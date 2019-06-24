

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Gameplay/HordeBaseController.h"
#include "GameChat.generated.h"

/**
 * 
 */


UCLASS()
class HORDETEMPLATEV2NATIVE_API UGameChat : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
		void OnMessageReceived(const FChatMessage& Message);

	UFUNCTION(BlueprintImplementableEvent, Category = "Chat")
		void OnGameFocusChat();

	UFUNCTION(BlueprintCallable, Category = "Chat")
		void SubmitChatMessage(const FText& Message);

	virtual void NativeDestruct() override;

};
