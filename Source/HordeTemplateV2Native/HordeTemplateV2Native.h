

#pragma once
#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"

/*
	Default Project Definitions
*/
#define INVENTORY_DATATABLE_PATH TEXT("DataTable'/Game/HordeTemplateBP/CppTesting/HordeInventoryItems.HordeInventoryItems'")
#define DROP_TRACE_CHANNEL ECC_GameTraceChannel1
#define PLAYER_HEAD_DISPLAY_CHANNEL ECC_GameTraceChannel2
#define INTERACTION_TRACE_CHANNEL ECC_GameTraceChannel3

/*
	Loads object dynamically from a given path.
*/
template <typename ObjClass>
static FORCEINLINE ObjClass* ObjectFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;
	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}