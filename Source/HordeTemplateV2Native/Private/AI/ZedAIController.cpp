

#include "ZedAIController.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "HordeTemplateV2Native.h"
#include "Character/HordeBaseCharacter.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"

/*
	AI Controller Constructor
*/
AZedAIController::AZedAIController()
{
	PCC = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));

	UAISenseConfig_Sight* sightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	sightConfig->DetectionByAffiliation.bDetectEnemies = true;
	sightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	sightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	sightConfig->SightRadius = ZED_SIGHT_RADIUS;
	sightConfig->LoseSightRadius = ZED_LOSE_SIGHT_RADIUS;
	sightConfig->SetMaxAge(10.f);

	PCC->ConfigureSense(*sightConfig);
	PCC->SetDominantSense(sightConfig->GetSenseImplementation());
	PCC->OnTargetPerceptionUpdated.AddDynamic(this, &AZedAIController::EnemyInSight);
}

/*
	AI - Enemy in Sight.
	
	Set Enemy in Blackboard and set timer to loose sight after time.
*/
void AZedAIController::EnemyInSight(AActor* Actor, FAIStimulus Stimulus)
{
	AHordeBaseCharacter* Enemy = Cast<AHordeBaseCharacter>(Actor);
	if (Enemy)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(SightClearTimer))
		{
			GetWorld()->GetTimerManager().SetTimer(SightClearTimer, this, &AZedAIController::ClearSight, FMath::FRandRange(ZED_LOSE_SIGHT_TIME_MIN, ZED_LOSE_SIGHT_TIME_MAX), false);
		}
		UBlackboardComponent* BB = GetBlackboardComponent();
		if (BB)
		{
			BB->SetValueAsObject("Enemy", Enemy);
		}
	}
}

/*
	AI - Clear Enemy Sight

	Sets Enemy as nullptr in Blackboard
*/
void AZedAIController::ClearSight()
{
	UBlackboardComponent* BB = GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsObject("Enemy", nullptr);
	}
}

/*
	Begin Play

	Runs Behavior Tree.
*/
void AZedAIController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UBehaviorTree* AITree = ObjectFromPath<UBehaviorTree>(ZED_BEHAVIORTREE_ASSET_PATH);
		if (AITree)
		{
			RunBehaviorTree(AITree);
		}

	}
}
