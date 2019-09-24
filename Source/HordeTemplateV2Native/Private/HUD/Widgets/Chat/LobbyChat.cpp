

#include "LobbyChat.h"
#include "Gameplay/HordePlayerState.h"

/** ( Virtual; Overridden )
 * Binds delegate for On Message Received Function.
 *
 * @param
 * @return void
 */
void ULobbyChat::NativeConstruct()
{
	Super::NativeConstruct();

	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		PC->OnLobbyMessageReceivedDelegate.AddDynamic(this, &ULobbyChat::OnMessageReceived);
	}
}

/**
 * Submits a chat message with given text to the server.
 *
 * @param The Text Message.
 * @return void
 */
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

/** ( Virtual; Overridden )
 *	Getting called if widget gets destroyed.
 *
 * @param
 * @return void
 */
void ULobbyChat::NativeDestruct()
{
	Super::NativeDestruct();
}
