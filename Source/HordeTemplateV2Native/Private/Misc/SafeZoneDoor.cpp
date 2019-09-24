

#include "SafeZoneDoor.h"
#include "ConstructorHelpers.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprintGeneratedClass.h"

/**
 * Constructor for ASafeZoneDoor
 *
 * @param
 * @return
 */
ASafeZoneDoor::ASafeZoneDoor()
{
	SetReplicates(true);
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	DoorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Door Mesh"));
	DoorMesh->SetupAttachment(RootComponent);
	DoorMesh->SetCollisionProfileName("BlockAll");
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> DoorMeshAsset(TEXT("SkeletalMesh'/Game/HordeTemplateBP/Assets/Meshes/Misc/SK_SafeZoneDoor.SK_SafeZoneDoor'"));
	if (DoorMeshAsset.Succeeded())
	{
		DoorMesh->SetSkeletalMesh(DoorMeshAsset.Object);
	}

	const ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> AnimBlueprint(TEXT("AnimBlueprint'/Game/HordeTemplateBP/Assets/Meshes/Misc/ABP_SafeZoneDoor.ABP_SafeZoneDoor_C'"));
	if (AnimBlueprint.Succeeded())
	{
		DoorMesh->AnimClass = AnimBlueprint.Object;
	}
}

/** ( Overridden )
 * Defines Replicated Props
 *
 * @param
 * @output Lifetime Props as Array.
 * @return void
 */
void ASafeZoneDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASafeZoneDoor, bIsOpen);
}

/** ( Interface )
 * Opens door.
 *
 * @param Interacting Actor
 * @return
 */
void ASafeZoneDoor::Interact_Implementation(AActor* InteractingOwner)
{
	if (!bIsOpen)
	{
		bIsOpen = true;
	}
}

/** ( Interface )
 * Returns Interaction Info.
 *
 * @param
 * @return Interaction Information.
 */
FInteractionInfo ASafeZoneDoor::GetInteractionInfo_Implementation()
{
	return FInteractionInfo((bIsOpen) ? "" : "Open Door", 2, true);
}



