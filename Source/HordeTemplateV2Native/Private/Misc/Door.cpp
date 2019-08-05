

#include "Door.h"
#include "ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprintGeneratedClass.h"

ADoor::ADoor()
{
	SetReplicates(true);
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	DoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(RootComponent);
	DoorMesh->SetCollisionProfileName("BlockAll");
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> DoorMeshAsset(TEXT("SkeletalMesh'/Game/HordeTemplateBP/Assets/Meshes/Misc/SK_BaseDoor_Blue.SK_BaseDoor_Blue'"));
	if (DoorMeshAsset.Succeeded())
	{
		DoorMesh->SetSkeletalMesh(DoorMeshAsset.Object);
	}

	const ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> AnimBlueprint(TEXT("AnimBlueprint'/Game/HordeTemplateBP/Assets/Meshes/Misc/ABP_BaseDoor.ABP_BaseDoor_C'"));
	if (AnimBlueprint.Succeeded())
	{
		DoorMesh->AnimClass = AnimBlueprint.Object;
	}

}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADoor, bIsOpen);
}

void ADoor::Interact_Implementation(AActor* InteractingOwner)
{
	bIsOpen = !bIsOpen;
}


FInteractionInfo ADoor::GetInteractionInfo_Implementation()
{
	return FInteractionInfo((bIsOpen) ? "Close Door" : "Open Door", 2, true);
}
