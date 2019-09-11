

#include "CharacterJoshua.h"

/*
	FUNCTION: Constructor for ACharacterJoshua
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for ACharacterJoshua. Sets the Default Material to Cyan Material.

*/
ACharacterJoshua::ACharacterJoshua()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Cyan.M_UE4Man_Cyan'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
