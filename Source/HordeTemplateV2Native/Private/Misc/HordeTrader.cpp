
#include "HordeTrader.h"
#include "HordeTemplateV2Native.h"
#include "Components/SceneComponent.h"
#include "Gameplay/HordeGameState.h"
#include "Gameplay/HordeBaseController.h"
#include "Gameplay/HordeWorldSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimBlueprintGeneratedClass.h"
#include "Animation/AnimInstance.h"
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
	UAnimMontage* WelcomeAnimation = ObjectFromPath<UAnimMontage>(TEXT("AnimMontage'/Game/HordeTemplateBP/Assets/Mannequin/Animations/A_Trader_Welcome_Montage.A_Trader_Welcome_Montage'"));
	if (WelcomeSound && WelcomeAnimation)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WelcomeSound, TraderMeshComponent->GetComponentLocation());
		TraderMeshComponent->GetAnimInstance()->Montage_Play(WelcomeAnimation);
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
	
		AHordeBaseController* PC = Cast<AHordeBaseController>(InteractingOwner->GetInstigatorController());
		if (PC)
		{
			PC->ClientOpenTraderUI();
		}
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
