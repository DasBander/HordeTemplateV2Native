

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "HordeGameMode.generated.h"


UENUM(BlueprintType)
enum class EMatchMode : uint8
{
	EMatchModeLinear UMETA(DisplayName = "Linear"),
	EMatchModeNonLinear UMETA(DisplayName = "Nonlinear")
};


/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AHordeGameMode();

	virtual void Logout(AController* Exiting) override;
};
