

#include "CharacterLouis.h"

/**
 *	Constructor for Character Louis
 *
 * @param
 * @return
 */
ACharacterLouis::ACharacterLouis()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Yellow.M_UE4Man_Yellow'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
