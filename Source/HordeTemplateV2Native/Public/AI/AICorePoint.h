

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICorePoint.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AAICorePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAICorePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
