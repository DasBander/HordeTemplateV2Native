

#include "InventoryComponent.h"
#include "HordeTemplateV2Native.h"
#include "InventoryBaseItem.h"
#include "Character/HordeBaseCharacter.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, ActiveItemIndex);
	DOREPLIFETIME(UInventoryComponent, AvailableAmmoForCurrentWeapon);
}


const TArray<FItem>& UInventoryComponent::GetInventory()
{
	return Inventory;
}

void UInventoryComponent::UpdateCurrentItemAmmo(int32 Ammo)
{
	if (Inventory.IsValidIndex(ActiveItemIndex))
	{
		Inventory[ActiveItemIndex].UpdateAmmo(Ammo);
	}
}

void UInventoryComponent::RefreshCurrentAmmoForItem()
{
	if (Inventory.IsValidIndex(ActiveItemIndex))
	{
		int32 TempIndex;
		AvailableAmmoForCurrentWeapon = CountAmmo(Inventory[ActiveItemIndex].AmmoType, TempIndex);
	}
}

void UInventoryComponent::ServerAddItem_Implementation(const FString& ItemID, bool Custom, FItem CustomItem)
{
	FItem FoundItm;
	if (GetItemByID(ItemID, FoundItm))
	{
		int32 ExistingItemIndex;
		EActiveType ExistingActiveType;
		if (InventoryItemExists(ItemID, ExistingItemIndex, ExistingActiveType))
		{
			if (FoundItm.Type != EActiveType::EActiveAmmo)
			{
				AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
				if (PLY)
				{
					ServerDropItem(PLY->GetCurrentFirearm());
					int32 NewAddedIndex = Inventory.Add((Custom) ? CustomItem : FoundItm);
					if (FoundItm.Type != EActiveType::EActiveAmmo)
					{
						ActiveItemIndex = NewAddedIndex;
						OnActiveItemChanged.Broadcast(Inventory[NewAddedIndex].ItemID.ToString(), NewAddedIndex, Inventory[NewAddedIndex].DefaultLoadedAmmo);
						
					}
					RefreshCurrentAmmoForItem();
				}
				
			}
			else {
				Inventory[ExistingItemIndex].UpdateAmmo(Inventory[ExistingItemIndex].DefaultLoadedAmmo + FoundItm.DefaultLoadedAmmo);
				RefreshCurrentAmmoForItem();
			}
		}
		else {
			int32 NewAddedIndex = Inventory.Add((Custom) ? CustomItem : FoundItm);
			if (FoundItm.Type != EActiveType::EActiveAmmo)
			{
				ActiveItemIndex = NewAddedIndex;
				OnActiveItemChanged.Broadcast(Inventory[NewAddedIndex].ItemID.ToString(), NewAddedIndex, Inventory[NewAddedIndex].DefaultLoadedAmmo);

			}
			RefreshCurrentAmmoForItem();
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Unable to find %s in DataTable."), *ItemID);
	}
}

bool UInventoryComponent::ServerAddItem_Validate(const FString& ItemID, bool Custom, FItem CustomItem)
{
	return true;
}

void UInventoryComponent::ServerDropItem_Implementation(ABaseFirearm* Firearm)
{
	if (Firearm)
	{
		if (Inventory.IsValidIndex(ActiveItemIndex) && Inventory[ActiveItemIndex].ItemID != "Item_Hands" || "Hands")
		{
			FItem ItemToDrop = Inventory[ActiveItemIndex];
			ItemToDrop.UpdateAmmo(Firearm->LoadedAmmo);
			FActorSpawnParameters SpawnParam;
			SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AInventoryBaseItem* Item = GetWorld()->SpawnActor<AInventoryBaseItem>(AInventoryBaseItem::StaticClass(), CalculateDropLocation(), SpawnParam);
			if (Item)
			{
				Inventory.RemoveAt(ActiveItemIndex);
				ActiveItemIndex = 0;
				OnActiveItemChanged.Broadcast("Item_Hands", 0, 0);
			}
		}
	}
}

bool UInventoryComponent::ServerDropItem_Validate(ABaseFirearm* Firearm)
{
	return true;
}

bool UInventoryComponent::InventoryItemExists(FString ItemID, int32& Index, EActiveType &ItemType)
{
	int32 TempIndex = -1;
	EActiveType TempActive = EActiveType::EActiveRifle;
	bool TempSuccess = false;
	for (auto& Item : Inventory)
	{
		TempIndex = TempIndex + 1;
		if (Item.ItemID == FName(*ItemID))
		{
			TempActive = Item.Type;
			TempSuccess = true;
			break;
		}
	}
	Index = TempIndex;
	ItemType = TempActive;
	return TempSuccess;
}

FTransform UInventoryComponent::CalculateDropLocation()
{
	FTransform TempLocation;
	ACharacter* PLY = Cast<ACharacter>(GetOwner());
	if (PLY)
	{
		APlayerController* PC = Cast<APlayerController>(PLY->GetController());
		if (PC)
		{
			FVector EyeLocation; 
			FRotator EyeRotation;
			PC->GetActorEyesViewPoint(EyeLocation, EyeRotation);

			FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("ItemTrace")), true, GetOwner());
			TraceParams.bTraceComplex = true;
			TraceParams.bReturnPhysicalMaterial = false;
			TraceParams.AddIgnoredActor(GetOwner());
			FHitResult HitResult(ForceInit);
			if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, EyeLocation + (EyeRotation.Vector() * 200.f), DROP_TRACE_CHANNEL, TraceParams))
			{
				if (HitResult.bBlockingHit)
				{
					TempLocation.SetLocation(HitResult.TraceEnd);
				}
				else {
					TempLocation.SetLocation(HitResult.ImpactPoint);
				}
				TempLocation.SetRotation(FRotator({ 0.f, FMath::FRandRange(0.f, 360.f), 0.f }).Quaternion());
			}
		}
	}

	return TempLocation;
}

int32 UInventoryComponent::CountAmmo(FName AmmoType, int32& Index)
{
	int32 TempAmount = 0;
	int32 TempIndex = -1;

	for (auto& Item : Inventory)
	{
		TempIndex = TempIndex + 1;
		if (Item.ItemID == AmmoType)
		{
			TempAmount = TempAmount + Item.DefaultLoadedAmmo;
		}
	}

	Index = TempIndex;
	return TempAmount;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

bool UInventoryComponent::GetItemByID(FString ItemID, FItem& Item)
{
	bool TempSuccess = false;
	FItem* ItemFromRow = DataTable->FindRow<FItem>(FName(*ItemID), "Inventory Component - GetItemByID", false);
	if (ItemFromRow)
	{
		if (ItemFromRow->ItemID != "None")
		{
			TempSuccess = true;
			Item = *ItemFromRow;
		}
	}
	return TempSuccess;
}

