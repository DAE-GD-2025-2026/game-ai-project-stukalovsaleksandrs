#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"
#include <cstdint>


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	Target.Position = pFlock->GetAverageNeighborLocation() - Agent.GetLocation();
	return Seek::CalculateSteering(DeltaTime, Agent);
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	uint32_t EvadedBoidCount{};
	for (ASteeringAgent* const pNeighbor : pFlock->GetNeighbors())
	{
		FVector2D const NeighborToAgent{ Agent.GetLocation() - pNeighbor->GetLocation() };
		if (double const Distance{ NeighborToAgent.Length() };
			Distance < AvoidanceRadius)
		{
			// 1. Normalizing the NeighborToAgent vector and dividing it by distance, then adding to the result
			Steering.LinearVelocity += NeighborToAgent.GetSafeNormal() / Distance;
			++EvadedBoidCount;
		}
	}
	// 2. Dividing the weightedVelocity by the boid count to make the velocity boid count-independent
	if (EvadedBoidCount > 0)
	{
		Steering.LinearVelocity /= EvadedBoidCount;
		Steering.LinearVelocity *= SeparationFactor;
	}

	return Steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	
	return Seek::CalculateSteering(DeltaTime, Agent);
}
