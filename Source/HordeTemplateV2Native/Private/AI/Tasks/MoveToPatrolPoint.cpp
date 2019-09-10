

#include "MoveToPatrolPoint.h"
#include "EngineUtils.h"
#include "AI/AICorePoint.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/AIController.h"

/*
	FUNCTION: Constructor for UMoveToPatrolPoint
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for UMoveToPatrolPoint

*/
UMoveToPatrolPoint::UMoveToPatrolPoint()
{
	NodeName = "Move to Patrol Point";
	bNotifyTick = true;
}

/*
	FUNCTION: Tick Task
	PARAM: UBehaviorTreeComponent ( Owner Component ), uint8 ( NodeMemory ), float ( DeltaSeconds )
	RETURN: void
	DESC:
	Finishes latent Task if Enemy is Set ort Move Status is Idle. Also clears Patrol Tag;

*/
void UMoveToPatrolPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
			if (MyController->GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			BCP->SetValueAsName("PatrolTag", NAME_None);
		}
	
	}
}

/*
	FUNCTION: Execute Task
	PARAM: UBehaviorTreeComponent ( Owner Component ), uint8 ( NodeMemory )
	RETURN: EBTNodeResult::Type
	DESC:
	Moves to Patrol Tag Location.

*/
EBTNodeResult::Type UMoveToPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* MyController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (MyController)
	{
		UBlackboardComponent* BCP = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());
		if (BCP)
		{
			if (MyController->GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				FAIMoveRequest MoveRequest;
				FVector LocationToMove = GetPatrolLocation(BCP->GetValueAsName("PatrolTag"));
				MoveRequest.SetGoalLocation(LocationToMove);
				MyController->MoveTo(MoveRequest);
				return EBTNodeResult::InProgress;
			}
		}
	}
	return EBTNodeResult::Failed;
}

/*
	FUNCTION: Get Patrol Location
	PARAM: FName ( Patrol Tag )
	RETURN: FVector ( Patrol Location )
	DESC:
	Returns random Patrol Location of AICorePoints by given Patrol Tag.

*/
FVector UMoveToPatrolPoint::GetPatrolLocation(FName PatrolTag)
{
	TArray<FVector> PatrolLocations;
	for(TActorIterator<AAICorePoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AAICorePoint* CorePoint = *ActorItr;
		if (CorePoint && CorePoint->PatrolTag == PatrolTag)
		{
			PatrolLocations.Add(CorePoint->GetActorLocation());
		}
	}
	int32 RetVal = FMath::RandRange(0, PatrolLocations.Num() - 1);
	return (PatrolLocations.IsValidIndex(RetVal)) ? PatrolLocations[RetVal] : FVector::ZeroVector;
}
