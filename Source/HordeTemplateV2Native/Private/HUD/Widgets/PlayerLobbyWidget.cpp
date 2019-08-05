

#include "PlayerLobbyWidget.h"
#include "Gameplay/HordeGameState.h"
#include "Gameplay/HordePlayerState.h"
#include "Gameplay/HordeBaseController.h"
#include "HordeTemplateV2Native.h"
#include "Kismet/GameplayStatics.h"

void UPlayerLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//Bind Delegate to ImplementableEvent
	OnLobbyPlayersUpdateDelegate.AddDynamic(this, &UPlayerLobbyWidget::OnLobbyPlayersUpdated);
	bIsFocusable = true;
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

bool UPlayerLobbyWidget::IsDisconnectBlocked()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		return !GS->BlockDisconnect;
	}
	else
	{
		return false;
	}
}

ESlateVisibility UPlayerLobbyWidget::IsCharacterTrading()
{
	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwningPlayer());
	if (PC)
	{
		AHordePlayerState* PS = Cast<AHordePlayerState>(PC->PlayerState);
		if (PS)
		{
			AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
			if (GS)
			{
				if (GS->IsTradeInProgress && (GS->TradeProgress.Instigator == PS->GetPlayerInfo().PlayerID || GS->TradeProgress.Target == PS->GetPlayerInfo().PlayerID))
				{
					return ESlateVisibility::Visible;
				}
				else
				{
					return ESlateVisibility::Collapsed;
				}
			}
			else {
				return ESlateVisibility::Collapsed;
			}
		}
		else {
			return ESlateVisibility::Collapsed;
		}
	}
	else {
		return ESlateVisibility::Collapsed;
	}
}

bool UPlayerLobbyWidget::bIsGameStarting()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		return !GS->GameStarting;
	}
	else {
		return false;
	}
}
