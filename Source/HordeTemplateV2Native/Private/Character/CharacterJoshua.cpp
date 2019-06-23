

#include "CharacterJoshua.h"

ACharacterJoshua::ACharacterJoshua()
{
	const ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> CharacterMaterialAsset(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Mannequin/Character/Materials/M_UE4Man_Cyan.M_UE4Man_Cyan'"));
	if (CharacterMaterialAsset.Succeeded())
	{
		GetMesh()->SetMaterial(0, CharacterMaterialAsset.Object);
	}
}
