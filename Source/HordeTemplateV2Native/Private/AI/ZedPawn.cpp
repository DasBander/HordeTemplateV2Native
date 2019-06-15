

#include "ZedPawn.h"
#include "ZedAIController.h"
#include "Gameplay/HordeGameState.h"

#include "HordeTemplateV2Native.h"

AZedPawn::AZedPawn()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	const ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMeshAsset(TEXT("SkeletalMesh'/Game/HordeTemplateBP/Assets/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (PlayerMeshAsset.Succeeded()) {
		GetMesh()->SetSkeletalMesh(PlayerMeshAsset.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
		GetMesh()->SetCollisionProfileName(FName(TEXT("Zed")));
	}

	static ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> AnimBlueprint(TEXT("AnimBlueprint'/Game/HordeTemplateBP/Assets/Animations/Zombie/ABP_Zombie.ABP_Zombie_C'"));
	if (AnimBlueprint.Succeeded())
	{
		GetMesh()->AnimClass = AnimBlueprint.Object;
	}

	AttackPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(RootComponent);
	AttackPoint->SetRelativeLocation(FVector(0.f, 0.f, 56.f));

	PlayerRangeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Sphere"));
	PlayerRangeCollision->SetupAttachment(RootComponent);
	PlayerRangeCollision->SetRelativeLocation(FVector(74.f, 0.f, 0.f));
	PlayerRangeCollision->SetRelativeScale3D(FVector(3.8125f, 4.75f, 7.75f));
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AZedAIController::StaticClass();

}

void AZedPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZedPawn, Health);
	DOREPLIFETIME(AZedPawn, IsDead);
	DOREPLIFETIME(AZedPawn, PatrolTag);
}

void AZedPawn::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle DelayedBeginPlayHandle;
	FTimerDelegate DelayedBeginPlayDelegate;

	DelayedBeginPlayDelegate.BindLambda([=] {
		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			GS->UpdateAliveZeds();
		}
	});

	GetWorld()->GetTimerManager().SetTimer(DelayedBeginPlayHandle, DelayedBeginPlayDelegate, 1.f, false);
}


