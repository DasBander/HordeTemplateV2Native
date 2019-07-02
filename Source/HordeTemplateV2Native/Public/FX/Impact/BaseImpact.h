

#pragma once

#include "CoreMinimal.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/Actor.h"
#include "BaseImpact.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API ABaseImpact : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseImpact();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "FX")
		class UParticleSystemComponent* ImpactFX;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "FX")
		class UAudioComponent* ImpactSound;

};
