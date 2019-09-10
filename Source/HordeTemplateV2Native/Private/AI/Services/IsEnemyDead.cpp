

#include "IsEnemyDead.h"
#include "Character/HordeBaseCharacter.h"
#include "AI/ZedPawn.h"
#include "AI/ZedAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

/*
	FUNCTION: Constructor UIsEnemyDead
	PARAM: None
	RETURN: None
	DESC:
	Constructor of Service IsEnemyDead

*/
UIsEnemyDead::UIsEnemyDead()
{
	NodeName = "Check: Is Enemy Dead?";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

/*
	FUNCTION: Tick Node
	PARAM: UBehaviorTreeComponent (Owner Component), uint8 (Node Memory), float (Delta Seconds)
	RETURN: void
	DESC:
	Tick Node which checks if Enemy is Dead ( Player Character ). If zombie killed player he ignores him.
*/
void UIsEnemyDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BBC = Cast<UBlackboardComponent>(OwnerComp.GetBlackboardComponent());
	if (BBC)
	{
		AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(BBC->GetValueAsObject("Enemy"));
		AZedPawn* Zed = Cast<AZedPawn>(OwnerComp.GetAIOwner()->GetPawn());
		if (PLY && PLY->GetIsDead())
		{
			BBC->SetValueAsObject("Enemy", nullptr);
			if (Zed && Zed->GetCharacterMovement()->MaxWalkSpeed != 200.f)
			{
				Zed->ModifyWalkSpeed(200.f);
			}
			OwnerComp.GetAIOwner()->SetFocus(nullptr);
		}
		else if(PLY && !PLY->GetIsDead())
		{
			if (Zed && Zed->GetCharacterMovement()->MaxWalkSpeed != 500.f)
			{
				Zed->ModifyWalkSpeed(500.f);
			}
		}
	}
}
