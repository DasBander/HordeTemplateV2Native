

#include "AIAttackPlayer.h"
#include "AI/ZedPawn.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "AIModule/Classes/AIController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/HordeBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

/*
	FUNCTION: Constructor UAIAttackPlayer
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for UAIAttackPlayer

*/
UAIAttackPlayer::UAIAttackPlayer()
{
	NodeName = "Attack Player";
}

/*
	FUNCTION: Execute Task
	PARAM: UBehaviorTreeComponent ( Owner Component ), uint8 ( NodeMemory )
	RETURN: EBTNodeResult::Type
	DESC:
	Executes Task to Attack the Player. Traces Player with SphereTraceSingle and applys damage to him.
*/

EBTNodeResult::Type UAIAttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	UBlackboardComponent* BBC = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());
	if (BBC)
	{
		AZedPawn* Zed = Cast<AZedPawn>(OwnerComp.GetAIOwner()->GetPawn());
		if (Zed && !Zed->GetIsDead())
		{
			TArray<AActor*> ActorsToIgnore = { Zed };
			FHitResult OutResult;
			if (UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Zed->AttackPoint->GetComponentLocation(), (Zed->AttackPoint->GetForwardVector() * 150.f) + Zed->AttackPoint->GetComponentLocation(), 16.f, ETraceTypeQuery::TraceTypeQuery15, false, ActorsToIgnore, EDrawDebugTrace::None, OutResult, true)) 
			{
				AHordeBaseCharacter* Char = Cast<AHordeBaseCharacter>(OutResult.GetActor());
				if (Char && !Char->GetIsDead())
				{
					UGameplayStatics::ApplyPointDamage(Char, FMath::RandRange(5.f, 9.f), Zed->GetActorLocation(), OutResult, OwnerComp.GetAIOwner(), OwnerComp.GetAIOwner()->GetPawn(), nullptr);
					Zed->PlayAttackFX();
				}
			}
		}
	}
	return EBTNodeResult::Succeeded;
}
