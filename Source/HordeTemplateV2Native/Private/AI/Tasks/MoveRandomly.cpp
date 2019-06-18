

#include "MoveRandomly.h"
#include "AIModule/Classes/AIController.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"


UMoveRandomly::UMoveRandomly()
{
	NodeName = "Zed Move Randomly";
	bNotifyTick = true;
}

void UMoveRandomly::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* MyController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (MyController)
	{
		UBlackboardComponent* BCP = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());
		if (BCP)
		{
			if (BCP->GetValueAsObject("Enemy") != nullptr)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
		if (MyController->GetMoveStatus() == EPathFollowingStatus::Idle)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}

}

EBTNodeResult::Type UMoveRandomly::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* MyController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (MyController)
	{
		APawn* Pawn = Cast<APawn>(MyController->GetPawn());
		if (Pawn)
		{
			if (MyController->GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				FAIMoveRequest MoveRequest;
				FVector LocationToMove;
				UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), Pawn->GetActorLocation(), LocationToMove, 500.f, nullptr, nullptr);
				MoveRequest.SetGoalLocation(LocationToMove);
				MyController->MoveTo(MoveRequest);
				return EBTNodeResult::InProgress;
			}
		}
	
	}
	return EBTNodeResult::Failed;
}
