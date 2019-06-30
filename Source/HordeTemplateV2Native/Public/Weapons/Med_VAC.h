

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BaseFirearm.h"
#include "Components/StaticMeshComponent.h"
#include "Med_VAC.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API AMed_VAC : public ABaseFirearm
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "VAC")
		bool IsHealing = false;
	
protected:
	AMed_VAC();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class UStaticMeshComponent* VACMesh;

	virtual void FireFirearm() override;

	UFUNCTION(NetMulticast, WithValidation, Reliable, BlueprintCallable, Category = "VAC")
		void StartHealing();

	UFUNCTION(Server, WithValidation, Reliable, BlueprintCallable, Category = "VAC")
		void FinishHealing();
};
