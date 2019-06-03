

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
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
		TArray<FName> StartingItems = {"Item_Hands"};


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 MaxRounds = 13;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 PauseTime = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 RoundTime = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horde Game|Round Based")
		int32 ZedMultiplier = 2;
		
};
