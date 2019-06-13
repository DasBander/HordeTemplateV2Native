

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "AISpawnPoint.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AAISpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AAISpawnPoint();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		FName PatrolTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
		bool SpawnNotFree = false;


protected:
	UFUNCTION()
		void CharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void CharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
		class UBillboardComponent* ActorIcon;

};
