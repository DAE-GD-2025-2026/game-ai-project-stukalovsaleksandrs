// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Shared/Level_Base.h"
#include "DecisionMaking/Guard.h"
#include "Level_FSM.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_FSM : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_FSM();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGuard> GuardClass{};
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AGuard* Guard{};
	
	TArray<FVector> GetPatrolPoints() const;
	// Setting the player location in the blackboard
	UBlackboardComponent* GuardBlackboardComponent{};
	
	[[nodiscard]] bool DoesGuardSeePlayerCharacter() const;
};
