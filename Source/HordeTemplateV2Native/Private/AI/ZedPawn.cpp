

#include "ZedPawn.h"

AZedPawn::AZedPawn()
{

	PrimaryActorTick.bCanEverTick = true;
	AttackPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(RootComponent);
	AttackPoint->SetRelativeLocation(FVector(0.f, 0.f, 56.f));

	PlayerRangeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Sphere"));
	PlayerRangeCollision->SetupAttachment(RootComponent);
	PlayerRangeCollision->SetRelativeLocation(FVector(74.f, 0.f, 0.f));
	PlayerRangeCollision->SetRelativeScale3D(FVector(3.8125f, 4.75f, 7.75f));

	
}

void AZedPawn::BeginPlay()
{
	Super::BeginPlay();
	
}


