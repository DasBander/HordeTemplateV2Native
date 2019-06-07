

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "Gameplay/HordeGameMode.h"
#include "HordeTemplateV2Native.h"
#include "HordeWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Player Starting")
		TArray<FName> StartingItems = DEFAULT_STARTING_ITEMS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Lobby")
		TArray<FName> PlayerCharacters = DEFAULT_AVAILABLE_PLAYERCHARACTERS;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Gameplay")
		EMatchMode MatchMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 MaxRounds = 13;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 PauseTime = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 RoundTime = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 ZedMultiplier = 2;
		
};
