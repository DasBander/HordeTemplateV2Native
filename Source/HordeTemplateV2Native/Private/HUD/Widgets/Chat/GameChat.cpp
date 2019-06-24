

#include "GameChat.h"
#include "Gameplay/HordePlayerState.h"

void UGameChat::NativeConstruct()
{
	Super::NativeConstruct();

	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		PC->OnMessageReceivedDelegate.AddDynamic(this, &UGameChat::OnMessageReceived);
		PC->OnFocusGameChat.AddDynamic(this, &UGameChat::OnGameFocusChat);
	}
}

void UGameChat::SubmitChatMessage(const FText& Message)
{
	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		AHordePlayerState* PS = Cast<AHordePlayerState>(PC->PlayerState);
		if (PS)
		{
			PS->SubmitMessage(Message);
			PC->CloseChat();
		}
	}
}

void UGameChat::NativeDestruct()
{
	Super::NativeDestruct();
}
