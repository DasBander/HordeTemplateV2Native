

#include "BaseSpectator.h"

ABaseSpectator::ABaseSpectator()
{

}

void ABaseSpectator::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseSpectator::ServerFocusPlayer);

}

void ABaseSpectator::ClientFocusPlayer_Implementation(AHordeBaseCharacter* Player)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetViewTargetWithBlend(Player, 1.f, VTBlend_EaseIn, 0.5f, true);
	}
}

AHordeBaseCharacter* ABaseSpectator::GetRandomAlivePlayer()
{
	TArray<AHordeBaseCharacter*> AliveCharacter;

	for (TObjectIterator<AHordeBaseCharacter> Itr; Itr; ++Itr)
	{
		AHordeBaseCharacter* PLY = *Itr;
		if (!PLY->GetIsDead())
		{
			AliveCharacter.Add(PLY);
		}
	}
	return (AliveCharacter.Num() > 0) ? AliveCharacter[FMath::RandRange(0, AliveCharacter.Num() - 1)] : nullptr;
}

void ABaseSpectator::ServerFocusPlayer_Implementation()
{
	AHordeBaseCharacter* PLY = GetRandomAlivePlayer();
	if (PLY)
	{
		ClientFocusPlayer(PLY);
	}
}

bool ABaseSpectator::ServerFocusPlayer_Validate()
{
	return true;
}
