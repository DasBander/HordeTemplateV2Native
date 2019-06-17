

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapons/BaseFirearm.h"
#include "Components/WidgetComponent.h"
#include "Gameplay/GameplayStructures.h"
#include "Inventory/InventoryComponent.h"
#include "HUD/HordeBaseHud.h"
#include "AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "HordeBaseCharacter.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHordeBaseCharacter();

	

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetRemotePitch();

protected:
	virtual void BeginPlay() override;

	/*
	Character Basic Stuff
	*/
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Character|Stamina")
		float Health = 100.f;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Character|Firearm")
		ABaseFirearm* CurrentSelectedFirearm;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|Inventory")
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|Inventory")
		UCameraComponent* FollowCamera;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|Inventory")
		UWidgetComponent* PlayerNameWidget;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character")
		UAIPerceptionStimuliSourceComponent* StimuliSource;

	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable, Category = "Interaction")
		void ServerInteract(AActor* ActorToInteractWith);
	
	UFUNCTION()
		virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
		bool IsDead = false;


	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation, Category = "Character")
		void PlaySoundOnAllClients(USoundCue* Sound, FVector Location);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation, Category = "Character")
		void PlayAnimationAllClients(class UAnimMontage* Montage);

	UFUNCTION(BlueprintCallable, Category = "Character")
		bool RemoveHealth(float HealthToRemove);

	UFUNCTION(BlueprintCallable, Category = "Character")
		void CharacterDie();
	/*
	Interaction
	*/
	UPROPERTY()
		FTimerHandle InteractionDetectionTimer;

	UPROPERTY()
		FTimerHandle InteractionTimer;

	UFUNCTION()
		void StartInteraction();

	UFUNCTION()
		void StopInteraction();

	UFUNCTION()
		void ProcessInteraction();

	UFUNCTION()
		void HeadDisplayTrace();

	UPROPERTY()
		AActor* LastInteractionActor;

	UFUNCTION()
		void InteractionDetection();

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		bool IsInteracting = false;



	
	/*
	Sprinting
	*/

	UFUNCTION(NetMulticast, WithValidation, Reliable)
		void UpdatePlayerMovementSpeed(float NewMovementSpeed);

	/*
	Head Display ( 3D Player Name/Health Widget )
	*/
	UPROPERTY()
		FTimerHandle HeadDisplayTraceTimer;

	UPROPERTY(BlueprintReadOnly, Category = "Character|Head Display")
		AHordeBaseCharacter* LastFacingCharacter;

	
	UFUNCTION(NetMulticast, Unreliable, WithValidation, BlueprintCallable, Category = "Player|Head Display")
		void UpdateHeadDisplayWidget(const FString& PlayerName, float PlayerHealth);

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "Player|Head Display")
		void RagdollPlayer();

	UPROPERTY(BlueprintReadWrite, Category = "Character|Stamina")
		float StaminaRefreshRate = 0.8f;

	UPROPERTY(BlueprintReadWrite, Category = "Character|Stamina")
		float StaminaDecreaseRate = 0.8f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Character|Stamina")
		float Stamina = 100.f;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Character|Stamina")
		bool IsSprinting = false;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStartSprinting();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopSprinting();

	UFUNCTION()
		void DecreaseStamina();

	UFUNCTION()
		void IncreaseStamina();

	UPROPERTY()
		FTimerHandle TimerIncreaseStamina;

	UPROPERTY()
		FTimerHandle TimerDecreaseStamina;

	/*
	Inventory
	*/
	UFUNCTION()
		void ActiveItemChanged(FString ItemID, int32 ItemIndex, int32 LoadedAmmo);

	/* 
	HUD
	*/
	UFUNCTION()
		AHordeBaseHUD* GetHUD();


	/*
	Weapon Logic
	*/

	UFUNCTION()
		void StopWeaponFire();

	UFUNCTION()
		void TriggerWeaponFire();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Firearm")
		bool Reloading = false;

	UPROPERTY(BlueprintReadOnly, Category = "Firearm")
		bool IsBursting = false;

	UPROPERTY()
		FTimerHandle WeaponFireTimer;

	UPROPERTY()
		FItem CurrentWeaponInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Firearm")
		FTimerHandle BurstTimer;

	UFUNCTION()
		void BurstWeapon();

	UFUNCTION()
		void AutoFireWeapon();

	UPROPERTY()
		float NumberOfBursts = 0.f;


	UFUNCTION()
		void ToggleFiremode();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Firearm")
		void ServerReload();

	UPROPERTY()
		FTimerHandle ReloadTimerHandle;

	UFUNCTION()
		void FinishReload();

	UFUNCTION()
		void DropCurrentItem();

public:	

	FORCEINLINE float GetHealth()
	{
		return Health;
	}

	FORCEINLINE bool GetIsDead()
	{
		return IsDead;
	}

	FORCEINLINE UCameraComponent* GetCamera()
	{
		return FollowCamera;
	}

	UPROPERTY(BlueprintReadOnly, Category="Interaction")
		float InteractionProgress = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		float InteractionTime = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
		float TargetInteractionTime = 0.f;


	UUserWidget* GetHeadDisplayWidget();

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Player|Animation")
		int32 AnimMode = 0;

	ABaseFirearm* GetCurrentFirearm();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Character|Inventory")
		UInventoryComponent* Inventory;


	
};
