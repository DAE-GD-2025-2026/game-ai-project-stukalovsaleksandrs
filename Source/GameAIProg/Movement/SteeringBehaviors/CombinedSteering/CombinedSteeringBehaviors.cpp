
#include "CombinedSteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "DrawDebugHelpers.h"
#include <algorithm>

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior>& WeightedBehaviors)
	:m_WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float DeltaTime, ASteeringAgent& SteeringAgent)
{
	SteeringOutput BlendedSteering = {};
	// TODO: Calculate the weighted average steeringbehavior
	// We have the multiple of steering behavior that we want to switch between right now
	// 1. Iterate over all the weighted steering behaviors
	for (auto const& WeightedBehavior : m_WeightedBehaviors)
	{
		SteeringOutput WeightedBehaviorSteering{ WeightedBehavior.pBehavior->CalculateSteering(DeltaTime, SteeringAgent) };
		// 2. We have 2 variables, values for which we have to calculate: direction and angular velocity
		// 2.1. Direction
		BlendedSteering.LinearVelocity += WeightedBehaviorSteering.LinearVelocity * WeightedBehavior.Weight;
		// 2.2. Angular velocity
		BlendedSteering.DegreesPerSec += WeightedBehaviorSteering.DegreesPerSec * WeightedBehavior.Weight;
	}

	// Debug output for the blended steering
	if (SteeringAgent.GetDebugRenderingEnabled())
	{
		DrawDebugLine(
			SteeringAgent.GetWorld(),
			SteeringAgent.GetActorLocation(),
			SteeringAgent.GetActorLocation() + FVector(BlendedSteering.LinearVelocity, 0.0),
			FColor::Green, false, 0.025f, 0, 5
			);
	}
	
	return BlendedSteering;
}

float* BlendedSteering::GetWeight(ISteeringBehavior* const SteeringBehavior)
{
	auto it = find_if(m_WeightedBehaviors.begin(),
		m_WeightedBehaviors.end(),
		[SteeringBehavior](const WeightedBehavior& Elem)
		{
			return Elem.pBehavior == SteeringBehavior;
		}
	);

	if(it!= m_WeightedBehaviors.end())
		return &it->Weight;
	
	return nullptr;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
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