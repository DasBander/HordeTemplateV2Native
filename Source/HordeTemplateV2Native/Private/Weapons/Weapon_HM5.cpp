

#include "Weapon_HM5.h"
#include "ConstructorHelpers.h"

AWeapon_HM5::AWeapon_HM5()
{
	const ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshAsset(TEXT("SkeletalMesh'/Game/HordeTemplateBP/Assets/Meshes/Weapons/HTM4/SK_HTM4.SK_HTM4'"));
	if (WeaponMeshAsset.Succeeded())
	{
		Weapon->SetSkeletalMesh(WeaponMeshAsset.Object);
		Weapon->SetRelativeRotation(FRotator({ 0.f, -180.f, 0.f }).Quaternion());
		Weapon->SetRelativeLocation(FVector(-1.999965f, 5.000014f, -3.0f));
	}
}
