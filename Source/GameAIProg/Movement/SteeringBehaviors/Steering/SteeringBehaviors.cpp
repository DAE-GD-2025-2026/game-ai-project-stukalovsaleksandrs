#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

// Seek
SteeringOutput Seek::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.Direction = Target.Position - Agent.GetPosition();
	// NOTE: There is no need to normalize the Direction, because AddMovementInput already normalizes the vector.
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
	SteeringOutput Steering{};
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
