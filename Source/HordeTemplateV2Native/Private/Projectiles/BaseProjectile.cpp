

#include "BaseProjectile.h"

ABaseProjectile::ABaseProjectile()
{
 
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetReplicates(true);

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

}


void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}


