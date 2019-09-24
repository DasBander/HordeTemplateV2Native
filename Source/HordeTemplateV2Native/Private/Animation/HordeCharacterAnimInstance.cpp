

#include "HordeCharacterAnimInstance.h"

/**
 *	Constructor for UHordeCharacterAnimInstance
 *
 * @param
 * @return
 */
void UHordeCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Character = Cast<AHordeBaseCharacter>(TryGetPawnOwner());
}

/** ( Virtual ; Overridden)
 *	Sets the Character Animation Variables.
 *
 * @param DeltaSeconds
 * @return void
 */
void UHordeCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character)
	{
		bIsInAir = Character->GetCharacterMovement()->IsFalling();
		Speed = Character->GetVelocity().Size();
		AnimationType = Character->AnimMode;

		float AimPitch = (Character->IsLocallyControlled()) ? Character->GetControlRotation().Pitch : Character->GetRemotePitch();
		AimRotation = (AimPitch > 180.f) ? 360.f - AimPitch : AimPitch * -1.f;
	}
}
