#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"
#include "DrawDebugHelpers.h"
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
			Distance < m_AvoidanceRadius)
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
		Steering.LinearVelocity *= m_SeparationFactor;
	}


	// 3. Debug output
	if (Agent.GetDebugRenderingEnabled())
	{
		// 3.1. Avoidance radius
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), m_AvoidanceRadius, 32, FColor::Blue, false, 0.025f, 0, 5, FVector(0, 1, 0), FVector(1, 0, 0), false);
		// 3.2. Direction vector
		DrawDebugLine(
			Agent.GetWorld(),
			Agent.GetActorLocation(),
			Agent.GetActorLocation() + FVector(Steering.LinearVelocity, 0.0),
			FColor::Blue, false, 0.025f, 0, 5
			);
	}
		
	return Steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	return Seek::CalculateSteering(DeltaTime, Agent);
}
