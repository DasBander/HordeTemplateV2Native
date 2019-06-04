

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "Character/HordeBaseCharacter.h"
#include "BaseSpectator.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API ABaseSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

public:

	ABaseSpectator();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, WithValidation, Reliable, Category = "Spectator")
		void ServerFocusPlayer();

	UFUNCTION(Client, Reliable, Category = "Spectator")
		void ClientFocusPlayer(AHordeBaseCharacter* Player);

	AHordeBaseCharacter* GetRandomAlivePlayer();
};
