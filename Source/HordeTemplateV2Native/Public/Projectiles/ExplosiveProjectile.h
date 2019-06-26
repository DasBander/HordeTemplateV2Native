

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/BaseProjectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AExplosiveProjectile : public ABaseProjectile
{
	GENERATED_BODY()
	
public:

	AExplosiveProjectile();

	UFUNCTION()
		void OnProjectileImpact(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION(NetMulticast, WithValidation, Reliable, Category = "FX")
		void PlayWorldFX(FVector Epicenter);
};
