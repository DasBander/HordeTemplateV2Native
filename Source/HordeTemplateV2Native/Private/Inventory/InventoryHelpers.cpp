

#include "InventoryHelpers.h"
#include "HordeTemplateV2Native.h"

/**
 * Constructor for UInventoryHelpers
 *
 * @param
 * @return
 */
UInventoryHelpers::UInventoryHelpers()
{

}

/**
 * Finds Item by Item ID.
 *
 * @param Item ID
 * @return Item
 */
FItem UInventoryHelpers::FindItemByID(FName ItemID)
{
	UDataTable* InventoryData = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, INVENTORY_DATATABLE_PATH));
	FItem TempItem;

	if (InventoryData) {
		FItem* ItemFromRow = InventoryData->FindRow<FItem>(ItemID, "Loot Helper - Find Item By ID", false);
		if (ItemFromRow)
		{
			TempItem = *ItemFromRow;
		}
	}
	else {
		GLog->Log("Inventory Data not valid.");
	}

	return TempItem;
}