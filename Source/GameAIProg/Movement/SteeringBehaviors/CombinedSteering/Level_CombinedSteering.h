// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombinedSteeringBehaviors.h"
#include "GameAIProg/Shared/Level_Base.h"
#include "GameAIProg/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/Steering/Level_SteeringBehaviors.h"
#include "Level_CombinedSteering.generated.h"

UCLASS()
class GAMEAIPROG_API ALevel_CombinedSteering final : public ALevel_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevel_CombinedSteering();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

private:
	// NOTE: Smart pointers give segmentation faults when exiting the game,
	// so raw pointers are used
	
	// Agents
	// Example of blend between Seek and Wander.
	// It is referred to as drunk, because it goes
	// towards target while roaming from side to side
	ASteeringAgent* BlendedSteeringAgent{};// Non-owning

	// Example Combines Wander and Evade.
	// Evades when close to the other agent, otherwise it wanders
	ASteeringAgent* PrioritySteeringAgent{};

	// Steering behaviors
	std::unique_ptr<Seek> SeekBehavior{};
	std::unique_ptr<Wander> WanderBehavior{};
	std::unique_ptr<Evade> EvadeBehavior{};
	std::unique_ptr<FBlendedSteering> BlendedBehavior{};
	// FPrioritySteering* PriorityBehavior;
	
	bool UseMouseTarget{}, CanDebugRender{};
};
