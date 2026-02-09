#include "SteeringBehaviors.h"

#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"
#include "DrawDebugHelpers.h"

// Seek
SteeringOutput Seek::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	// NOTE: There is no need to normalize since AddMovementInput already normalizes the vector.
	Steering.Direction = Target.Position - Agent.GetPosition();
	return Steering;
}

// Flee
SteeringOutput Flee::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.Direction = Agent.GetPosition() - Target.Position;
	return Steering;
}

// Arrive
SteeringOutput Arrive::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	// Preserving the old speed if relevant
	if (Agent.GetMaxLinearSpeed() > Agent.OldSpeed) Agent.OldSpeed = Agent.GetMaxLinearSpeed();

	// Drawing debug circles
	DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), TargetRadius, 32, FColor::Blue, false, 0.025f, 0, 5, FVector(0, 1, 0), FVector(1, 0, 0), false);
	DrawDebugCircle(Agent.GetWorld(), Agent.GetActorLocation(), SlowRadius, 32, FColor::Orange, false, 0.025f, 0, 5, FVector(0, 1, 0), FVector(1, 0, 0), false);

	SteeringOutput Steering{};
	// Returning early if target is in the origin
	if (Target.Position.Length() < 1.f) return Steering;// Not updating
	// Performing steering logic
	Steering.Direction = Target.Position - Agent.GetPosition();// Norm is distance
	double const Distance{ Steering.Direction.Length() };
	
	// Returning early if closer than TargetRadius
	if (Distance < TargetRadius)
	{
		Agent.SetMaxLinearSpeed(0.f);
		return Steering;
	}
	
	// Clamping the distance between the target and slow radii
	// NOTE: The clamping is performed in a way that the distances smaller or equal to TargetRadius get equated to 0.
	double const ClampedDistance{ FMath::Clamp(Distance - TargetRadius, 0, SlowRadius - TargetRadius )};
	double const SpeedFactor{ ClampedDistance / (SlowRadius - TargetRadius) };

	// Updating the agent's max speed
	Agent.SetMaxLinearSpeed(SpeedFactor * Agent.OldSpeed);
	
	return Steering;
}

// Face
SteeringOutput Face::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	return Steering;
}

// Pursuit 
SteeringOutput Pursuit::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	return Steering;
}

// Evade 
SteeringOutput Evade::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	return Steering;
}

// Wander
SteeringOutput Wander::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	return Steering;
}
