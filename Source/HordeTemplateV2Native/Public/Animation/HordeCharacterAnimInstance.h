

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/HordeBaseCharacter.h"
#include "HordeCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UHordeCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:

	UFUNCTION()
		virtual void NativeBeginPlay() override;

	UFUNCTION()
		virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Animation")
		bool bIsInAir = false;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation")
		float Speed = 0.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation")
		int32 AnimationType = 0;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation")
		float AimRotation = 0.f;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Animation")
		float AimCorrection = 1.f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Animation")
		AHordeBaseCharacter* Character;
};
