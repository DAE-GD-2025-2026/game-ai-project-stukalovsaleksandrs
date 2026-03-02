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
	for (ASteeringAgent const* const Neighbor : Flock->GetNeighbors())
	{
		FVector2D const NeighborToAgent{ Agent.GetLocation() - Neighbor->GetLocation() };
		// Avoiding division by increasingly small numbers to avoid getting NaN
		if (double const Distance{ NeighborToAgent.Length() };
			Distance > DBL_EPSILON)
		{
			// 1. Dividing the NeighborToAgent vector by distance squared, then adding to the result
			Steering.LinearVelocity += NeighborToAgent / NeighborToAgent.Length() / NeighborToAgent.Length();
			assert(!Steering.LinearVelocity.ContainsNaN());
		}
	}
		
	// 2. Dividing the weightedVelocity by the neighbor count to make the velocity neighbor count-independent
	if (int const NeighborCount{ Flock->GetNeighborCount()})// Avoiding division by 0
	{
		Steering.LinearVelocity /= NeighborCount;
	}
	Steering.LinearVelocity *= SeparationFactor;

	assert(!Steering.LinearVelocity.ContainsNaN());
		
	return Steering;
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	for (ASteeringAgent const* const pNeighbor : m_pFlock->GetNeighbors())
	{
		Steering.LinearVelocity += pNeighbor->GetLinearVelocity();
	}

	if (int const NeighborCount{ m_pFlock->GetNeighborCount()})// Avoiding division by 0
	{
		Steering.LinearVelocity /= static_cast<float>(NeighborCount);
	}
	return Steering;
}
