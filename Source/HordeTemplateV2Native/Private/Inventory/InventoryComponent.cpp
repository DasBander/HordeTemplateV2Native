

#include "InventoryComponent.h"
#include "HordeTemplateV2Native.h"
#include "InventoryBaseItem.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeWorldSettings.h"

/**
 * Constructor for UInventoryComponent
 *
 * @param
 * @return
 */
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

/** ( Overridden )
 * Define Replicated Props
 *
 * @param
 * @output Lifetime Props
 * @return void
 */
void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, ActiveItemIndex);
	DOREPLIFETIME(UInventoryComponent, AvailableAmmoForCurrentWeapon);
}

/**
 * Returns current Inventory Array
 *
 * @param
 * @return TArray of FItem ( Inventory )
 */
const TArray<FItem>& UInventoryComponent::GetInventory()
{
	return Inventory;
}

/**
 * Updates the Ammo of current active item in Inventory.
 *
 * @param Ammo to update.
 * @return void
 */
void UInventoryComponent::UpdateCurrentItemAmmo(int32 Ammo)
{
	if (Inventory.IsValidIndex(ActiveItemIndex))
	{
		Inventory[ActiveItemIndex].UpdateAmmo(Ammo);
	}
}

/**
 * Refreshes the Available Ammo Value for the current item so we don't have to replicate the whole item.
 *
 * @param
 * @return
 */
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

/** ( Server )
 * Adds Item with given Item ID.
 *
 * @param The Item ID, bool if modified Item and the modified Item. Only used if Custom is set to true.
 * @return void
 */
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
					if (ItemID == PLY->GetCurrentFirearm()->WeaponID)
					{
						ServerDropItem(PLY->GetCurrentFirearm());
					}
					else {
						ServerDropItemAtIndex(ExistingItemIndex);
					}
					
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

/** ( Server )
 * Drops current item and removes it from the Inventory.
 *
 * @param
 * @return
 */
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

/** ( Server )
 * Drops item by given inventory index.
 *
 * @param Inventory Index to Drop
 * @return void
 */
void UInventoryComponent::ServerDropItemAtIndex_Implementation(int32 IndexToDrop)
{
		if (Inventory.IsValidIndex(IndexToDrop) && (Inventory[IndexToDrop].ItemID != "Item_Hands" || Inventory[IndexToDrop].ItemID != "Hands"))
		{
			FItem ItemToDrop = Inventory[IndexToDrop];
			FTransform TransformToSpawn = CalculateDropLocation();
			AInventoryBaseItem* Item = GetWorld()->SpawnActorDeferred<AInventoryBaseItem>(AInventoryBaseItem::StaticClass(), TransformToSpawn, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (Item)
			{
				Item->ItemInfo = ItemToDrop;
				Item->Spawned = true;
				Item->ItemID = ItemToDrop.ItemID;
				Inventory.RemoveAt(IndexToDrop);
				Item->FinishSpawning(TransformToSpawn);

			}
		}
}

bool UInventoryComponent::ServerDropItemAtIndex_Validate(int32 IndexToDrop)
{
	return true;
}

bool UInventoryComponent::ServerDropItem_Validate(ABaseFirearm* Firearm)
{
	return true;
}

/**
 * Returns if the item exists or not. Also returns the Item Index inside the Inventory and the Item Type.
 *
 * @param The Item ID
 * @output Item Index in Inventory and the Item Type
 * @return if item exists or not.
 */
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

/**
 * Calculates item drop location with a line trace by channel.
 *
 * @param
 * @return Item Drop Location
 */
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

/**
 * Counts Ammo by Ammo Type, returns the amount and index of it.
 *
 * @param Ammo Type
 * @output Index in Inventory
 * @return Amount of Ammo
 */
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

/**
 * Removes Ammo by given Type and Amount.
 *
 * @param
 * @return bool true if has enough to remove or false if not.
 */
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

/**
 * Returns next active weapon type by current selected item.
 *
 * @param
 * @return EActiveType ( Primary, Secondary, Med, Misc )
 */
EActiveType UInventoryComponent::FindNextWeaponType()
{
	EActiveType RetType;
	TArray<EActiveType> AvailableCategories = GetAvailableCategories();
	int32 CurrentIndex = AvailableCategories.Find(Inventory[ActiveItemIndex].Type);
	if (CurrentIndex == AvailableCategories.Num() - 1)
	{
		RetType = AvailableCategories[0];
	}
	else
	{
		RetType = AvailableCategories[CurrentIndex + 1];
	}
	return RetType;
}

/**
 * Returns last active weapon type by current selected item.
 *
 * @param
 * @return EActiveType ( Primary, Secondary, Med, Misc )
 */
EActiveType UInventoryComponent::FindLastWeaponType()
{
	EActiveType RetType = EActiveType::EActiveRifle;
	TArray<EActiveType> AvailableCategories = GetAvailableCategories();
	int32 CurrentIndex = AvailableCategories.Find(Inventory[ActiveItemIndex].Type);
	if (CurrentIndex == 0 && AvailableCategories.Num() > 0)
	{
		RetType = AvailableCategories[AvailableCategories.Num() - 1];
	}
	else 
	{
		if (AvailableCategories.IsValidIndex(CurrentIndex - 1))
		{
			RetType = AvailableCategories[CurrentIndex - 1];
		}

	}
	return RetType;
}

/**
 * Returns available EActiveType category from Inventory.
 *
 * @param
 * @return TArray<EActiveType> Weapon Category in Inventory.
 */
TArray<EActiveType> UInventoryComponent::GetAvailableCategories()
{
	TArray<EActiveType> TempActiveTypes;
	for (auto Itm : Inventory)
	{
		if (Itm.Type != EActiveType::EActiveAmmo)
		{
			TempActiveTypes.AddUnique(Itm.Type);
		}
	}
	return TempActiveTypes;
}

/** ( Server )
 * Switch to active weapon by Item Type.
 *
 * @param Item Type to be switched to.
 * @return void
 */
void UInventoryComponent::SwitchWeapon_Implementation(EActiveType ItemType)
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
	if (PLY && !PLY->Reloading && !PLY->GetIsDead())
	{
		FItem NewItem;
		int32 NewIndex;
		FindItemByCategory(ItemType, NewItem, NewIndex);
		if (NewIndex > -1 && NewIndex != ActiveItemIndex)
		{
			ActiveItemIndex = NewIndex;
			OnActiveItemChanged.Broadcast(Inventory[NewIndex].ItemID.ToString(), NewIndex, Inventory[NewIndex].DefaultLoadedAmmo);
			RefreshCurrentAmmoForItem();
		}
	}
}

bool UInventoryComponent::SwitchWeapon_Validate(EActiveType ItemType)
{
	return true;
}

/**
 * Returns Item and Index by Category.
 *
 * @param Item Type
 * @output The Item and Index in the Inventory.
 * @return void
 */
void UInventoryComponent::FindItemByCategory(EActiveType IType, FItem& OutItem, int32& OutIndex)
{
	int32 LIndex = -1;
	FItem LItem;
	for (auto Itm : Inventory)
	{
		LIndex++;
		if (Itm.Type == IType && LIndex != ActiveItemIndex && Itm.ItemID != "Item_Hands")
		{
			LItem = Itm;
			break;
		}
	}
	OutItem = LItem;
	OutIndex = LIndex;
}

/** ( Server )
 * Scroll trough Inventory mostly by MouseWheel Up and Down.
 *
 * @param bool if Scrolling up or down.
 * @return void
 */
void UInventoryComponent::ScrollItems_Implementation(bool ScrollUp)
{
	if (ScrollUp)
	{
		SwitchWeapon(FindNextWeaponType());
	}
	else
	{
		SwitchWeapon(FindLastWeaponType());
	}
}

bool UInventoryComponent::ScrollItems_Validate(bool ScrollUp)
{
	return true;
}

/** ( Virtual; Overridden )
 * Adds starting items to inventory.
 *
 * @param
 * @return
 */
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

/**
 * Returns Item by Item ID.
 *
 * @param Item ID
 * @output The Item.
 * @return bool if found in Datatable.
 */
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

