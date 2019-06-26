

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "HordeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UHordeGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Server Creation")
		FString LobbyName = "Horde Game - Lobby";

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Server Creation")
		TArray<FName> MapRotation;
};
