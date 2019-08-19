

#include "PlayerScoreboardItem.h"
#include "Gameplay/HordePlayerState.h"

FLinearColor UPlayerScoreboardItem::GetDeadBorderColor()
{
	FLinearColor RetColor = FLinearColor();
	AHordePlayerState* PS = Cast<AHordePlayerState>(PlayerState);
	if (PS)
	{
		
		if (PS->bIsDead)
		{
			RetColor = FLinearColor(0.229167f, 0.f, 0.f, 0.8f);
		}
		else {
			RetColor = FLinearColor(0.010417f, 0.010417f, 0.010417f, 0.8f);
		}
	}
	return RetColor;
}

FText UPlayerScoreboardItem::GetPlayerPing()
{
	FString RetPing = "/";
	AHordePlayerState* PS = Cast<AHordePlayerState>(PlayerState);
	if (PS)
	{
		RetPing = FString::FromInt(PS->Ping) + " Ms";
	}
	return FText::FromString(RetPing);
}

FText UPlayerScoreboardItem::GetPlayerScore()
{
	FString RetScore = "/";
	AHordePlayerState* PS = Cast<AHordePlayerState>(PlayerState);
	if (PS)
	{
		RetScore = FString::FromInt(PS->Points) + " Points";
	}
	return FText::FromString(RetScore);
}

FText UPlayerScoreboardItem::GetPlayerName()
{
	FString RetUsername = "Unknown";
	AHordePlayerState* PS = Cast<AHordePlayerState>(PlayerState);
	if (PS)
	{
		RetUsername = PS->GetPlayerInfo().UserName;
	}
	return FText::FromString(RetUsername);
}
