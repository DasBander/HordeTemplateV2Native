

#include "HordeBaseController.h"
#include "Kismet/GameplayStatics.h"


void AHordeBaseController::ClientCloseTraderUI_Implementation()
{

}

void AHordeBaseController::ClientPlay2DSound_Implementation(USoundCue* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Sound);
	}
}

AHordeBaseController::AHordeBaseController()
{
	bAttachToPawn = true;
}
