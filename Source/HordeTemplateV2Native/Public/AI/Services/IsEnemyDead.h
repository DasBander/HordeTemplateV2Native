

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "IsEnemyDead.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UIsEnemyDead : public UBTService
{
	GENERATED_BODY()
public:

	UIsEnemyDead();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;


};
