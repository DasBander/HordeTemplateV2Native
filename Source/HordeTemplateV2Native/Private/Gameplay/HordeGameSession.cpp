

#include "HordeGameSession.h"
#include "OnlineSubsystem.h"

/*
	FUNCTION: End Game Session
	PARAM: None
	RETURN: void
	DESC:
	Ends the current game session in the Online Subsystem.
*/
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
