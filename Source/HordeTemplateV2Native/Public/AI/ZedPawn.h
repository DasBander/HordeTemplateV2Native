

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "ZedPawn.generated.h"

UCLASS()
class HORDETEMPLATEV2NATIVE_API AZedPawn : public ACharacter
{
	GENERATED_BODY()

public:
	AZedPawn();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Zed AI")
		class UArrowComponent* AttackPoint;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Zed AI")
		class USphereComponent* PlayerRangeCollision;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zed AI")
		float Health = 100.f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Zed AI")
		bool IsDead = false;

public:	
	FORCEINLINE float GetHealth() { return Health; };
	FORCEINLINE bool GetIsDead() { return IsDead; };
};
