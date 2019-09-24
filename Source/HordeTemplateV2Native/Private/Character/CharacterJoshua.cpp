

#include "CharacterJoshua.h"

/**
 *	Constructor for Character Joshua
 *
 * @param
 * @return
 */
ACharacterJoshua::ACharacterJoshua()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Cyan.M_UE4Man_Cyan'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
