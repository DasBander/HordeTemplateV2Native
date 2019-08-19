

#include "LobbyCharacterTradeWidget.h"
#include "Gameplay/HordeGameState.h"
#include "Gameplay/HordePlayerState.h"
#include "HordeTemplateV2Native.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

FText ULobbyCharacterTradeWidget::GetTradeTime()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		float LobbyTime = GS->TradeProgress.TimeLeft;

		int32 Minutes = FMath::FloorToInt(LobbyTime / 60.f);
		int32 Seconds = FMath::TruncToInt(LobbyTime - (Minutes * 60.f));

		FString TimeStr = FString::Printf(TEXT("%s%s : %s%s"), (Minutes < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Minutes), (Seconds < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Seconds));

		return FText::FromString(TimeStr);
	}
	else {
		return FText::FromString("nA / nA");
	}
}

FText ULobbyCharacterTradeWidget::GetTargetCharacterName()
{
	FString RetStr = "None";
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		int32 TempIndex;
		FPlayableCharacter Char = FromDatatable<FPlayableCharacter>(CHARACTER_DATATABLE_PATH, GS->GetCharacterByID(GS->TradeProgress.Target, TempIndex));
		RetStr = Char.CharacterTitle;
	}

	return FText::FromString(RetStr);
}

FSlateBrush ULobbyCharacterTradeWidget::GetTargetCharacterImage()
{
	FSlateBrush RetBrush;
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		int32 TempIndex;
		FPlayableCharacter Char = FromDatatable<FPlayableCharacter>(CHARACTER_DATATABLE_PATH, GS->GetCharacterByID(GS->TradeProgress.Target, TempIndex));
		RetBrush = UWidgetBlueprintLibrary::MakeBrushFromTexture(Char.CharacterImage, 64, 64);
	}
	return RetBrush;
}

FText ULobbyCharacterTradeWidget::GetOwnCharacterName()
{
	FString RetStr = "None";
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		int32 TempIndex;
		FPlayableCharacter Char = FromDatatable<FPlayableCharacter>(CHARACTER_DATATABLE_PATH, GS->GetCharacterByID(GS->TradeProgress.Instigator, TempIndex));
		RetStr = Char.CharacterTitle;
	}

	return FText::FromString(RetStr);
}

FSlateBrush ULobbyCharacterTradeWidget::GetOwnCharacterImage()
{
	FSlateBrush RetBrush;
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		int32 TempIndex;
		FPlayableCharacter Char = FromDatatable<FPlayableCharacter>(CHARACTER_DATATABLE_PATH, GS->GetCharacterByID(GS->TradeProgress.Instigator, TempIndex));
		RetBrush = UWidgetBlueprintLibrary::MakeBrushFromTexture(Char.CharacterImage, 64, 64);
	}
	return RetBrush;
}

ESlateVisibility ULobbyCharacterTradeWidget::IsInCharacterTrade()
{
	ESlateVisibility RetVis = ESlateVisibility::Collapsed;
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		AHordePlayerState* PS = Cast<AHordePlayerState>(GetOwningPlayer()->PlayerState);
		if (PS)
		{
			if (GS->TradeProgress.Target == PS->GetPlayerInfo().PlayerID)
			{
				RetVis = ESlateVisibility::Visible;
			}
		}
	}
	return RetVis;
}
