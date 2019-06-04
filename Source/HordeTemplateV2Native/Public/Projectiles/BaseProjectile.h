

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseProjectile.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY()
		int32 ImpactCounter = 0;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float Damage = 25.f;



};
