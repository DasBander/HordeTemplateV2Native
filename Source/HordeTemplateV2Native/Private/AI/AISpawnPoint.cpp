

#include "AISpawnPoint.h"

// Sets default values
AAISpawnPoint::AAISpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAISpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAISpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

