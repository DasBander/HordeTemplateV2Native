

#include "HordeGameSession.h"
#include "OnlineSubsystem.h"

/**
 *	Ends the current game session in the Online Subsystem.
 *	
 * @param
 * @return void
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
