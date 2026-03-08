
#include "CombinedSteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "DrawDebugHelpers.h"
#include <algorithm>

FBlendedSteering::FBlendedSteering(const std::vector<FWeightedBehavior>& WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput FBlendedSteering::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput BlendedSteering = {};
	// We have multiple steering behaviors that we want to switch between right now
	// 1. Iterate over all the weighted steering behaviors
	for (auto const& WeightedBehavior : WeightedBehaviors)
	{
		// Not processing behaviors with weights close to 0 
		if (WeightedBehavior.Weight < FLT_EPSILON) continue;
		SteeringOutput WeightedBehaviorSteering{ WeightedBehavior.Behavior->CalculateSteering(DeltaTime, Agent) };
		// 2. We have 2 variables, values for which we have to calculate: direction and angular velocity
		// 2.1. Direction
		BlendedSteering.LinearVelocity += WeightedBehaviorSteering.LinearVelocity.GetSafeNormal() * WeightedBehavior.Weight;
		// 2.2. Angular velocity
		BlendedSteering.DegreesPerSec += WeightedBehaviorSteering.DegreesPerSec * WeightedBehavior.Weight;
	}

	BlendedSteering.LinearVelocity.Normalize();
	return BlendedSteering;
}

float* FBlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = find_if(WeightedBehaviors.begin(),
		WeightedBehaviors.end(),
		[SteeringBehavior](const FWeightedBehavior& Elem)
		{
			return Elem.Behavior == SteeringBehavior;
		}
	);

	if(it!= WeightedBehaviors.end())
		return &it->Weight;
	
	return nullptr;
}

void FBlendedSteering::DebugDraw(ASteeringAgent const& Agent)
{
	FVector const AgentLocation{ Agent.GetActorLocation() };
	DrawDebugLine(
		Agent.GetWorld(),
		AgentLocation,
		AgentLocation + FVector(Agent.GetLinearVelocity().X, Agent.GetLinearVelocity().Y, AgentLocation.Z).GetSafeNormal() * 50.f,
		FColor::Red, false, 0.035f, 0, 5
	);
}

//*****************
//PRIORITY STEERING
SteeringOutput FPrioritySteering::CalculateSteering(float const DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior* const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);

		if (Steering.IsValid)
			break;
	}

	//If none of the behavior return a valid output, last behavior is returned
	return Steering;
}