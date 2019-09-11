

#include "CharacterMatt.h"


/*
	FUNCTION: Constructor for ACharacterMatt
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for ACharacterMatt. Sets the Default Material to Blue Material.

*/
ACharacterMatt::ACharacterMatt()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Blue.M_UE4Man_Blue'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
