/*
@Todo
Weapon Switching
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/BaseFirearm.h"
#include "Gameplay/GameplayStructures.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HORDETEMPLATEV2NATIVE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActiveItemChanged, FString, ItemID, int32, ItemIndex, int32, LoadedRounds);

public:	
	UInventoryComponent();

	/*
	Gets the Protected Inventory.
	*/
	UFUNCTION(BlueprintPure, Category="Inventory")
	const TArray<FItem>& GetInventory();

	/*
	Gets called if the current item has changed.
	*/
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnActiveItemChanged OnActiveItemChanged;

	/*
	Updates the Amount of Ammo in Current Selected item.
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateCurrentItemAmmo(int32 Ammo);

	/*
	Refreshes the Value of AvailableAmmoForCurrentWeapon so we can use it in our HUD.
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshCurrentAmmoForItem();

	/*
	Adds Item by ID or by Custom Structure to Inventory on the Server.
	*/
	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = "Inventory")
		void ServerAddItem(const FString &ItemID, bool Custom, FItem CustomItem);


	/*
	Drops current selected Weapon on ground.
	*/
	UFUNCTION(Server, WithValidation, Reliable, Category = "Inventory")
		void ServerDropItem(ABaseFirearm* Firearm);

	/*
	Drops Weapon on given index to the ground.
	*/
	UFUNCTION(Server, WithValidation, Reliable, Category = "Inventory")
		void ServerDropItemAtIndex(int32 IndexToDrop);

	/*
	Checks if Item exists in Inventory and returns Index as well as the Item Type
	*/
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool InventoryItemExists(FString ItemID, int32& Index, EActiveType& ItemType);

	/*
	Returns Transform in front of the current owning actor. Used to Drop items on the Ground.
	*/
	UFUNCTION(BlueprintPure, Category="Inventory")
	FTransform CalculateDropLocation();

	/*
	Gets amount of Ammo by Type
	*/
	UFUNCTION(BlueprintCallable, Category="Inventory")
	int32 CountAmmo(FName AmmoType, int32& Index);

	/*
	Removes Specific Amount of Ammo by Type
	*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool RemoveAmmoByType(FName AmmoType, int32 AmountToRemove);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Inventory")
		void ScrollItems(bool ScrollUp);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Inventory")
		void SwitchWeapon(EActiveType ItemType);

	UFUNCTION()
		EActiveType FindNextWeaponType();

	UFUNCTION()
		EActiveType FindLastWeaponType();

	UFUNCTION()
		TArray<EActiveType> GetAvailableCategories();
	/*
	Data Table where we have all Item Base Values stored.
	*/
	UPROPERTY(EditAnywhere, Category="Inventory")
		UDataTable* DataTable;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void FindItemByCategory(EActiveType IType, FItem& OutItem, int32& OutIndex);

protected:
	virtual void BeginPlay() override;

	/*
	The Inventory. Do not touch or make it public!
	*/
	UPROPERTY()
		TArray<FItem> Inventory;

	/*
	Gets a fresh Item of our Data Table based on the ItemID
	*/
	bool GetItemByID(FString ItemID, FItem &Item);

	/*
	Current Selected Active Index in our Inventory
	*/
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Inventory")
		int32 ActiveItemIndex;

	/*
	Available Ammo for Current Selected Weapon ( Used for HUD )
	*/
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Inventory")
		int32 AvailableAmmoForCurrentWeapon;

		
};
