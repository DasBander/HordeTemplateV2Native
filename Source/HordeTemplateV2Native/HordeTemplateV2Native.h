

#pragma once
#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"

/*
	Default Project Definitions
*/

//Inventory
#define INVENTORY_DATATABLE_PATH TEXT("DataTable'/Game/HordeTemplateBP/CppTesting/HordeInventoryItems.HordeInventoryItems'")

//Lobby
#define MAPS_DATATABLE_PATH TEXT("DataTable'/Game/HordeTemplateBP/CppTesting/PlayableLevels.PlayableLevels'")

//Tracing
#define DROP_TRACE_CHANNEL ECC_GameTraceChannel1
#define PLAYER_HEAD_DISPLAY_CHANNEL ECC_GameTraceChannel2
#define INTERACTION_TRACE_CHANNEL ECC_GameTraceChannel3

//Default Map Settings
#define DEFAULT_STARTING_ITEMS {"Item_Hands"}
#define DEFAULT_AVAILABLE_PLAYERCHARACTERS { "Char_Matt", "Char_Louis", "Char_Jacob", "Char_Joshua" }

/*
	Loads object dynamically from a given path.
*/
template <typename ObjClass>
static FORCEINLINE ObjClass* ObjectFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;
	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}