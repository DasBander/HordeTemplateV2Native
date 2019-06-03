

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionInterface.h"
#include "Gameplay/GameplayStructures.h"
#include "InventoryBaseItem.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AInventoryBaseItem : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AInventoryBaseItem();

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Inventory Item")
		FItem ItemInfo;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Inventory Item")
		FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Item")
		bool Spawned = false;

	


protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firearm")
		UStaticMeshComponent* WorldMesh;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* InteractingOwner);
	virtual void Interact_Implementation(AActor* InteractingOwner) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FInteractionInfo GetInteractionInfo();
	virtual FInteractionInfo GetInteractionInfo_Implementation() override;

	void PopInfo();

	virtual void OnConstruction(const FTransform& Transform) override;

};
