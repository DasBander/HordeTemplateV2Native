

#include "CharacterJacob.h"

ACharacterJacob::ACharacterJacob()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Green.M_UE4Man_Green'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
