

#include "HordeGameSession.h"
#include "OnlineSubsystem.h"

void AHordeGameSession::EndGameSession()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Session = OnlineSub->GetSessionInterface();
		if (Session.IsValid())
		{
			if (Session->EndSession(GameSessionName))
			{
				Session->DestroySession(GameSessionName);
			}
		}
	}
}
