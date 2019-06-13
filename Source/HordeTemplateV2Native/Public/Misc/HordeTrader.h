

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Inventory/InteractionInterface.h"
#include "HordeTrader.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AHordeTrader : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	
	AHordeTrader();

	UFUNCTION(NetMulticast, WithValidation, Reliable, BlueprintCallable, Category = "Trader")
		void PlayWelcome();

	UFUNCTION(NetMulticast, WithValidation, Reliable, BlueprintCallable, Category = "Trader")
		void PlayGoodBye();

protected:


	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Trader")
		class USkeletalMeshComponent* TraderMeshComponent; 

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Trader")
		class UTextRenderComponent* TraderTextComponent;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void Interact(AActor* InteractingOwner);
	virtual void Interact_Implementation(AActor* InteractingOwner) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FInteractionInfo GetInteractionInfo();
	virtual FInteractionInfo GetInteractionInfo_Implementation() override;



};
