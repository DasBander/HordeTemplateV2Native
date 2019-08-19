

#include "PlayerEndScreen.h"
#include "Gameplay/HordeGameState.h"
#include "HordeTemplateV2Native.h"

FText UPlayerEndScreen::GetMVPName()
{
	FText RetText = FText::FromString("None");
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		RetText = FText::FromString(*GS->Score_MVP.ScoreType);
	}
	return RetText;
}

FText UPlayerEndScreen::GetMHSName()
{
	FText RetText = FText::FromString("None");
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		RetText = FText::FromString(*GS->Score_MostHeadshots.ScoreType);
	}
	return RetText;
}

FText UPlayerEndScreen::GetMKSName()
{
	FText RetText = FText::FromString("None");
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		RetText = FText::FromString(*GS->Score_MostKills.ScoreType);
	}
	return RetText;
}

FText UPlayerEndScreen::GetMVPPoints()
{
	FText RetText = FText::FromString("0");
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		RetText = FText::FromString(*FString::FromInt(GS->Score_MVP.Score));
	}
	return RetText;
}

FText UPlayerEndScreen::GetMHSPoints()
{
	FText RetText = FText::FromString("0");
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		RetText = FText::FromString(*FString::FromInt(GS->Score_MostHeadshots.Score));
	}
	return RetText;
}

FText UPlayerEndScreen::GetMKSPoints()
{
	FText RetText = FText::FromString("0");
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		RetText = FText::FromString(*FString::FromInt(GS->Score_MostKills.Score));
	}
	return RetText;
}

FText UPlayerEndScreen::GetEndTime()
{
	FFormatNamedArguments Args;
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		float LobbyTime = GS->EndTime;

		int32 Minutes = FMath::FloorToInt(LobbyTime / 60.f);
		int32 Seconds = FMath::TruncToInt(LobbyTime - (Minutes * 60.f));

		FString TimeStr = FString::Printf(TEXT("%s%s : %s%s"), (Minutes < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Minutes), (Seconds < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Seconds));

		return FText::FromString(TimeStr);
	}
	else {
		return FText::FromString("nA / nA");
	}
}

FText UPlayerEndScreen::GetNextLevel()
{
	FString RetText = "NEXT: Not found!";
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		FPlayableLevel Ply = FindLevelByID(GS->NextLevel);
		if (Ply.RawLevelName != NAME_None)
		{
			RetText = "NEXT: " + Ply.LevelName.ToString();
		}
	}

	return FText::FromString(RetText);
}

FPlayableLevel UPlayerEndScreen::FindLevelByID(FName LevelID)
{
	UDataTable* InventoryData = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, MAPS_DATATABLE_PATH));
	FPlayableLevel TempItem;

	if (InventoryData) {
		FPlayableLevel* ItemFromRow = InventoryData->FindRow<FPlayableLevel>(LevelID, "PlayerEndScreen Widget - Find Level By ID", false);
		if (ItemFromRow)
		{
			TempItem = *ItemFromRow;
		}
	}
	else {
		GLog->Log("Player Maps Data not valid.");
	}

	return TempItem;
}