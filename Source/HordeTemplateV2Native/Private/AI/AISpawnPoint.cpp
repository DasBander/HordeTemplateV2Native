

#include "AISpawnPoint.h"
#include "Components/CapsuleComponent.h"
#include "ConstructorHelpers.h"
#include "GameFramework/Character.h"


/*
	FUNCTION: Constructor for AAISpawnPoint
	PARAM: None
	RETURN: None
	DESC:
	Create RootComponent, Capsule Component and Icon. Populates them with Default Values.
*/
AAISpawnPoint::AAISpawnPoint()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	RootComponent->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

	UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(RootComponent);
	if (Capsule)
	{
		Capsule->SetCapsuleHalfHeight(44.f, true);
		Capsule->SetCapsuleRadius(22.f);
		Capsule->OnComponentBeginOverlap.AddDynamic(this, &AAISpawnPoint::CharacterOverlap);
		Capsule->OnComponentEndOverlap.AddDynamic(this, &AAISpawnPoint::CharacterEndOverlap);
		Capsule->SetCollisionProfileName(FName("SpawnerCollision"));
	}


	ActorIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("Icon"));
	ActorIcon->SetupAttachment(RootComponent);
	ActorIcon->SetRelativeLocation(FVector(0.f, 0.f, 53.f));

	const ConstructorHelpers::FObjectFinder<UTexture2D> IconAsset(TEXT("Texture2D'/Engine/EditorResources/S_Actor.S_Actor'"));
	if (IconAsset.Succeeded())
	{
		ActorIcon->SetSprite(IconAsset.Object);
	}


}

/*
	FUNCTION: Character Overlap
	PARAM: UPrimitiveComponent ( Overlapped Component ), AActor ( Other Actor ), UPrimitiveComponent ( Other Component ), int32 ( Other Body Index ), bool ( bFromSweep ), FHitResult ( Sweep Result ) 
	RETURN: void
	DESC:
	Overlap with Spawn Collision.
	If Character is inside this Collision Spawn wont be free.
*/
void AAISpawnPoint::CharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ACharacter* PLY = Cast<ACharacter>(OtherActor);
		if (PLY)
		{
			SpawnNotFree = true;
		}
	}

}

/*
	FUNCTION: Character End Overlap
	PARAM: UPrimitiveComponent ( Overlapped Component ), AActor ( Other Actor ), UPrimitiveComponent ( Other Component ), int32 ( Other Body Index )
	RETURN: void
	DESC:
	If Character leaves freeup the Spawn.

*/
void AAISpawnPoint::CharacterEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		ACharacter* PLY = Cast<ACharacter>(OtherActor);
		if (PLY)
		{
			SpawnNotFree = false;
		}
	}
}

