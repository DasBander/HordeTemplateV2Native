

#include "HordeGameSession.h"
#include "OnlineSubsystem.h"

void AHordeGameSession::EndGameSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling to End the Session"));
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			if (Session->EndSession(GameSessionName))
			{
				Session->DestroySession(GameSessionName);
				UE_LOG(LogTemp, Warning, TEXT("Session Destroyed."));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Failed to End Session!"));
			}
		}
	}
}
