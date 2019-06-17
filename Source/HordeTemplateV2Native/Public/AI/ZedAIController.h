

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "AIModule/Classes/Perception/AIPerceptionComponent.h"
#include "ZedAIController.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AZedAIController : public AAIController
{
	GENERATED_BODY()

public:
	AZedAIController();

	UFUNCTION()
		void EnemyInSight(AActor* Actor, FAIStimulus Stimulus);
protected:

	UPROPERTY()
		FTimerHandle SightClearTimer;

	UFUNCTION()
		void ClearSight();

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Target")
		class UAIPerceptionComponent* PCC;
};
