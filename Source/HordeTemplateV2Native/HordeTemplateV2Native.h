

#pragma once
#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"

/*
	Default Project Definitions
*/

//Inventory
#define INVENTORY_DATATABLE_PATH TEXT("DataTable'/Game/HordeTemplateBP/Data/HordeInventoryItems.HordeInventoryItems'")

//Zombie AI
#define ZED_BEHAVIORTREE_ASSET_PATH TEXT("BehaviorTree'/Game/HordeTemplateBP/Blueprint/Ai/BT/BT_Zed.BT_Zed'")
#define ZED_LOSE_SIGHT_TIME_MIN 6.f
#define ZED_LOSE_SIGHT_TIME_MAX 12.f
#define ZED_LOSE_SIGHT_RADIUS 2500.f
#define ZED_SIGHT_RADIUS 1000.f

//Lobby
#define MAPS_DATATABLE_PATH TEXT("DataTable'/Game/HordeTemplateBP/Data/PlayableLevels.PlayableLevels'")
#define CHARACTER_DATATABLE_PATH TEXT("DataTable'/Game/HordeTemplateBP/Data/PlayableCharacters.PlayableCharacters'")

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