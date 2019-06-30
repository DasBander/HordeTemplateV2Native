

#include "BaseImpact.h"

// Sets default values
ABaseImpact::ABaseImpact()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseImpact::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseImpact::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

