

#include "BaseFirearm.h"

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

}

bool ABaseFirearm::ServerToggleFireMode_Validate()
{
	return true;
}

void ABaseFirearm::GetOwnerEyePoint(bool LocationFromWeapon, FVector& ViewLocation, FRotator& ViewRotation)
{
	ViewLocation = FVector::ZeroVector;
	ViewRotation = FRotator::ZeroRotator;
}

// Called when the game starts or when spawned
void ABaseFirearm::BeginPlay()
{
	Super::BeginPlay();
	
}


