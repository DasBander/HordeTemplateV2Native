

#include "CharacterMatt.h"


/**
 *	Constructor for Character Matt
 *
 * @param
 * @return
 */
ACharacterMatt::ACharacterMatt()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Blue.M_UE4Man_Blue'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
