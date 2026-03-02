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
	for (ASteeringAgent const* const Neighbor : m_pFlock->GetNeighbors())
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
	if (int const NeighborCount{ m_pFlock->GetNeighborCount()})// Avoiding division by 0
	{
		Steering.LinearVelocity /= NeighborCount;
	}
	Steering.LinearVelocity *= m_SeparationFactor;

	assert(!Steering.LinearVelocity.ContainsNaN());
	
	// 3. Debug output
	if (Agent.GetDebugRenderingEnabled())
	{
		// 3.1. Avoidance radius
		DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), m_pFlock->GetNeighborhoodRadius(), 32, FColor::Blue, false, 0.025f, 0, 5, FVector(0, 1, 0), FVector(1, 0, 0), false);
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
