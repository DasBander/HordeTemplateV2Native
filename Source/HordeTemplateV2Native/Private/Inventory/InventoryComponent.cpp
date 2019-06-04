

#include "InventoryComponent.h"
#include "HordeTemplateV2Native.h"
#include "InventoryBaseItem.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeWorldSettings.h"

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
	else {
		UE_LOG(LogTemp, Warning, TEXT("ActiveItemIndex invalid."));
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
		if (Inventory.IsValidIndex(ActiveItemIndex) && (Inventory[ActiveItemIndex].ItemID != "Item_Hands" || Inventory[ActiveItemIndex].ItemID != "Hands"))
		{
			FItem ItemToDrop = Inventory[ActiveItemIndex];
			ItemToDrop.UpdateAmmo(Firearm->LoadedAmmo);
			FTransform TransformToSpawn = CalculateDropLocation();
			AInventoryBaseItem* Item = GetWorld()->SpawnActorDeferred<AInventoryBaseItem>(AInventoryBaseItem::StaticClass(), TransformToSpawn, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (Item)
			{
				Item->ItemInfo = ItemToDrop;
				Item->Spawned = true;
				Item->ItemID = ItemToDrop.ItemID;

				Inventory.RemoveAt(ActiveItemIndex);
				ActiveItemIndex = 0;
				AHordeBaseCharacter* OwningPLY = Cast<AHordeBaseCharacter>(GetOwner());
				if (OwningPLY && !OwningPLY->GetIsDead())
				{
					OnActiveItemChanged.Broadcast("Item_Hands", 0, 0);
				}
				Item->FinishSpawning(TransformToSpawn);

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
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
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
			TraceParams.AddIgnoredActor(PLY->GetCurrentFirearm());
			FHitResult HitResult(ForceInit);
			GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, EyeLocation + (EyeRotation.Vector() * 200.f), DROP_TRACE_CHANNEL, TraceParams);
			
				if (HitResult.bBlockingHit)
				{
					TempLocation.SetLocation(HitResult.ImpactPoint);
				}
				else {
					TempLocation.SetLocation(HitResult.TraceEnd);

				}
				TempLocation.SetRotation(FRotator({ 0.f, FMath::FRandRange(0.f, 360.f), 0.f }).Quaternion());
			
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

bool UInventoryComponent::RemoveAmmoByType(FName AmmoType, int32 AmountToRemove)
{
	int32 AmmoIndex;
	int32 TempAmount = CountAmmo(AmmoType, AmmoIndex);
	if (TempAmount >= AmountToRemove)
	{
		if ((Inventory[AmmoIndex].DefaultLoadedAmmo - AmountToRemove) <= 0)
		{
			Inventory.RemoveAt(AmmoIndex);
		}
		else
		{
			Inventory[AmmoIndex].UpdateAmmo((Inventory[AmmoIndex].DefaultLoadedAmmo - AmountToRemove));
		}
		RefreshCurrentAmmoForItem();
		return true;
	
	}
	else {
		return false;
	}
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		AHordeWorldSettings* WorldSettings = Cast<AHordeWorldSettings>(GetWorld()->GetWorldSettings(true));
		if (WorldSettings)
		{
			for (auto& StartingItem : WorldSettings->StartingItems)
			{
				ServerAddItem(StartingItem.ToString(), false, FItem());
			}
		}

	}
	
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

