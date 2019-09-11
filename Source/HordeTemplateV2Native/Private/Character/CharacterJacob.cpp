

#include "CharacterJacob.h"


/*
	FUNCTION: Constructor for ACharacterJacob
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for ACharacterJacob. Sets the Default Material to Green Material.

*/
ACharacterJacob::ACharacterJacob()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Green.M_UE4Man_Green'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
