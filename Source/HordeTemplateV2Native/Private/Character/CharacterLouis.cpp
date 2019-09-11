

#include "CharacterLouis.h"

/*
	FUNCTION: Constructor for ACharacterLouis
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for ACharacterLouis. Sets the Default Material to Yellow Material.

*/
ACharacterLouis::ACharacterLouis()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Yellow.M_UE4Man_Yellow'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
