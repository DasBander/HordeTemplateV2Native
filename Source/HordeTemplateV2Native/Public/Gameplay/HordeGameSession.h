

#pragma once

#include "CoreMinimal.h"
#include "Online.h"
#include "GameFramework/GameSession.h"
#include "HordeGameSession.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeGameSession : public AGameSession
{
	GENERATED_BODY()

public:

	void EndGameSession();
	
};
