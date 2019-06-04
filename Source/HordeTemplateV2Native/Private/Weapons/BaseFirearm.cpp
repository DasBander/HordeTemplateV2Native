

#include "BaseFirearm.h"
#include "Gameplay/GameplayStructures.h"
#include "Inventory/InventoryHelpers.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeBaseController.h"

ABaseFirearm::ABaseFirearm()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetReplicates(true);

	/*
	Root Component
	*/
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	Weapon->SetupAttachment(RootComponent);

	/*
	Particle Component ( For Muzzle Flash )
	*/
	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Muzzle Flash"));
	MuzzleFlash->SetupAttachment(Weapon);
	MuzzleFlash->SetAutoActivate(false);

	/*
	Audio Component ( For Weapon Sound )
	*/
	WeaponSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Weapon Sound"));
	WeaponSound->SetupAttachment(Weapon);
	WeaponSound->SetAutoActivate(false);



}

void ABaseFirearm::FireFirearm()
{
	if (LoadedAmmo > 0)
	{
		PlayFirearmFX();
		LoadedAmmo = FMath::Clamp<int32>((LoadedAmmo - 1), 0, 9999);

		FItem CurrentWeaponItem = UInventoryHelpers::FindItemByID(FName(*WeaponID));
		FTransform SpawnTransform;
		FVector EyeViewPoint;
		FRotator EyeRotation;
		GetOwnerEyePoint(false, EyeViewPoint, EyeRotation);
		SpawnTransform.SetLocation(EyeViewPoint);
		SpawnTransform.SetRotation(EyeRotation.Quaternion());
		ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(CurrentWeaponItem.ProjectileClass, SpawnTransform, FActorSpawnParameters());
		if (Projectile)
		{
			Projectile->SetOwner(GetOwner());
		}

		AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
		if (PLY)
		{
			AHordeBaseController* PC = Cast<AHordeBaseController>(PLY->GetController());
			if (PC)
			{
				//PC->ClientPlayCameraShake()
			}
			PLY->Inventory->UpdateCurrentItemAmmo(LoadedAmmo);
		}
	}

	else {
		//No Ammo
	}
}



void ABaseFirearm::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseFirearm, LoadedAmmo);
	DOREPLIFETIME(ABaseFirearm, WeaponID);
	DOREPLIFETIME(ABaseFirearm, ProjectileFromMuzzle);
	DOREPLIFETIME(ABaseFirearm, FireMode);
}

void ABaseFirearm::PlayFirearmFX_Implementation()
{
	MuzzleFlash->Activate(true);
	WeaponSound->Play();
}

bool ABaseFirearm::PlayFirearmFX_Validate()
{
	return true;
}

void ABaseFirearm::ServerFireFirearm_Implementation()
{
	FireFirearm();
}

bool ABaseFirearm::ServerFireFirearm_Validate()
{
	return true;
}

void ABaseFirearm::PlayFireModeChange_Implementation()
{

}

bool ABaseFirearm::PlayFireModeChange_Validate()
{
	return true;
}

void ABaseFirearm::ServerToggleFireMode_Implementation()
{
	EFireMode CurrentFireMode = EFireMode(FireMode);
	FItem CurrentWeaponItem = UInventoryHelpers::FindItemByID(FName(*WeaponID));
	int32 CurrentSelectedIndex = CurrentWeaponItem.FireModes.Find(CurrentFireMode);
	if (CurrentSelectedIndex == (CurrentWeaponItem.FireModes.Num() - 1))
	{
		FireMode = (uint8)CurrentWeaponItem.FireModes[0];
	}
	else {
		FireMode = (uint8)CurrentWeaponItem.FireModes[CurrentSelectedIndex + 1];
	}
}

bool ABaseFirearm::ServerToggleFireMode_Validate()
{
	return true;
}

void ABaseFirearm::GetOwnerEyePoint(bool LocationFromWeapon, FVector& ViewLocation, FRotator& ViewRotation)
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwner());
	if (PLY)
	{
		FVector CopyLocationViewPoint;
		FRotator CopyRotationViewPoint;
		PLY->GetActorEyesViewPoint(CopyLocationViewPoint, CopyRotationViewPoint);
		ViewLocation = (LocationFromWeapon) ? Weapon->GetSocketLocation(FName("muzzle")) : PLY->GetCamera()->GetComponentLocation();
		ViewRotation = (LocationFromWeapon) ? Weapon->GetSocketRotation(FName("muzzle")) : CopyRotationViewPoint;
	}
	else {
		ViewLocation = FVector::ZeroVector;
		ViewRotation = FRotator::ZeroRotator;
	}

}

// Called when the game starts or when spawned
void ABaseFirearm::BeginPlay()
{
	Super::BeginPlay();
	
}


