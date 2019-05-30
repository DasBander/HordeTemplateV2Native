

#include "PlayerHUDWidget.h"
#include "GameFramework/Character.h"
#include "Character/HordeBaseCharacter.h"
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

