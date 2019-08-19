

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerState.h"
#include "PlayerScoreboardItem.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UPlayerScoreboardItem : public UUserWidget
{
	GENERATED_BODY()

public:
	/*
	Safe the Playerstate and expose it on spawn so we have the playerstate per player.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn = "true"))
		APlayerState* PlayerState;
	

	/*
	Function to get a color for the border depending on if the character is dead or not.
	*/
	UFUNCTION(BlueprintPure, Category = "Scoreboard Item")
		FLinearColor GetDeadBorderColor();

	UFUNCTION(BlueprintPure, Category = "Scoreboard Item")
		FText GetPlayerPing();

	UFUNCTION(BlueprintPure, Category = "Scoreboard Item")
		FText GetPlayerScore();

	UFUNCTION(BlueprintPure, Category = "Scoreboard Item")
		FText GetPlayerName();
};
