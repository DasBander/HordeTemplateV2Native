

#include "IsEnemyDead.h"
#include "Character/HordeBaseCharacter.h"
#include "AI/ZedPawn.h"
#include "AI/ZedAIController.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
UIsEnemyDead::UIsEnemyDead()
{
	NodeName = "Check: Is Enemy Dead?";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UIsEnemyDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
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
