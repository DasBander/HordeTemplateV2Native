

#pragma once
#include "HordeTemplateV2Native.h"
#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "BaseFirearm.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API ABaseFirearm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseFirearm();

	UFUNCTION(BlueprintCallable, Category = "Firearm")
		virtual void FireFirearm();

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Firearm")
		int32 LoadedAmmo;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Firearm")
		FString WeaponID;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Firearm")
		uint8 FireMode;

	
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Firearm")
		bool ProjectileFromMuzzle = false; 

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Firearm")
		void ServerToggleFireMode();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "Firearm")
		void ServerFireFirearm();

protected:

	UFUNCTION(NetMulticast, WithValidation, Unreliable, BlueprintCallable, Category = "Firearm")
		void PlayFirearmFX();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
		USkeletalMeshComponent* Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
		UParticleSystemComponent* MuzzleFlash;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
		UAudioComponent* WeaponSound;

	UFUNCTION(BlueprintPure, Category=" Firearm")
	void GetOwnerEyePoint(bool LocationFromWeapon, FVector& ViewLocation, FRotator& ViewRotation);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



};
