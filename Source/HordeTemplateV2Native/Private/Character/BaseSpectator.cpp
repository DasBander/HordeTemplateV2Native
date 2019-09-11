

#include "BaseSpectator.h"

/*
	FUNCTION: Constructor for ABaseSpectator
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for ABaseSpectator

*/
ABaseSpectator::ABaseSpectator()
{

}

/*
	FUNCTION: Setup Player Input Component
	PARAM: UInputComponent ( Player Input Component )
	RETURN: void
	DESC:
	Sets up Key Bindings for Player.
*/
void ABaseSpectator::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABaseSpectator::ServerFocusPlayer);

}

/*
	FUNCTION: Client Focus Player ( Client )
	PARAM: AHordeBaseCharacter ( Player to Focus )
	RETURN: void
	DESC:
	Focuses Player by Player Object.
*/
void ABaseSpectator::ClientFocusPlayer_Implementation(AHordeBaseCharacter* Player)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetViewTargetWithBlend(Player, 1.f, VTBlend_EaseIn, 0.5f, true);
	}
}

/*
	FUNCTION: Get Random Alive Player
	PARAM: None
	RETURN: AHordeBaseCharacter ( Alive Player Object )
	DESC:
	Gets random alive player from the World.
*/
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

/*
	FUNCTION: Server Focus Player
	PARAM: None
	RETURN: void
	DESC:
	Gets Random Alive Player and focuses him.
*/
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
