

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "EndGameTrigger.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AEndGameTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEndGameTrigger();

protected:

	UPROPERTY()
	class UBoxComponent* BoxComponent;

	UFUNCTION()
		void OnColide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
