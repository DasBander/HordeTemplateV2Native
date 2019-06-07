

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AISpawnPoint.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AAISpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAISpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
