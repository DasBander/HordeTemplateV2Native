

#include "PlayerTravelWidget.h"
#include "Gameplay/HordeGameState.h"
#include "Kismet/GameplayStatics.h"

FText UPlayerTravelWidget::GetServerInfo()
{
	FString RetInfo = "Getting Serverinfo.....";
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		FString MapName = "None";
		if (GS->NextLevel != NAME_None)
		{
			MapName = GS->NextLevel.ToString();
		}
		else {
			MapName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
		}
		RetInfo = "Loading " + MapName;
	}
	return FText::FromString(RetInfo);
}
