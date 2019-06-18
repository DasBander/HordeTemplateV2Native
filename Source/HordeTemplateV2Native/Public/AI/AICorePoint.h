

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "AICorePoint.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AAICorePoint : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Target")
		FName PatrolTag = NAME_None;


	AAICorePoint();

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class UBillboardComponent* Icon;

public:	


};
