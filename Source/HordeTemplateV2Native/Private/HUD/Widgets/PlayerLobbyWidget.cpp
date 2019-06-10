

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
		if (FMath::FloorToInt((GS->LobbyTime / 60)) < 10)
		{
			Args.Add("Minutes", FText::FromString("0" + FString::FromInt(FMath::FloorToInt((GS->LobbyTime / 60)))));
		}
		else {
			Args.Add("Minutes", FText::FromString(FString::FromInt(FMath::FloorToInt((GS->LobbyTime / 60)))));
		}
		
		if (FMath::TruncToInt(GS->LobbyTime - (FMath::FloorToInt((GS->LobbyTime / 60)) * 60.f)) < 10)
		{
			Args.Add("Seconds", FText::FromString("0" + FString::FromInt(FMath::TruncToInt(GS->LobbyTime - (FMath::FloorToInt((GS->LobbyTime / 60)) * 60.f)))));
		}
		else {
			Args.Add("Seconds", FText::FromString(FString::FromInt(FMath::TruncToInt(GS->LobbyTime - (FMath::FloorToInt((GS->LobbyTime / 60)) * 60.f)))));
		}

		return FText::Format(NSLOCTEXT("LobbyTime", "LobbyTime", "{Minutes} : {Seconds}"), Args);
	}
	else {
		return FText::FromString("Error.");
	}

}
