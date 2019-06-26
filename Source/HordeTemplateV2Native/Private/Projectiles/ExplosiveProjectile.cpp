

#include "ExplosiveProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "HordeTemplateV2Native.h"
#include "Materials/MaterialInstanceConstant.h"
#include "FX/Camera/CameraShake_Explosion.h"
#include "ConstructorHelpers.h"
AExplosiveProjectile::AExplosiveProjectile()
{
	TracerMesh->SetRelativeLocation(FVector(-16.f, 0.f, 0.f));
	TracerMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f).Quaternion());
	TracerMesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));

	const ConstructorHelpers::FObjectFinder<UStaticMesh> ExplosiveMesh(TEXT("StaticMesh'/Game/HordeTemplateBP/Assets/Meshes/Misc/SM_ExplosiveRound.SM_ExplosiveRound'"));
	if (ExplosiveMesh.Succeeded())
	{
		TracerMesh->SetStaticMesh(ExplosiveMesh.Object);
	}

	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> ExplosiveRoundMatAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Materials/M_ExplosiveRound_Inst.M_ExplosiveRound_Inst'"));
	if (ExplosiveRoundMatAsset.Succeeded())
	{
		TracerMesh->SetMaterial(0, ExplosiveRoundMatAsset.Object);
	}

	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 15000.f;
	ProjectileMovement->ProjectileGravityScale = 2.f;

	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AExplosiveProjectile::OnProjectileImpact);
}

void AExplosiveProjectile::OnProjectileImpact(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (HasAuthority())
	{
		PlayWorldFX(ImpactResult.ImpactPoint);
		TArray<AActor*> IgnoredDamageActors = { GetOwner() };
		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), 150.f, 100.f, ImpactResult.ImpactPoint, 800.f, 1600.f, 1.f, nullptr, IgnoredDamageActors, GetOwner(), GetOwner()->GetInstigatorController(), ECC_Visibility);
	}
}

void AExplosiveProjectile::PlayWorldFX_Implementation(FVector Epicenter)
{
	USoundCue* ExpSound = ObjectFromPath<USoundCue>(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_GranadeExplosion.A_GranadeExplosion'"));
	if (ExpSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ExpSound, Epicenter);
	}
	UParticleSystem* ExpEmitter = ObjectFromPath<UParticleSystem>(TEXT("ParticleSystem'/Game/HordeTemplateBP/Assets/Effects/P_Explosion.P_Explosion'"));
	if (ExpEmitter)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExpEmitter, Epicenter);
	}


	UGameplayStatics::PlayWorldCameraShake(GetWorld(), UCameraShake_Explosion::StaticClass(), Epicenter, 0.f, 8000.f, 1.f, true);
}

bool AExplosiveProjectile::PlayWorldFX_Validate(FVector Epicenter)
{
	return true;
}
