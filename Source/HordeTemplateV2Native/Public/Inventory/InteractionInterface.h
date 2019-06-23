#pragma once
#include "EngineMinimal.h"
#include "Sound/SoundCue.h"
#include "InteractionInterface.generated.h"

USTRUCT(BlueprintType)
struct FInteractionInfo {
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		FText InteractionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool HideKeyInText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		float InteractionTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		USoundCue * InteractionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
		bool AllowedToInteract;



	FInteractionInfo()
	{
		InteractionText = FText::FromString("None");
		InteractionTime = 0.f;
		InteractionSound = nullptr;
		AllowedToInteract = true;
		HideKeyInText = false;
	}
};


/**
 * 
 */
UINTERFACE(BlueprintType)
class HORDETEMPLATEV2NATIVE_API UInteractionInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
	
};

class HORDETEMPLATEV2NATIVE_API IInteractionInterface
{
	GENERATED_IINTERFACE_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		void Interact(AActor* InteractingOwner);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
		FInteractionInfo GetInteractionInfo();
};
