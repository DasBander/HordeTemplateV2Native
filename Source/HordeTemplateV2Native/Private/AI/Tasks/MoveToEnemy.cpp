

#include "MoveToEnemy.h"
#include "AIModule/Classes/AIController.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "Character/HordeBaseCharacter.h"

/*
	FUNCTION: Constructor UMoveToEnemy
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for UMoveToEnemy

*/
UMoveToEnemy::UMoveToEnemy()
{
	NodeName = "Move To Enemy Player";
	bNotifyTick = true;
}

/*
	FUNCTION: Tick Task
	PARAM: UBehaviorTreeComponent ( Owner Component ), uint8 ( NodeMemory ), float( DeltaSeconds )
	RETURN: void
	DESC:
	Finishes latent Task if Enemy is Set, the AI is dead, a player is in Range or if the movement status is idle.
*/
void UMoveToEnemy::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
			if (BCP->GetValueAsBool("IsDead"))
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			if (BCP->GetValueAsBool("PlayerInRange"))
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

/*
	FUNCTION: Execute Task
	PARAM: UBehaviorTreeComponent ( Owner Component ), uint8 ( NodeMemory )
	RETURN: EBTNodeResult::Type
	DESC:
	Follows Characters Position.

*/
EBTNodeResult::Type UMoveToEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* MyController = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (MyController)
	{
		UBlackboardComponent* BCP = Cast<UBlackboardComponent>(MyController->GetBlackboardComponent());
		if (BCP)
		{
			if (MyController->GetMoveStatus() == EPathFollowingStatus::Idle)
			{
				FAIMoveRequest MoveRequest;
				AHordeBaseCharacter* BChar = Cast<AHordeBaseCharacter>(BCP->GetValueAsObject("Enemy"));
				MyController->MoveToActor(BChar, 80.f, true, true, true);

				return EBTNodeResult::InProgress;
			}
			else if (MyController->GetMoveStatus() == EPathFollowingStatus::Moving)
			{
				return EBTNodeResult::InProgress;
			}
			else if (MyController->GetMoveStatus() == EPathFollowingStatus::Waiting)
			{
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;

}
