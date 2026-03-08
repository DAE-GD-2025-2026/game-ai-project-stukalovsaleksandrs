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
	if (auto const Result{ pFlock->GetAverageNeighborLocation() };
		Result.has_value())
	{
		Target.Position = Result.value();
		return Seek::CalculateSteering(DeltaTime, Agent);
	}
	return SteeringOutput{};
}

//*********************
//SEPARATION (FLOCKING)
SteeringOutput Separation::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	for (ASteeringAgent const* const Neighbor : Flock->GetNeighbors())
	{
		if (!Neighbor) continue;
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

void Separation::DebugDraw(ASteeringAgent const& Agent, float const NeighborhoodRadius)
{
	DrawDebugCircle(
		Agent.GetWorld(),
		Agent.GetActorLocation(),
		NeighborhoodRadius,
		32, FColor::Blue, false,
		-1.f, 0, 5,
		FVector(0, 1, 0), FVector
		(1, 0, 0), false
	);
}

//*************************
//VELOCITY MATCH (FLOCKING)
SteeringOutput VelocityMatch::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	for (ASteeringAgent const* const Neighbor : m_pFlock->GetNeighbors())
	{
		if (Neighbor)
		{
			Steering.LinearVelocity += Neighbor->GetLinearVelocity();
		}
	}

	if (int const NeighborCount{ m_pFlock->GetNeighborCount()})// Avoiding division by 0
	{
		Steering.LinearVelocity /= static_cast<float>(NeighborCount);
	}
	return Steering;
}
