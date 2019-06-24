

#include "LobbyChat.h"

#include "Gameplay/HordePlayerState.h"

void ULobbyChat::NativeConstruct()
{
	Super::NativeConstruct();

	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		PC->OnLobbyMessageReceivedDelegate.AddDynamic(this, &ULobbyChat::OnMessageReceived);
	}
}

void ULobbyChat::SubmitChatMessage(const FText& Message)
{
	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		AHordePlayerState* PS = Cast<AHordePlayerState>(PC->PlayerState);
		if (PS)
		{
			PS->SubmitMessage(Message);
		}
	}
}

void ULobbyChat::NativeDestruct()
{
	Super::NativeDestruct();
}
