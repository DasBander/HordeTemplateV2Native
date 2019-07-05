

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/InteractionInterface.h"
#include "HordeTemplateV2Native.h"
#include "Door.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API ADoor : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	ADoor();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class USkeletalMeshComponent* DoorMesh;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void Interact(AActor* InteractingOwner);
		virtual void Interact_Implementation(AActor* InteractingOwner) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FInteractionInfo GetInteractionInfo();
		virtual FInteractionInfo GetInteractionInfo_Implementation() override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Door")
		bool bIsOpen = false;
};
