

#include "Med_VAC.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/HordeBaseCharacter.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "ConstructorHelpers.h"

void AMed_VAC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMed_VAC, IsHealing);
}


AMed_VAC::AMed_VAC()
{
	Weapon->SetSkeletalMesh(nullptr);
	VACMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Vac Mesh"));
	VACMesh->SetupAttachment(RootComponent);
	VACMesh->SetRelativeScale3D(FVector(.5f, .5f, .5f));

	const ConstructorHelpers::FObjectFinder<UStaticMesh>  VACMeshAsset(TEXT("StaticMesh'/Game/HordeTemplateBP/Assets/Meshes/Misc/SM_VAC.SM_VAC'"));
	if (VACMeshAsset.Succeeded())
	{
		VACMesh->SetStaticMesh(VACMeshAsset.Object);
		VACMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AMed_VAC::FireFirearm()
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Should start Healing..."));
		AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
		if (PLY && PLY->GetHealth() < 100.f && LoadedAmmo > 0 && !IsHealing)
		{
			IsHealing = true;

			//Simulate Character Reloading so he can't drop the weapon or switch something.
			PLY->Reloading = true;

			StartHealing();
		}
	}

}

void AMed_VAC::FinishHealing_Implementation()
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
	if (PLY)
	{
		IsHealing = false;
		PLY->AddHealth(15.f);
		LoadedAmmo--;
		PLY->Reloading = false;
		PLY->Inventory->UpdateCurrentItemAmmo(LoadedAmmo);
		PLY->Inventory->ServerDropItem(this);
	}
}

bool AMed_VAC::FinishHealing_Validate()
{
	return true;
}

void AMed_VAC::StartHealing_Implementation()
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
	if (PLY)
	{
		UAnimMontage* VACHealAnimation = ObjectFromPath<UAnimMontage>(TEXT("AnimMontage'/Game/HordeTemplateBP/Assets/Animations/AM_VAC_Insert.AM_VAC_Insert'"));
		if (VACHealAnimation)
		{
			PLY->GetMesh()->GetAnimInstance()->Montage_Play(VACHealAnimation);
			FTimerHandle FinishTimer;
			FTimerDelegate FinishTimerDelegate;
			FinishTimerDelegate.BindLambda([=] {
				FinishHealing();
			});
			GetWorld()->GetTimerManager().SetTimer(FinishTimer, FinishTimerDelegate, VACHealAnimation->CalculateSequenceLength() / 2.f, false);
		}
	}
}

bool AMed_VAC::StartHealing_Validate()
{
	return true;
}
