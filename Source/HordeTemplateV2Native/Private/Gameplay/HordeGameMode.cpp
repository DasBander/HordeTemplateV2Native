

#include "HordeGameMode.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeBaseController.h"
#include "Gameplay/HordeGameState.h"
#include "Gameplay/HordePlayerState.h"
#include "HUD/HordeBaseHUD.h"

AHordeGameMode::AHordeGameMode()
{
	GameStateClass = AHordeGameState::StaticClass();
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AHordeBaseController::StaticClass();
	PlayerStateClass = AHordePlayerState::StaticClass();
	HUDClass = AHordeBaseHUD::StaticClass();

	bStartPlayersAsSpectators = 0;
}

void AHordeGameMode::Logout(AController* Exiting)
{
	FTimerHandle DelayedRemove;
	FTimerDelegate DelayedRemoveDel;
	DelayedRemoveDel.BindLambda([=] {
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			GS->UpdatePlayerLobby();
		}
	});
	GetWorld()->GetTimerManager().SetTimer(DelayedRemove, DelayedRemoveDel, 1.f, false);

}


