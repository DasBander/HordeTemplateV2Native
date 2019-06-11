

#include "PlayerLobbyWidget.h"
#include "Gameplay/HordeGameState.h"
#include "HordeTemplateV2Native.h"
#include "Kismet/GameplayStatics.h"

void UPlayerLobbyWidget::NativeConstruct()
{
	//Bind Delegate to ImplementableEvent
	OnLobbyPlayersUpdateDelegate.AddDynamic(this, &UPlayerLobbyWidget::OnLobbyPlayersUpdated);

	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		LobbyInfo = GS->LobbyInformation;

		UDataTable* DTMaps = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, MAPS_DATATABLE_PATH));
		if (DTMaps) {
			FPlayableLevel* NewMapInfo = DTMaps->FindRow<FPlayableLevel>(FName(*UGameplayStatics::GetCurrentLevelName(GetWorld(), true)), "DataTableFinder", true);
			if (NewMapInfo)
			{
				MapInfo = *NewMapInfo;
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Unable to Read Playable Level."));
			}

		}
	}
}

FText UPlayerLobbyWidget::GetLobbyTime()
{
	FFormatNamedArguments Args;
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		float LobbyTime = GS->LobbyTime;

		int32 Minutes = FMath::FloorToInt(LobbyTime / 60.f);
		int32 Seconds = FMath::TruncToInt(LobbyTime - (Minutes * 60.f));

		FString TimeStr = FString::Printf(TEXT("%s%s : %s%s"), (Minutes < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Minutes), (Seconds < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Seconds));

		return FText::FromString(TimeStr);
	}
	else {
		return FText::FromString("nA / nA");
	}

}
