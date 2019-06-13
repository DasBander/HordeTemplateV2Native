
#include "HordeTrader.h"
#include "HordeTemplateV2Native.h"
#include "Components/SceneComponent.h"
#include "Gameplay/HordeGameState.h"
#include "Gameplay/HordeBaseController.h"
#include "Gameplay/HordeWorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ConstructorHelpers.h"

AHordeTrader::AHordeTrader()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	TraderMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TraderMesh"));
	TraderMeshComponent->SetupAttachment(RootComponent);
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> TraderMeshAsset(TEXT("SkeletalMesh'/Game/HordeTemplateBP/Assets/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (TraderMeshAsset.Succeeded())
	{
		TraderMeshComponent->SetSkeletalMesh(TraderMeshAsset.Object);
		TraderMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		TraderMeshComponent->SetCollisionProfileName(TEXT("TraderCollision"));
	}
	const ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> TraderMeshAnimAsset(TEXT("AnimBlueprint'/Game/HordeTemplateBP/Assets/Mannequin/Animations/ABP_ThirdPerson.ABP_ThirdPerson_C'"));
	if (TraderMeshAnimAsset.Succeeded())
	{
		TraderMeshComponent->AnimClass = TraderMeshAnimAsset.Object;
	}
	TraderTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Trader Header Text"));
	TraderTextComponent->SetupAttachment(TraderMeshComponent);
	TraderTextComponent->SetRelativeLocation(FVector(-29.f, 0.f, 180.f));
	TraderTextComponent->SetRelativeRotation(FRotator(0.f, 90.f, 0.f).Quaternion());
	TraderTextComponent->SetText(FText::FromString("Trader"));
// 	const ConstructorHelpers::FObjectFinder<USoundCue> WelcomeSoundAsset(TEXT(""));
// 	if (WelcomeSoundAsset.Succeeded())
// 	{
// 		WelcomeSound = WelcomeSoundAsset.Object;
// 	}
}

void AHordeTrader::PlayGoodBye_Implementation()
{

}

bool AHordeTrader::PlayGoodBye_Validate()
{
	return true;
}

void AHordeTrader::PlayWelcome_Implementation()
{
	USoundCue* WelcomeSound = ObjectFromPath<USoundCue>(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/Trader/A_Trader_Welcome.A_Trader_Welcome'"));

	if (WelcomeSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WelcomeSound, TraderMeshComponent->GetComponentLocation());
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find Welcome Sound."));
	}
}

bool AHordeTrader::PlayWelcome_Validate()
{
	return true;
}

void AHordeTrader::Interact_Implementation(AActor* InteractingOwner)
{
	if (HasAuthority())
	{
		PlayWelcome();
		UE_LOG(LogTemp, Warning, TEXT("Character Interacted with Trader."));
	}

}


FInteractionInfo AHordeTrader::GetInteractionInfo_Implementation()
{
	FInteractionInfo InteractionInfo;
	InteractionInfo.InteractionText = FText::FromString("Talk to Trader");
	InteractionInfo.InteractionTime = 3.f;
	AHordeWorldSettings* WS = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings());
 	if (WS)
 	{
 		AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
 		if (GS)
 		{
 			InteractionInfo.AllowedToInteract = (WS->MatchMode == EMatchMode::EMatchModeNonLinear) ? GS->IsRoundPaused : true;
 		}
 	}
	
	return InteractionInfo;
}
