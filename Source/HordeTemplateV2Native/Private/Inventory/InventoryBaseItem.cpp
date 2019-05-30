

#include "InventoryBaseItem.h"
#include "InventoryComponent.h"
#include "InventoryHelpers.h"

AInventoryBaseItem::AInventoryBaseItem()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetReplicates(true);


	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	WorldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldMesh"));
	WorldMesh->SetupAttachment(RootComponent);

}
void AInventoryBaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(AInventoryBaseItem, ItemInfo);
}

// Called when the game starts or when spawned
void AInventoryBaseItem::BeginPlay()
{
	Super::BeginPlay();
	
	PopInfo();
}

void AInventoryBaseItem::Interact_Implementation(AActor* InteractingOwner)
{
	UInventoryComponent* PlayerInventory = InteractingOwner->FindComponentByClass<UInventoryComponent>();
	if (PlayerInventory)
	{
		PlayerInventory->ServerAddItem(ItemID.ToString(), Spawned, ItemInfo);
		SetLifeSpan(0.1f);
	}
}


void AInventoryBaseItem::PopInfo()
{
	if (ItemID.ToString() != "None" && !Spawned)
	{
		ItemInfo = UInventoryHelpers::FindItemByID(ItemID);
		if (ItemInfo.WorldModel)
		{
			WorldMesh->SetStaticMesh(ItemInfo.WorldModel);
		}
	}
	else {
		ItemID = ItemInfo.ItemID;
		WorldMesh->SetSimulatePhysics(true);

	}
}

void AInventoryBaseItem::OnConstruction(const FTransform& Transform)
{
	PopInfo();
}

