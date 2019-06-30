

#include "HordeBaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Inventory/InventoryHelpers.h"
#include "Gameplay/HordeGameMode.h"
#include "Weapons/BaseFirearm.h"
#include "HUD/Widgets/PlayerHeadDisplay.h"
#include "AIModule/Classes/Perception/AISense_Sight.h"
#include "FX/Camera/CameraShake_Damage.h"


void AHordeBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHordeBaseCharacter, Stamina);
	DOREPLIFETIME(AHordeBaseCharacter, Health);
	DOREPLIFETIME(AHordeBaseCharacter, AnimMode);
	DOREPLIFETIME(AHordeBaseCharacter, IsSprinting);
	DOREPLIFETIME(AHordeBaseCharacter, IsDead);
	DOREPLIFETIME(AHordeBaseCharacter, Reloading);
	DOREPLIFETIME(AHordeBaseCharacter, CurrentSelectedFirearm);
}

// Sets default values
AHordeBaseCharacter::AHordeBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


	const ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerMeshAsset(TEXT("SkeletalMesh'/Game/HordeTemplateBP/Assets/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (PlayerMeshAsset.Succeeded()) {
		GetMesh()->SetSkeletalMesh(PlayerMeshAsset.Object);
		GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
		GetMesh()->SetCollisionProfileName(FName(TEXT("Ragdoll")));
	}
	static ConstructorHelpers::FObjectFinder<UAnimBlueprintGeneratedClass> AnimBlueprint(TEXT("AnimBlueprint'/Game/HordeTemplateBP/Assets/Mannequin/Animations/ABP_ThirdPerson.ABP_ThirdPerson_C'"));
	if (AnimBlueprint.Succeeded())
	{
		GetMesh()->AnimClass = AnimBlueprint.Object;
	}

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 90.f;
	CameraBoom->SetRelativeLocation(FVector(0.f, 49.f, 70.f));
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritRoll = true;
	CameraBoom->bInheritYaw = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;


	PlayerNameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Player 3D Name Widget"));
	PlayerNameWidget->SetupAttachment(RootComponent);
	PlayerNameWidget->SetDrawAtDesiredSize(true);
	PlayerNameWidget->SetDrawSize(FVector2D(400.f, 50.f));
	PlayerNameWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PlayerNameWidget->SetPivot(FVector2D(0.f, 0.f));
	PlayerNameWidget->SetRelativeLocation(FVector(0.f, 6.f, 81.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> NameWidgetAsset(TEXT("WidgetBlueprint'/Game/HordeTemplateBP/Blueprint/Widgets/Misc/WBP_3d_PlayerView.WBP_3d_PlayerView_C'"));
	if (NameWidgetAsset.Succeeded())
	{
		PlayerNameWidget->SetWidgetClass(NameWidgetAsset.Class);
	}

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AI Stimuli Source"));
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());


	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Player Inventory"));
	if (GetWorld())
	{
		Inventory->RegisterComponent();
		Inventory->SetIsReplicated(true);
		Inventory->OnActiveItemChanged.AddDynamic(this, &AHordeBaseCharacter::ActiveItemChanged);
		const ConstructorHelpers::FObjectFinder<UDataTable> InventoryDataTableAsset(INVENTORY_DATATABLE_PATH);
		if (InventoryDataTableAsset.Succeeded())
		{
			Inventory->DataTable = InventoryDataTableAsset.Object;
		}
	}


}

/*
Hacky methode for GetBaseAimRotation() (Pitch).

Pitch is somehow 2 instead of 0 by default which means it doesn't use the replicated value for it even if it should at that point inside the anim bp.
That's why we get it here and transform it into the value we need. We have no idea why the pitch is by default 2. If we find a fix for it we can remove that.
*/
float AHordeBaseCharacter::GetRemotePitch()
{
	return RemoteViewPitch * 360.0f / 255.0f;
}

void AHordeBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle DelayedBeginPlay;
	FTimerDelegate DelayedBeginPlayDelegate;

	DelayedBeginPlayDelegate.BindLambda([=] {
		if (IsLocallyControlled())
		{
			GetWorld()->GetTimerManager().SetTimer(InteractionDetectionTimer, this, &AHordeBaseCharacter::InteractionDetection, 0.05f, true);
			PlayerNameWidget->SetWidget(nullptr);
			GetWorld()->GetTimerManager().SetTimer(HeadDisplayTraceTimer, this, &AHordeBaseCharacter::HeadDisplayTrace, 0.05f, true);
		}
		if (HasAuthority())
		{

			UpdateHeadDisplayWidget(GetPlayerState()->GetPlayerName(), Health);
		}
	});
	GetWorld()->GetTimerManager().SetTimer(DelayedBeginPlay, DelayedBeginPlayDelegate, 1.f, false);
	
}


void AHordeBaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void AHordeBaseCharacter::ServerInteract_Implementation(AActor* ActorToInteractWith)
{
	if (ActorToInteractWith)
	{
		IInteractionInterface::Execute_Interact(ActorToInteractWith, this);
		FInteractionInfo InterInf = IInteractionInterface::Execute_GetInteractionInfo(ActorToInteractWith);
		if (InterInf.InteractionSound)
		{
			PlaySoundOnAllClients(InterInf.InteractionSound, GetMesh()->GetComponentLocation());
		}
	}
}

bool AHordeBaseCharacter::ServerInteract_Validate(AActor* ActorToInteractWith)
{
	return true;
}

void AHordeBaseCharacter::PlaySoundOnAllClients_Implementation(USoundCue* Sound, FVector Location)
{
	if (Sound)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, Location, FRotator::ZeroRotator, 1.f, 1.f, 0.f, nullptr, nullptr, true);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Could not PlaySoundOnAllClients! SoundCue not valid."));
	}
	
}

bool AHordeBaseCharacter::PlaySoundOnAllClients_Validate(USoundCue* Sound, FVector Location)
{
	return true;
}

float AHordeBaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	if (HasAuthority())
	{
		if (!IsDead)
		{
			if (RemoveHealth(Damage))
			{
				CharacterDie();
			}
			else {
				APlayerController* PC = Cast<APlayerController>(GetController());
				if (PC)
				{
					PC->ClientPlayCameraShake(UCameraShake_Damage::StaticClass(), 2.f);
				}
				USoundCue* DamageSound = ObjectFromPath<USoundCue>(FName(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_PlayerDamage.A_PlayerDamage'")));
				if (DamageSound)
				{
					PlaySoundOnAllClients(DamageSound, GetMesh()->GetComponentLocation());
				}
			}
		}
	}
	
	return Health;
}

void AHordeBaseCharacter::PlayAnimationAllClients_Implementation(class UAnimMontage* Montage)
{
	UAnimMontage* Mont = ObjectFromPath<UAnimMontage>(TEXT("AnimMontage'/Game/HordeTemplateBP/Assets/Animations/AM_Reload_Rifle.AM_Reload_Rifle'"));
	GetMesh()->GetAnimInstance()->Montage_Play(Mont, 1.f, EMontagePlayReturnType::Duration, 0.f, false);

}

bool AHordeBaseCharacter::PlayAnimationAllClients_Validate(class UAnimMontage* Montage)
{
	return true;
}

bool AHordeBaseCharacter::RemoveHealth(float HealthToRemove)
{
	Health = FMath::Clamp<float>((Health - HealthToRemove), 0.f, 100.f);
	UpdateHeadDisplayWidget(GetPlayerState()->GetPlayerName(), Health);
	return (Health <= 0.f);
}

void AHordeBaseCharacter::CharacterDie()
{
	IsDead = true;
	if (CurrentSelectedFirearm)
	{
		Inventory->ServerDropItem(CurrentSelectedFirearm);
	}

	AHordeGameMode* GM = Cast<AHordeGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			GM->SpawnSpectator(PC);
		}
	}
	/*
	Set Dead in PlayerState
	*/
	RagdollPlayer();
	SetLifeSpan(20.f);
}

void AHordeBaseCharacter::StartInteraction()
{
	if (!IsInteracting && LastInteractionActor)
	{
		InteractionProgress = 0.f;
		FInteractionInfo InteractionInfo = IInteractionInterface::Execute_GetInteractionInfo(LastInteractionActor);
		if (InteractionInfo.AllowedToInteract)
		{
			GetHUD()->GetHUDWidget()->IsInteracting = true;
			IsInteracting = true;
			TargetInteractionTime = InteractionInfo.InteractionTime;
			GetWorld()->GetTimerManager().SetTimer(InteractionTimer, this, &AHordeBaseCharacter::ProcessInteraction, .01f, true);
		}
	}
}

void AHordeBaseCharacter::StopInteraction()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(InteractionTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(InteractionTimer);
	}
	IsInteracting = false;
	GetHUD()->GetHUDWidget()->IsInteracting = false;
	InteractionTime = 0.f;
	TargetInteractionTime = 0.f;
	InteractionProgress = 0.f;
}

void AHordeBaseCharacter::ProcessInteraction()
{
	if (LastInteractionActor)
	{
		if (InteractionTime >= TargetInteractionTime)
		{
			StopInteraction();
			if (LastInteractionActor)
			{
				ServerInteract(LastInteractionActor);
			}
		}
		else {
			InteractionTime = InteractionTime + 0.01f;
			InteractionProgress = FMath::GetMappedRangeValueClamped(FVector2D(0.f, TargetInteractionTime), FVector2D(0.f, 100.f), InteractionTime);
		}
		FInteractionInfo InteractionInfo = IInteractionInterface::Execute_GetInteractionInfo(LastInteractionActor);
		if (!InteractionInfo.AllowedToInteract || IsDead)
		{
			StopInteraction();
		}
	}
	else {
		StopInteraction();
	}
	
}

void AHordeBaseCharacter::HeadDisplayTrace()
{
	FHitResult HitResult(ForceInit);

	const TArray<AActor*> IgnoringActors = {this};


	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), FollowCamera->GetComponentLocation(), FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * 500.f), 64.f, ETraceTypeQuery::TraceTypeQuery3, false, IgnoringActors, EDrawDebugTrace::None, HitResult, true))
	{
		AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(HitResult.GetActor());
		if (PLY)
		{
			LastFacingCharacter = PLY;
			UPlayerHeadDisplay* PLYWidget = Cast<UPlayerHeadDisplay>(LastFacingCharacter->GetHeadDisplayWidget());
			if (PLYWidget)
			{
				PLYWidget->OnShowWidgetDelegate.Broadcast();
			}
		}
		else {
			if (LastFacingCharacter)
			{
				UPlayerHeadDisplay* PLYWidget = Cast<UPlayerHeadDisplay>(LastFacingCharacter->GetHeadDisplayWidget());
				if (PLYWidget)
				{
					PLYWidget->OnHideWidgetDelegate.Broadcast();
					LastFacingCharacter = nullptr;
				}
			}
		}
	}
	else {
		if (LastFacingCharacter)
		{
			UPlayerHeadDisplay* PLYWidget = Cast<UPlayerHeadDisplay>(LastFacingCharacter->GetHeadDisplayWidget());
			if (PLYWidget)
			{
				PLYWidget->OnHideWidgetDelegate.Broadcast();
				LastFacingCharacter = nullptr;
			}
		}
	}
}

void AHordeBaseCharacter::InteractionDetection()
{
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("ItemTrace")), true, GetOwner());
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.AddIgnoredActor(GetOwner());
	FHitResult HitResult(ForceInit);
	

	const TArray<AActor*> IgnoringActors;

	if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), FollowCamera->GetComponentLocation(), FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * 300.f), 5.f, ETraceTypeQuery::TraceTypeQuery1, false, IgnoringActors, EDrawDebugTrace::None, HitResult, true))
	{
		if (HitResult.GetActor())
		{
			if (HitResult.GetActor()->Implements<UInteractionInterface>())
			{
				LastInteractionActor = HitResult.GetActor();
				FInteractionInfo NewInfo = IInteractionInterface::Execute_GetInteractionInfo(LastInteractionActor);
				AHordeBaseHUD* HUD = GetHUD();
				UInputSettings* Settings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());

				if (HUD && HUD->GetHUDWidget() && Settings)
				{
					TArray<FInputActionKeyMapping> UseKeys;
					Settings->GetActionMappingByName("Use", UseKeys);
					FString InteractionText;
					if (UseKeys.IsValidIndex(0) && !NewInfo.HideKeyInText)
					{
						InteractionText = "[" + UseKeys[0].Key.GetDisplayName().ToString() + "] " + NewInfo.InteractionText.ToString();
					}
					else
					{
						InteractionText = NewInfo.InteractionText.ToString();
					}
					HUD->GetHUDWidget()->OnSetInteractionText.Broadcast(FText::FromString(*InteractionText));
				}

			}
			else {
				LastInteractionActor = nullptr;
				AHordeBaseHUD* HUD = GetHUD();
				if (HUD && HUD->GetHUDWidget())
				{
					HUD->GetHUDWidget()->OnHideInteractionText.Broadcast();
				}
			}
		}
		
	}
	else {
		LastInteractionActor = nullptr;
		AHordeBaseHUD* HUD = GetHUD();
		if (HUD && HUD->GetHUDWidget())
		{
			HUD->GetHUDWidget()->OnHideInteractionText.Broadcast();
		}
	}
}


void AHordeBaseCharacter::RagdollPlayer_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("DeadAI")));
}

bool AHordeBaseCharacter::RagdollPlayer_Validate()
{
	return true;
}

void AHordeBaseCharacter::UpdatePlayerMovementSpeed_Implementation(float NewMovementSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
}

bool AHordeBaseCharacter::UpdatePlayerMovementSpeed_Validate(float NewMovementSpeed)
{
	return true;
}

void AHordeBaseCharacter::UpdateHeadDisplayWidget_Implementation(const FString& PlayerName, float PlayerHealth)
{
	UPlayerHeadDisplay* PHD = Cast<UPlayerHeadDisplay>(PlayerNameWidget->GetUserWidgetObject());
	if (PHD)
	{
		PHD->PlayerName = PlayerName;
		PHD->Health = PlayerHealth;
	}
}

bool AHordeBaseCharacter::UpdateHeadDisplayWidget_Validate(const FString& PlayerName, float PlayerHealth)
{
	return true;
}



void AHordeBaseCharacter::ServerStartSprinting_Implementation()
{
	if (!IsSprinting && GetVelocity().SizeSquared() > 10.f && GetCharacterMovement()->IsMovingOnGround())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(TimerIncreaseStamina))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerIncreaseStamina);
		}
		GetWorld()->GetTimerManager().SetTimer(TimerDecreaseStamina, this, &AHordeBaseCharacter::DecreaseStamina, .1f, true);
		IsSprinting = true;
		UpdatePlayerMovementSpeed(600.f);
	}
}

bool AHordeBaseCharacter::ServerStartSprinting_Validate()
{
	return true;
}

void AHordeBaseCharacter::ServerStopSprinting_Implementation()
{
	if(IsSprinting)
	{
		if(GetWorld()->GetTimerManager().IsTimerActive(TimerDecreaseStamina))
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerDecreaseStamina);
		}
		IsSprinting = false;
		GetWorld()->GetTimerManager().SetTimer(TimerIncreaseStamina, this, &AHordeBaseCharacter::IncreaseStamina, .1f, true);
		UpdatePlayerMovementSpeed(300.f);
	}
}

bool AHordeBaseCharacter::ServerStopSprinting_Validate()
{
	return true;
}

void AHordeBaseCharacter::DecreaseStamina()
{
	if (Stamina > .0f && GetVelocity().SizeSquared() > 10.f)
	{
		Stamina = FMath::Clamp<float>((Stamina - StaminaDecreaseRate), 0.f, 100.f);
	}
	else {
		ServerStopSprinting();
	}
}

void AHordeBaseCharacter::IncreaseStamina()
{
	if (Stamina < 100.f && !IsSprinting)
	{
		Stamina = FMath::Clamp<float>((Stamina + StaminaRefreshRate), 0.f, 100.f);
	}
}

void AHordeBaseCharacter::ActiveItemChanged(FString ItemID, int32 ItemIndex, int32 LoadedAmmo)
{
	StopWeaponFire();
	if (CurrentSelectedFirearm)
	{
		CurrentSelectedFirearm->Destroy();
	}

	FItem NewFirearmItem = UInventoryHelpers::FindItemByID(FName(*ItemID));
	if (NewFirearmItem.ItemID != "None" && NewFirearmItem.FirearmClass != nullptr)
	{
		FTransform NullTransform;
		CurrentSelectedFirearm = GetWorld()->SpawnActorDeferred<ABaseFirearm>(NewFirearmItem.FirearmClass, NullTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (CurrentSelectedFirearm)
		{
			CurrentSelectedFirearm->WeaponID = ItemID;
			CurrentSelectedFirearm->LoadedAmmo = LoadedAmmo;
			CurrentSelectedFirearm->FireMode = (uint8)NewFirearmItem.DefaultFireMode;
			CurrentSelectedFirearm->FinishSpawning(NullTransform);
			CurrentSelectedFirearm->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NewFirearmItem.AttachmentPoint);
		}
	}
	AnimMode = NewFirearmItem.AnimType;
}

AHordeBaseHUD* AHordeBaseCharacter::GetHUD()
{
	AHordeBaseHUD* returnObj = nullptr;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) {
		returnObj = Cast<AHordeBaseHUD>(PC->GetHUD());
	}
	return returnObj;
}

void AHordeBaseCharacter::StopWeaponFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(WeaponFireTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireTimer);
	}
}

void AHordeBaseCharacter::TriggerWeaponFire()
{
	if (!(Reloading || IsBursting) && !IsDead)
	{
		if (CurrentSelectedFirearm)
		{
			CurrentWeaponInfo = UInventoryHelpers::FindItemByID(FName(*CurrentSelectedFirearm->WeaponID));
			if (CurrentWeaponInfo.FirearmClass != nullptr)
			{
				switch ((EFireMode)CurrentSelectedFirearm->FireMode) 
				{

				case EFireMode::EFireModeBurst:
					GetWorld()->GetTimerManager().SetTimer(BurstTimer, this, &AHordeBaseCharacter::BurstWeapon, CurrentWeaponInfo.FireRate, true);
					IsBursting = true;
					break;

				case EFireMode::EFireModeFull:
					CurrentSelectedFirearm->ServerFireFirearm();
					GetWorld()->GetTimerManager().SetTimer(WeaponFireTimer, this, &AHordeBaseCharacter::AutoFireWeapon, CurrentWeaponInfo.FireRate, true);
					break;

				case EFireMode::EFireModeSingle:
					CurrentSelectedFirearm->ServerFireFirearm();
				break;

				default:
					break;
				}
			}
		}
	}
}

void AHordeBaseCharacter::BurstWeapon()
{
	if (NumberOfBursts >= CurrentWeaponInfo.BurstFireAmount || IsDead)
	{
		GetWorld()->GetTimerManager().ClearTimer(BurstTimer);
		NumberOfBursts = 0.f;
		IsBursting = false;
	}
	else {
		if (CurrentSelectedFirearm)
		{
			CurrentSelectedFirearm->ServerFireFirearm();
			NumberOfBursts = NumberOfBursts + 1.f;
		}
	}
}

void AHordeBaseCharacter::AutoFireWeapon()
{
	if (CurrentSelectedFirearm)
	{
		CurrentSelectedFirearm->ServerFireFirearm();
		if (Reloading || IsDead)
		{
			StopWeaponFire();
		}
	}
}

void AHordeBaseCharacter::ToggleFiremode()
{
	if(CurrentSelectedFirearm)
	{
		CurrentSelectedFirearm->ServerToggleFireMode();

		USoundCue* ToggleFireModeSound = ObjectFromPath<USoundCue>(FName(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_ToggleFiremode_Cue.A_ToggleFiremode_Cue'")));
		if (ToggleFireModeSound)
		{
			PlaySoundOnAllClients(ToggleFireModeSound, GetMesh()->GetComponentLocation());
		}
	}
}



void AHordeBaseCharacter::FinishReload()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ReloadTimerHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}
	if (CurrentSelectedFirearm)
	{
		FItem TempItem = UInventoryHelpers::FindItemByID(FName(*CurrentSelectedFirearm->WeaponID));
		int32 AmmoIndex;
		int32 AmmoAmount = Inventory->CountAmmo(TempItem.AmmoType, AmmoIndex);
		if (AmmoAmount >= (TempItem.MaximumLoadedAmmo - CurrentSelectedFirearm->LoadedAmmo))
		{
			Inventory->RemoveAmmoByType(TempItem.AmmoType, (TempItem.MaximumLoadedAmmo - CurrentSelectedFirearm->LoadedAmmo));
			CurrentSelectedFirearm->LoadedAmmo = TempItem.MaximumLoadedAmmo;
		}
		else
		{
			CurrentSelectedFirearm->LoadedAmmo = AmmoAmount;
			Inventory->RemoveAmmoByType(TempItem.AmmoType, AmmoAmount);
		}
		Reloading = false;
		Inventory->UpdateCurrentItemAmmo(CurrentSelectedFirearm->LoadedAmmo);
	}
}



void AHordeBaseCharacter::ServerReload_Implementation()
{
	if (!Reloading)
	{
		if (CurrentSelectedFirearm)
		{
			FItem TempItem = UInventoryHelpers::FindItemByID(FName(*CurrentSelectedFirearm->WeaponID));
			int32 AmmoIndex;
			int32 AmmoAmount = Inventory->CountAmmo(TempItem.AmmoType, AmmoIndex);
			if (AmmoAmount > 0 && (TempItem.DefaultLoadedAmmo != CurrentSelectedFirearm->LoadedAmmo))
			{
				Reloading = true;
				if (TempItem.PlayerAnimationData.CharacterReloadAnimation)
				{
					float AnimationDuration = TempItem.PlayerAnimationData.CharacterReloadAnimation->CalculateSequenceLength();
					PlayAnimationAllClients(TempItem.PlayerAnimationData.CharacterReloadAnimation);
					GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AHordeBaseCharacter::FinishReload, AnimationDuration, false);
				}
				else {
					FinishReload();
				}
			}
		}
	}
}

bool AHordeBaseCharacter::ServerReload_Validate()
{
	return true;
}

void AHordeBaseCharacter::AddHealth(float InHealth)
{
	if (HasAuthority())
	{
		Health = FMath::Clamp<float>(Health + InHealth, 0.f, 100.f);
		UpdateHeadDisplayWidget(GetPlayerState()->GetPlayerName(), Health);
	}
}

UUserWidget* AHordeBaseCharacter::GetHeadDisplayWidget()
{
	return PlayerNameWidget->GetUserWidgetObject();
}

ABaseFirearm* AHordeBaseCharacter::GetCurrentFirearm()
{
	return CurrentSelectedFirearm;
}

/*
INPUT FUNCTIONS
*/
void AHordeBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("DropItem", IE_Pressed, this, &AHordeBaseCharacter::DropCurrentItem);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AHordeBaseCharacter::ServerReload);

	PlayerInputComponent->BindAction("MouseUp", IE_Pressed, this, &AHordeBaseCharacter::ScrollUpItems);
	PlayerInputComponent->BindAction("MouseDown", IE_Pressed, this, &AHordeBaseCharacter::ScrollDownItems);

	PlayerInputComponent->BindAction("Primary", IE_Pressed, this, &AHordeBaseCharacter::SwitchToPrimary);
	PlayerInputComponent->BindAction("Secondary", IE_Pressed, this, &AHordeBaseCharacter::SwitchToSecondary);
	PlayerInputComponent->BindAction("Healing", IE_Pressed, this, &AHordeBaseCharacter::SwitchToHealing);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHordeBaseCharacter::TriggerWeaponFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AHordeBaseCharacter::StopWeaponFire);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AHordeBaseCharacter::ServerStartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AHordeBaseCharacter::ServerStopSprinting);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AHordeBaseCharacter::StartInteraction);
	PlayerInputComponent->BindAction("Use", IE_Released, this, &AHordeBaseCharacter::StopInteraction);

	PlayerInputComponent->BindAction("Toggle Firemode", IE_Pressed, this, &AHordeBaseCharacter::ToggleFiremode);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHordeBaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHordeBaseCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

}

void AHordeBaseCharacter::DropCurrentItem()
{
	if (CurrentSelectedFirearm && !Reloading)
	{
		StopWeaponFire();
		Inventory->ServerDropItem(CurrentSelectedFirearm);
	}
}

void AHordeBaseCharacter::SwitchToPrimary()
{
	if (Inventory)
	{
		Inventory->SwitchWeapon(EActiveType::EActiveRifle);
	}
}

void AHordeBaseCharacter::SwitchToSecondary()
{
	if (Inventory)
	{
		Inventory->SwitchWeapon(EActiveType::EActivePistol);
	}
}


void AHordeBaseCharacter::SwitchToHealing()
{
	if (Inventory)
	{
		Inventory->SwitchWeapon(EActiveType::EActiveMed);
	}
}

void AHordeBaseCharacter::ScrollUpItems()
{
	if (Inventory)
	{
		Inventory->ScrollItems(true);
	}
}

void AHordeBaseCharacter::ScrollDownItems()
{
	if (Inventory)
	{
		Inventory->ScrollItems(false);
	}
}

void AHordeBaseCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHordeBaseCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}