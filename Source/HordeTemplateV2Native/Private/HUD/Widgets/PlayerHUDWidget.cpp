

#include "PlayerHUDWidget.h"
#include "GameFramework/Character.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeGameState.h"
#include "Gameplay/HordeWorldSettings.h"
#include "InventoryHelpers.h"
#include "HordeTemplateV2Native.h"

void UPlayerHUDWidget::HideInteractionTxt()
{
	OnHideInteractionText.Broadcast();
}

ESlateVisibility UPlayerHUDWidget::IsOwningCharacter()
{
	ESlateVisibility DefaultVisibilty = ESlateVisibility::Hidden;
	ACharacter* PLY = Cast<ACharacter>(GetOwningPlayerPawn());
	if (PLY)
	{
		DefaultVisibilty = ESlateVisibility::Visible;
	}
	return DefaultVisibilty;
}


float UPlayerHUDWidget::GetPlayerHealth()
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwningPlayerPawn());
	if (PLY)
	{
		HealthInterpolate = FMath::FInterpTo(HealthInterpolate, PLY->GetHealth(), GetWorld()->GetDeltaSeconds(), 2.f);
		return HealthInterpolate / 100.f;
	}
	else {
		return 0.f;
	}
}

float UPlayerHUDWidget::GetPlayerStamina()
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwningPlayerPawn());
	if (PLY)
	{
		StaminaInterpolate = FMath::FInterpTo(StaminaInterpolate, PLY->GetStamina(), GetWorld()->GetDeltaSeconds(), 2.f);
		return StaminaInterpolate / 100.f;
	}
	else {
		return 0.f;
	}
}

ESlateVisibility UPlayerHUDWidget::bGetIsDead()
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwningPlayerPawn());
	if (PLY)
	{
		return (PLY->GetIsDead()) ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
	}
	else
	{
		return ESlateVisibility::Collapsed;
	}
}

FText UPlayerHUDWidget::GetWeaponText()
{
	FString ReturnString;
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwningPlayerPawn());
	ABaseFirearm* Firearm = PLY->GetCurrentFirearm();
	if (PLY && Firearm)
	{
		ReturnString = (PLY->Inventory->AvailableAmmoForCurrentWeapon == 0) ? Firearm->WeaponID + " - " + FString::FromInt(Firearm->LoadedAmmo) : Firearm->WeaponID + " - " + FString::FromInt(Firearm->LoadedAmmo) + " / " + FString::FromInt(PLY->Inventory->AvailableAmmoForCurrentWeapon);
	}
	return FText::FromString(ReturnString);
}

FText UPlayerHUDWidget::GetHealthText()
{
	return FText::FromString(FString::SanitizeFloat(HealthInterpolate, 1));
}

FText UPlayerHUDWidget::GetZedsLeft()
{
	FString ReturnString = "None";
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		ReturnString = FString::FromInt(GS->ZedsLeft);
	}
	return FText::FromString(ReturnString);
}

FText UPlayerHUDWidget::GetRounds()
{
	FString ReturnString = "Unknown Rounds";
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		ReturnString = FString::FromInt(GS->GameRound) + " / " + FString::FromInt(GS->GetHordeWorldSettings()->MaxRounds) + " ROUNDS";
	}
	return FText::FromString(ReturnString);
}

ESlateVisibility UPlayerHUDWidget::bIsSpectator()
{
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwningPlayerPawn());
	return (PLY) ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;
}

ESlateVisibility UPlayerHUDWidget::bIsInteracting()
{
	return (IsInteracting) ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

FText UPlayerHUDWidget::GetWeaponFireMode()
{
	FString ReturnString;
	AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwningPlayerPawn());
	ABaseFirearm* Firearm = PLY->GetCurrentFirearm();
	if (PLY && Firearm)
	{
		switch ((EFireMode)Firearm->FireMode) {
			case EFireMode::EFireModeSingle:
				ReturnString = "Single";
			break;

			case EFireMode::EFireModeBurst:
				ReturnString = "Burst";
			break;

			case EFireMode::EFireModeFull:
				ReturnString = "Full Automatic";
			break;

			default:
			break;
		}
			
	}
	return FText::FromString(ReturnString);
}

ESlateVisibility UPlayerHUDWidget::GetGameType()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		return (GS->MatchMode == EMatchMode::EMatchModeNonLinear) ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
	}
	else {
		return ESlateVisibility::Collapsed;
	}
}

FText UPlayerHUDWidget::GetRoundTime()
{
	FFormatNamedArguments Args;
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		float LobbyTime = (GS->IsRoundPaused) ? GS->PauseTime : GS->RoundTime;

		int32 Minutes = FMath::FloorToInt(LobbyTime / 60.f);
		int32 Seconds = FMath::TruncToInt(LobbyTime - (Minutes * 60.f));

		FString TimeStr = FString::Printf(TEXT("%s%s : %s%s"), (Minutes < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Minutes), (Seconds < 10) ? TEXT("0") : TEXT(""), *FString::FromInt(Seconds));

		return FText::FromString(TimeStr + ((GS->IsRoundPaused) ? " Pause" : " Game Time"));
	}
	else {
		return FText::FromString("nA / nA");
	}
}

void UPlayerHUDWidget::NativeConstruct()
{
	
	/*
	Creates Dynamic Object for Circle Material so we can adjust the parameters for it.
	*/

	CircleMaterialInstance = ObjectFromPath<UMaterialInstanceConstant>(FName(TEXT("MaterialInstanceConstant'/Game/HordeTemplateBP/Assets/Materials/UI/MI_ProgressCircle.MI_ProgressCircle'")));

	if (CircleMaterialInstance)
	{
		ProgressCircleDynamic = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), CircleMaterialInstance);
	}

	OnSetInteractionText.AddDynamic(this, &UPlayerHUDWidget::SetInteractionText);
	OnHideInteractionText.AddDynamic(this, &UPlayerHUDWidget::HideInteractionText);
	OnBindingVariables.AddDynamic(this, &UPlayerHUDWidget::VariablesBound);

	OnBindingVariables.Broadcast();

	Super::NativeConstruct();
}

void UPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsInteracting && ProgressCircleDynamic)
	{

		AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(GetOwningPlayerPawn());
		if (PLY)
		{
			ProgressCircleDynamic->SetScalarParameterValue(FName(TEXT("Progress")), FMath::GetMappedRangeValueClamped(FVector2D(0.f, 100.f), FVector2D(0.f, 1.f), FMath::FInterpTo(PLY->InteractionProgress, InteractionValueToInterpolate, InDeltaTime, 0.1f)));
		}
	}
}

