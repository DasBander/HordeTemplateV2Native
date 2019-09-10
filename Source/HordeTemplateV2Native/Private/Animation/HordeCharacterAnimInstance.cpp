

#include "HordeCharacterAnimInstance.h"

/*
    FUNCTION: Native Begin Play
    PARAM: None
    RETURN: void
    DESC:
	Sets the Default Character Variable.
*/
void UHordeCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Character = Cast<AHordeBaseCharacter>(TryGetPawnOwner());
}

/*
    FUNCTION: Native Update Animation 
    PARAM: float ( Delta Seconds )
    RETURN: void
    DESC:
	Sets the Character Animation Variables.
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
