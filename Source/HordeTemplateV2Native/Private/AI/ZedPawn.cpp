

#include "ZedPawn.h"
#include "ZedAIController.h"
#include "Gameplay/HordeGameState.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Character/HordeBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "HordeTemplateV2Native.h"

/**
 *	Constructor
 *
 * @param
 * @return
 */
AZedPawn::AZedPawn()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	const ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMeshAsset(TEXT("SkeletalMesh'/Game/HordeTemplateBP/Assets/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (PlayerMeshAsset.Succeeded()) {
		GetMesh()->SetSkeletalMesh(PlayerMeshAsset.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
		GetMesh()->SetCollisionProfileName(FName(TEXT("Zed")));
	}

	const ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> AnimBlueprint(TEXT("AnimBlueprint'/Game/HordeTemplateBP/Assets/Animations/Zombie/ABP_Zombie.ABP_Zombie_C'"));
	if (AnimBlueprint.Succeeded())
	{
		GetMesh()->AnimClass = AnimBlueprint.Object;
	}

	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> ZedCharacterMaterial(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Zombie.M_UE4Man_Zombie'"));
	if (ZedCharacterMaterial.Succeeded())
	{
		GetMesh()->SetMaterial(0, ZedCharacterMaterial.Object);
	}

	AttackPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttackPoint"));
	AttackPoint->SetupAttachment(RootComponent);
	AttackPoint->SetRelativeLocation(FVector(0.f, 0.f, 56.f));

	ZedIdleSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Zed Idle Sound"));
	ZedIdleSound->SetupAttachment(RootComponent);

	const ConstructorHelpers::FObjectFinder<USoundCue> ZedIdleSoundAsset(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_Zed_RND_Idle.A_Zed_RND_Idle'"));
	if (ZedIdleSoundAsset.Succeeded())
	{
		ZedIdleSound->SetSound(ZedIdleSoundAsset.Object);
	}
	PlayerRangeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Player Collision Sphere"));
	PlayerRangeCollision->SetupAttachment(RootComponent);
	PlayerRangeCollision->SetRelativeLocation(FVector(74.f, 0.f, 0.f));
	PlayerRangeCollision->SetRelativeScale3D(FVector(3.8125f, 4.75f, 7.75f));
	PlayerRangeCollision->SetCollisionProfileName("CharacterDetection");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AZedAIController::StaticClass();

	PlayerRangeCollision->OnComponentBeginOverlap.AddDynamic(this, &AZedPawn::OnCharacterInRange);
	PlayerRangeCollision->OnComponentEndOverlap.AddDynamic(this, &AZedPawn::OnCharacterOutRange);


	GetCharacterMovement()->MaxWalkSpeed = 200.f;

}


/**
 *	Sets replicated variables.
 *
 * @param OutLifetimeProps
 * @return void
 */
void AZedPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AZedPawn, Health);
	DOREPLIFETIME(AZedPawn, IsDead);
	DOREPLIFETIME(AZedPawn, PatrolTag);
}


/**
 *	Begin Play -> Sets Patrol Tag in Blackboard and Updates Alive Zombies in GameState.
 *
 * @param
 * @return void
 */
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
			AZedAIController* AIC = Cast<AZedAIController>(GetController());
			if (AIC && AIC->GetBlackboardComponent())
			{
				AIC->GetBlackboardComponent()->SetValueAsName("PatrolTag", PatrolTag);
			}
		}
	});

	GetWorld()->GetTimerManager().SetTimer(DelayedBeginPlayHandle, DelayedBeginPlayDelegate, 1.f, false);
}



/**
 *	Gives specified player points.
 *
 * @param The Player Character to give points, the amount of points and the point type.
 * @return void
 */
void AZedPawn::GivePlayerPoints(ACharacter* Player, int32 Points, EPointType PointType)
{
	AHordeBaseCharacter* Char = Cast<AHordeBaseCharacter>(Player);
	if (Char)
	{
		AHordePlayerState* PS = Cast<AHordePlayerState>(Char->GetPlayerState());
		if (PS)
		{
			PS->AddPoints(Points, PointType);
		}
	}
}



/**
 *	Received Damage and executes the following functions depending on the AI's health. If AI gets an head shot it dies instantly.
 *
 * @param The Damage, Damage Event, Event Instigator and the Damage Causer.
 * @return
 */
float AZedPawn::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		if (!IsDead)
		{
			ACharacter* PLY = Cast<ACharacter>(DamageCauser);
			if (PLY)
			{
				KillAI(PLY, EPointType::EPointCasual);
			}
		}
	}
	else if(DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		if (!IsDead)
		{
			FHitResult HitRes;
			FVector HitDirection;
			DamageEvent.GetBestHitInfo(this, DamageCauser, HitRes, HitDirection);
			if (HitRes.BoneName != NAME_None)
			{
				if (HitRes.BoneName == "head")
				{
					PlayHeadShotFX();
					DeathFX(HitDirection);
					ACharacter* PLY = Cast<ACharacter>(DamageCauser);
					if (PLY)
					{
						KillAI(PLY, EPointType::EPointHeadShot);
					}
				}
				else
				{
					Health = FMath::Clamp<float>((Health - Damage), 0.f, 100.f);
					if (Health <= 0)
					{
						DeathFX(HitDirection);
						ACharacter* PLY = Cast<ACharacter>(DamageCauser);
						if (PLY)
						{
							KillAI(PLY, EPointType::EPointCasual);
						}
					}
				}
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("ZedPawn: Point Damage BoneName == NAME_None! There might be a collision in the way."));
			}
			
		}
	}
	
	return Health;
}


/**
 *	Sets AI as Dead. Gives killer points depending on kill type and updates alive zombies.
 *
 * @param ACharacter as killer and the Point Type
 * @return void
 */
void AZedPawn::KillAI(ACharacter* Killer, EPointType KillType)
{
	IsDead = true;
	Health = 0.f;

	switch (KillType) {
	case EPointType::EPointCasual:
		GivePlayerPoints(Killer, 100, KillType);
		break;

	case EPointType::EPointHeadShot:
		GivePlayerPoints(Killer, 250, KillType);
		break;

	default:
		break;
	}

	AZedAIController* AIC = Cast<AZedAIController>(GetController());
	if (AIC && AIC->GetBlackboardComponent())
	{
		AIC->GetBlackboardComponent()->SetValueAsBool("IsDead", true);
	}
	DeathFX(FVector(0.f, 0.f, 0.f));
	SetLifeSpan(10.f);
	

	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->UpdateAliveZeds();
	}

}


/** ( Bound to Delegate )
 *	Check if Player is colliding with AI. Sets the Blackboard Variable "PlayerInRange".
 *
 * @param UPrimitiveComponent ( Overlapping Component ), AActor ( Other Actor ), UPrimitiveComponent ( Other Component ), int32 ( Other Body Index ), bool ( bFromSweep ), FHitResult ( Sweep Result )
 * @return void
 */
void AZedPawn::OnCharacterInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHordeBaseCharacter* Char = Cast<AHordeBaseCharacter>(OtherActor);
	if (Char)
	{
		AZedAIController* AIC = Cast<AZedAIController>(GetController());
		if (AIC && AIC->GetBlackboardComponent())
		{
			AIC->GetBlackboardComponent()->SetValueAsBool("PlayerInRange", true);
		}
	}
}

/** ( Bound to Delegate )
 *	Resets BlackboardKey "PlayerInRange" if Horde Character stepped out of collision.
 *
 * @param UPrimitiveComponent ( Overlapping Component ), AActor ( Other Actor ), UPrimitiveComponent ( Other Component ), int32 ( Other Body Index )
 * @return void
 */
void AZedPawn::OnCharacterOutRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AHordeBaseCharacter* Char = Cast<AHordeBaseCharacter>(OtherActor);
	if (Char)
	{
		AZedAIController* AIC = Cast<AZedAIController>(GetController());
		if (AIC && AIC->GetBlackboardComponent())
		{
			AIC->GetBlackboardComponent()->SetValueAsBool("PlayerInRange", false);
		}
	}
}


/** ( Multicast )
 *	Simulates Physics - Stops Sounds and plays death sound.
 *
 * @param FVector ( Direction "Impact Direction where it got shot from.")
 * @return
 */
void AZedPawn::DeathFX_Implementation(FVector Direction)
{
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("DeadAI")));
	ZedIdleSound->Stop();

	USoundCue* DeathSound = ObjectFromPath<USoundCue>(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_ZedDeath.A_ZedDeath'"));
	if (DeathSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DeathSound, GetMesh()->GetComponentLocation());
	}

	if (GetMesh()->IsSimulatingPhysics(NAME_None))
	{
		GetMesh()->AddForce(Direction * 500.f, NAME_None, true);
	}
}

bool AZedPawn::DeathFX_Validate(FVector Direction)
{
	return true;
}


/**	( Multicast )
 *	Plays Attack Animation on all Clients. Also plays randomly AttackSound.
 *
 * @param
 * @return void
 */
void AZedPawn::PlayAttackFX_Implementation()
{
	UAnimMontage* AttackAnimation = ObjectFromPath<UAnimMontage>(TEXT("AnimMontage'/Game/HordeTemplateBP/Assets/Animations/Zombie/A_UEZomb_Attack_Montage.A_UEZomb_Attack_Montage'"));
	if (GetMesh()->GetAnimInstance() && AttackAnimation)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(AttackAnimation);
	}

	if (FMath::RandBool())
	{
		USoundCue* AttackSound = ObjectFromPath<USoundCue>(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_ZedAttack.A_ZedAttack'"));
		if (AttackSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), AttackSound, AttackPoint->GetComponentLocation());
		}
	}
}

bool AZedPawn::PlayAttackFX_Validate()
{
	return true;
}


/**	( Multicast )
 *	Hides head bone and spawns head shot particle on the head bone. Also spawns head shot sound.
 *
 * @param
 * @return void
 */
void AZedPawn::PlayHeadShotFX_Implementation()
{
	//Hide Headbone
	GetMesh()->HideBone(GetMesh()->GetBoneIndex("head"), EPhysBodyOp::PBO_None);

	//Spawn Blood Splatter
	UParticleSystem* BloodSplat = ObjectFromPath<UParticleSystem>(TEXT("ParticleSystem'/Game/HordeTemplateBP/Assets/Effects/ParticleSystems/Gameplay/Player/P_bloodSplatter.P_bloodSplatter'"));
	if (BloodSplat)
	{
		/*
		We need to use SpawnEmitterAtLocation instead of Attached. https://issues.unrealengine.com/issue/UE-29018
		*/
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodSplat, GetMesh()->GetBoneLocation("head", EBoneSpaces::WorldSpace), FRotator(0.f, 0.f, 0.f), true, EPSCPoolMethod::None);
	}
	
	//Spawn Headshot Sound at head.
	USoundCue* GoreShotSound = ObjectFromPath<USoundCue>(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_Horde_Goreshot_Cue.A_Horde_Goreshot_Cue'"));
	if (GoreShotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), GoreShotSound, GetMesh()->GetSocketLocation("head"));
	}

}

bool AZedPawn::PlayHeadShotFX_Validate()
{
	return true;
}


/**	( Multicast )
 *	Sets the Walkspeed on all clients.
 *
 * @param New Max Walkspeed
 * @return void
 */
void AZedPawn::ModifyWalkSpeed_Implementation(float MaxWalkSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
}

bool AZedPawn::ModifyWalkSpeed_Validate(float MaxWalkSpeed)
{
	return true;
}

