

#include "HordeBaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Inventory/InventoryHelpers.h"
#include "Gameplay/HordeGameMode.h"
#include "Weapons/BaseFirearm.h"
#include "HUD/Widgets/PlayerHeadDisplay.h"
#include "HordePlayerState.h"
#include "AIModule/Classes/Perception/AISense_Sight.h"
#include "FX/Camera/CameraShake_Damage.h"


/*
	FUNCTION: Get Lifetime Replicated Props
	PARAM: TArray - FLifetimeProperty ( Out Lifetime Props ) const
	RETURN: void
	DESC:
	Sets given variables as Replicated.
*/
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

/*
	FUNCTION: Constructor for AHordeBaseCharacter
	PARAM: None
	RETURN: None
	DESC:
	Sets default values of the Character and creates Components.
*/
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

/*
	FUNCTION: Begin Play
	PARAM: None
	RETURN: void
	DESC:
	Starts Interaction & HeadDisplay Trace Timer. Also Updates on server the HeadDisplayWidget.
*/
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


/*
	FUNCTION: Server Interact ( Server )
	PARAM: AActor ( Actor To Interact With )
	RETURN: void
	DESC:
	Interacts with given Actor and makes an Interface Call. Plays Interaction Sound as well.
*/
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

/*
	FUNCTION: Play Sound On All Clients ( Multicast )
	PARAM: USoundCue ( Sound ), FVector ( Location )
	RETURN: void
	DESC:
	Plays sound on all clients on given Location.
*/
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

/*
	FUNCTION: Take Damage
	PARAM: float ( Damage ), FDamageEvent ( Damage Event ), AController ( Event Instigator ), AActor ( Damage Causer )
	RETURN: float ( Health - Damage )
	DESC:
	Function to receive damage. Removes Health depending on the Damage. Runs CharacterDie() if Health is <= 0.f . Plays Camerashake on Client and Damage Sound on all clients.
*/
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

/*
	FUNCTION: Play Animation All Clients ( Multicast )
	PARAM: UAnimMontage ( Animation Montage )
	RETURN: void
	DESC:
	Plays Character Animation on all Clients.
*/
void AHordeBaseCharacter::PlayAnimationAllClients_Implementation(class UAnimMontage* Montage)
{
	UAnimMontage* Mont = ObjectFromPath<UAnimMontage>(TEXT("AnimMontage'/Game/HordeTemplateBP/Assets/Animations/AM_Reload_Rifle.AM_Reload_Rifle'"));
	GetMesh()->GetAnimInstance()->Montage_Play(Mont, 1.f, EMontagePlayReturnType::Duration, 0.f, false);

}

bool AHordeBaseCharacter::PlayAnimationAllClients_Validate(class UAnimMontage* Montage)
{
	return true;
}

/*
	FUNCTION: Remove Health
	PARAM: float ( Health To Remove )
	RETURN: bool ( true if Health is <= 0.f )
	DESC:
	Updates Health and returns if the Character is dead or not.
*/
bool AHordeBaseCharacter::RemoveHealth(float HealthToRemove)
{
	Health = FMath::Clamp<float>((Health - HealthToRemove), 0.f, 100.f);
	UpdateHeadDisplayWidget(GetPlayerState()->GetPlayerName(), Health);
	return (Health <= 0.f);
}

/*
	FUNCTION: Character Die
	PARAM: None
	RETURN: void
	DESC:
	Drops Current Item and lets Character Die. Sets lifespan to 20 seconds and enables physics on character. Spawns Spectator on server to possess with. 
*/
void AHordeBaseCharacter::CharacterDie()
{
	IsDead = true;
	if (CurrentSelectedFirearm)
	{
		Inventory->ServerDropItem(CurrentSelectedFirearm);
	}
	AHordePlayerState* PS = Cast<AHordePlayerState>(GetPlayerState());
	if (PS)
	{
		PS->bIsDead = true;
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
	RagdollPlayer();
	SetLifeSpan(20.f);
}

/*
	FUNCTION: Start Interaction
	PARAM: None
	RETURN: void
	DESC:
	Starts Interaction with Last Interaction Actor that you where facing.
*/
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

/*
	FUNCTION: Stop Interaction
	PARAM: None
	RETURN: void
	DESC:
	Stops Interaction.
*/
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

/*
	FUNCTION: Process Interaction
	PARAM: None
	RETURN: void
	DESC:
	Removes time from the Interaction. If Character is Dead, Not Allowed to Interact or Last Interaction Actor not valid the Interaction will be stopped. If Interaction Time is over calls Interact on Actor.
*/
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

/*
	FUNCTION: Head Display Trace
	PARAM: None
	RETURN: void
	DESC:
	Trace for Head Display Widget. Gets Information for other Players.

*/
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

/*
	FUNCTION: Interaction Detection
	PARAM: None
	RETURN: void
	DESC:
	Trace for Items to Interact with. Also Updates HUD with text to Display.
*/
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

/*
	FUNCTION: Ragdoll Player ( Multicast )
	PARAM: None
	RETURN: void
	DESC:
	Enables Physics for Character Mesh on all Clients.
*/
void AHordeBaseCharacter::RagdollPlayer_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionProfileName(FName(TEXT("DeadAI")));
}

bool AHordeBaseCharacter::RagdollPlayer_Validate()
{
	return true;
}

/*
	FUNCTION: Update Player Movement Speed
	PARAM: float ( New Movement Speed )
	RETURN: void
	DESC:
	Updates Characters Movement Speed on all clients.
*/
void AHordeBaseCharacter::UpdatePlayerMovementSpeed_Implementation(float NewMovementSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewMovementSpeed;
}

bool AHordeBaseCharacter::UpdatePlayerMovementSpeed_Validate(float NewMovementSpeed)
{
	return true;
}

/*
	FUNCTION: Update Head Display Widget ( Multicast )
	PARAM: FString ( Player Name ), float ( Player Health )
	RETURN: void
	DESC:
	Updates Player Head Display Information on all clients.
*/
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


/*
	FUNCTION: Server Start Sprinting ( Server )
	PARAM: None
	RETURN: void
	DESC:
	Starts Sprinting Timer and Decreases Stamina. Also Updates Movement Speed on all Clients
*/
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

/*
	FUNCTION: Server Stop Sprinting
	PARAM: None
	RETURN: void
	DESC:
	Stops Sprinting Timer and starts Increasing Stamina. Updates Player Movement Speed on all Clients.
*/
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

/*
	FUNCTION: Decrease Stamina
	PARAM: None
	RETURN: void
	DESC:
	Decreases Stamina. If Velocity is under 10.f and stamina 0.f then it stops sprinting.
*/
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

/*
	FUNCTION: Increase Stamina
	PARAM: None
	RETURN: void
	DESC:
	Increases Stamina with given Increase Rate.
*/
void AHordeBaseCharacter::IncreaseStamina()
{
	if (Stamina < 100.f && !IsSprinting)
	{
		Stamina = FMath::Clamp<float>((Stamina + StaminaRefreshRate), 0.f, 100.f);
	}
}

/*
	FUNCTION: Active Item Changed
	PARAM: FString ( Item ID ), int32 ( Item Index ), int32 ( Loaded Ammo )
	RETURN: void
	DESC:
	Function that gets called if the Active Item has been changed. Stops Weapon Fire and Destroys Firearm. Spawns new Firearm by given Item ID and updates Animation Mode.
*/
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

/*
	FUNCTION: Get HUD
	PARAM: None
	RETURN: AHordeBaseHUD ( HUD Object )
	DESC:
	Returns HUD Object.
*/
AHordeBaseHUD* AHordeBaseCharacter::GetHUD()
{
	AHordeBaseHUD* returnObj = nullptr;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) {
		returnObj = Cast<AHordeBaseHUD>(PC->GetHUD());
	}
	return returnObj;
}

/*
	FUNCTION: Stop Weapon Fire
	PARAM: None
	RETURN: void
	DESC:
	Stops Weapon Fire Timer.
*/
void AHordeBaseCharacter::StopWeaponFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(WeaponFireTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(WeaponFireTimer);
	}
}

/*
	FUNCTION: Trigger Weapon Fire
	PARAM: None
	RETURN: void
	DESC:
	Starts Weapon Fire depending on the FireMode of the Current Selected Weapon.
*/
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

/*
	FUNCTION: Burst Weapon
	PARAM: None
	RETURN: void
	DESC:
	Lets the Weapon burst depending on the Amount of Bursts defined in the Item Definition.
*/
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

/*
	FUNCTION: Auto Fire Weapon
	PARAM: None
	RETURN: void
	DESC:
	Lets the Weapon Automatic Fire and stops if reloading or dead.
*/
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

/*
	FUNCTION: Toggle Firemode
	PARAM: None
	RETURN: void
	DESC:
	Switches Firemode in Firearm and plays sound on all clients.
*/
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


/*
	FUNCTION: Finish Reload
	PARAM: None
	RETURN: void
	DESC:
	Finishes Reload by clearing all timers and updates the Ammo.
*/
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


/*
	FUNCTION: Server Reload ( Server )
	PARAM: None
	RETURN: void
	DESC:
	Initiates Reload for the Weapon and plays Animation.
*/
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

/*
	FUNCTION: Add Health
	PARAM: float ( Health to Add )
	RETURN: void
	DESC:
	Adds Health and updates Head Display Widget.
*/
void AHordeBaseCharacter::AddHealth(float InHealth)
{
	if (HasAuthority())
	{
		Health = FMath::Clamp<float>(Health + InHealth, 0.f, 100.f);
		UpdateHeadDisplayWidget(GetPlayerState()->GetPlayerName(), Health);
	}
}

/*
	FUNCTION: Get Head Display Widget
	PARAM: None
	RETURN: UUserWidget ( Head Display Widget )
	DESC:
	Returns HeadDisplayWidget Object
*/
UUserWidget* AHordeBaseCharacter::GetHeadDisplayWidget()
{
	return PlayerNameWidget->GetUserWidgetObject();
}

/*
	FUNCTION: Get Current Firearm
	PARAM: None
	RETURN: ABaseFirearm ( Firearm Object )
	DESC:
	Returns current Firearm Object.
*/
ABaseFirearm* AHordeBaseCharacter::GetCurrentFirearm()
{
	return CurrentSelectedFirearm;
}

/*
INPUT FUNCTIONS
*/
/*
	FUNCTION: Setup Player Input Component
	PARAM: UInputComponent ( Player Input Component )
	RETURN: void
	DESC:
	Sets Character Inputs and binds Functions to them.
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

/*
	FUNCTION: Drop Current Item
	PARAM: None
	RETURN: void
	DESC:
	Stops Weapon Fire and Drops Current Item. 
*/
void AHordeBaseCharacter::DropCurrentItem()
{
	if (CurrentSelectedFirearm && !Reloading)
	{
		StopWeaponFire();
		Inventory->ServerDropItem(CurrentSelectedFirearm);
	}
}

/*
	FUNCTION: Switch to Primary
	PARAM: None
	RETURN: void
	DESC:
	Switches Weapon to Primary.
*/
void AHordeBaseCharacter::SwitchToPrimary()
{
	if (Inventory)
	{
		Inventory->SwitchWeapon(EActiveType::EActiveRifle);
	}
}

/*
	FUNCTION: Switch To Secondary
	PARAM: None
	RETURN: void
	DESC:
	Switches Weapon to Secondary.
*/
void AHordeBaseCharacter::SwitchToSecondary()
{
	if (Inventory)
	{
		Inventory->SwitchWeapon(EActiveType::EActivePistol);
	}
}

/*
	FUNCTION: Switch To Healing
	PARAM: None
	RETURN: void
	DESC:
	Switches to Healing Item.
*/
void AHordeBaseCharacter::SwitchToHealing()
{
	if (Inventory)
	{
		Inventory->SwitchWeapon(EActiveType::EActiveMed);
	}
}

/*
	FUNCTION: Scroll Up Items
	PARAM: None
	RETURN: void
	DESC:
	Function for Mouse Wheel Up to scroll through Inventory.
*/
void AHordeBaseCharacter::ScrollUpItems()
{
	if (Inventory)
	{
		Inventory->ScrollItems(true);
	}
}

/*
	FUNCTION: Scroll Down Items
	PARAM: None
	RETURN: void
	DESC:
	Function for Mouse Wheel Down to scroll through Inventory.
*/
void AHordeBaseCharacter::ScrollDownItems()
{
	if (Inventory)
	{
		Inventory->ScrollItems(false);
	}
}

/*
	FUNCTION: Move Forward
	PARAM: float ( Forward / Backward Axis )
	RETURN: void
	DESC:
	Moves Character Forward and Backwards
*/
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

/*
	FUNCTION: Move Right
	PARAM: float ( Left / Right Axis )
	RETURN: void
	DESC:
	Moves Character Left and Right.
*/
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