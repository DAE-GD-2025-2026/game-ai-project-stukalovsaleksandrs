#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
SteeringOutput Seek::CalculateSteering(float const DeltaTime, ASteeringAgent& Agent)
{
	SteeringOutput Steering{};
	Steering.Direction = Target.Position - Agent.GetPosition();
	// NOTE: There is no need to normalize the Direction, because AddMovementInput already normalizes the vector.
	return Steering;
}
