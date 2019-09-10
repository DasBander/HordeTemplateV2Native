

#include "ZedAIController.h"
#include "AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "HordeTemplateV2Native.h"
#include "Character/HordeBaseCharacter.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"


/*
	FUNCTION: Constructor for AZedAIController
	PARAM: None
	RETURN: None
	DESC:
	Default Constructor for AZedAIController. Populates Default Values for Component.

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
	FUNCTION: Enemy In Sight
	PARAM: AActor ( Enemy ), FAIStimulus ( Stimulus )
	RETURN: void
	DESC:
	Detects Enemy Range Bases and sets it inside the Blackboard. Also starts clear sight timer in RandomRange Time Zone.

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
	FUNCTION: Clear Sight
	PARAM: None
	RETURN: void
	DESC:
	Sets Enemy as nullptr in Blackboard.

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
	FUNCTION: Begin Play
	PARAM: None
	RETURN: void
	DESC:
	Runs Behavior Tree on Server.

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
