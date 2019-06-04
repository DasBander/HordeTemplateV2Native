

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "BaseProjectile.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseProjectile();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class UStaticMeshComponent* TracerMesh;
	
	UPROPERTY()
		int32 ImpactCounter = 0;

	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	float Damage = 25.f;

	UFUNCTION()
		void OnProjectileStop(const FHitResult& ImpactResult);

	UFUNCTION()
		void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

};
